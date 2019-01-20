#include "./fluid_shared.h"

CAMLprim value fluid_Window_contentView(value window_v) {
  CAMLparam1(window_v);
  CAMLlocal1(contentView_v);
  log("Get content view\n");
  NSWindow* window = (NSWindow*)Unwrap(window_v);
  Wrap(contentView_v, window.contentView);
  CAMLreturn(contentView_v);
}

void fluid_Window_activate(value window_v) {
  CAMLparam1(window_v);
  NSWindow* window = (NSWindow*)Unwrap(window_v);

  [window makeKeyAndOrderFront:window];
  [window orderFrontRegardless];

  if ([window canBecomeMainWindow]) {
    [window makeMainWindow];
  }

  [NSApp unhide:nil];
  [NSApp activateIgnoringOtherApps:true];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  CAMLreturn0;
}

void fluid_Window_center(value window_v) {
  CAMLparam1(window_v);
  log("Center window\n");
  NSWindow* window = (NSWindow*)Unwrap(window_v);
  [window center];
  CAMLreturn0;
}

CAMLprim value fluid_Window_make(value title_v, value dims_v, value isFloating) {
  CAMLparam3(title_v, dims_v, isFloating);
  CAMLlocal1(window_v);
  log("Make window\n");

  Unpack_record4_double(dims_v, left, top, width, height);

  NSWindowStyleMask mask;
  if (isFloating == Val_true) {
    mask = NSWindowStyleMaskBorderless;
  } else {
    mask = 
      NSWindowStyleMaskClosable |
      NSWindowStyleMaskMiniaturizable |
      NSWindowStyleMaskResizable |
      NSWindowStyleMaskTitled;
  }

  NSWindow* window = [[NSWindow alloc]
    initWithContentRect: NSMakeRect(left, top, width, height)
    styleMask: mask
    backing: NSBackingStoreBuffered
    defer: NO];
  window.contentView = [[FlippedView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];

  [window setTitle:NSString_val(title_v)];

  if (isFloating == Val_true) {
    [window setLevel:NSStatusWindowLevel];
  }

  Wrap(window_v, window);
  CAMLreturn(window_v);
}
