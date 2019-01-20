#include "./fluid_shared.h"

@interface FluidWindow : NSWindow
@end

@implementation FluidWindow
{}

- (BOOL)canBecomeKeyWindow { 
    return YES; 
}

- (BOOL)canBecomeMainWindow { 
    return YES; 
}

@end

CAMLprim value fluid_Window_contentView(value window_v) {
  CAMLparam1(window_v);
  CAMLlocal1(contentView_v);
  log("Get content view\n");
  FluidWindow* window = (FluidWindow*)Unwrap(window_v);
  Wrap(contentView_v, window.contentView);
  CAMLreturn(contentView_v);
}

void fluid_Window_activate(value window_v) {
  CAMLparam1(window_v);
  FluidWindow* window = (FluidWindow*)Unwrap(window_v);

  [window makeKeyAndOrderFront:window];
  [window orderFrontRegardless];

  if ([window canBecomeMainWindow]) {
    [window makeMainWindow];
  }

  [NSApp unhide:nil];
  [NSApp activateIgnoringOtherApps:true];
  CAMLreturn0;
}

void fluid_Window_center(value window_v) {
  CAMLparam1(window_v);
  log("Center window\n");
  FluidWindow* window = (FluidWindow*)Unwrap(window_v);
  [window center];
  CAMLreturn0;
}

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate {
  value onBlur;
}

- (instancetype)initWithOnBlur:(value)onBlurv {
  if (self = [super init]) {
    onBlur = onBlurv;
    caml_register_global_root(&onBlurv);
  }
  return self;
}

- (void)windowDidResignKey:(NSNotification *)__unused not {
  CAMLparam0();
  CAMLlocal1(window_v);
  log("Window blur\n");

  if (Check_optional(onBlur)) {
    Wrap(window_v, self);
    caml_callback(Unpack_optional(onBlur), window_v);
  }

  CAMLreturn0;
}
@end


CAMLprim value fluid_Window_make(value title_v, value onBlur_v, value dims_v, value isFloating) {
  CAMLparam4(title_v, onBlur_v, dims_v, isFloating);
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

  WindowDelegate* delegate = [[WindowDelegate alloc] initWithOnBlur:onBlur_v];

  FluidWindow* window = [[FluidWindow alloc]
    initWithContentRect: NSMakeRect(left, top, width, height)
    styleMask: mask
    backing: NSBackingStoreBuffered
    defer: NO];
  window.delegate = delegate;
  window.contentView = [[FlippedView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];

  [window setTitle:NSString_val(title_v)];

  if (isFloating == Val_true) {
    [window setLevel:NSStatusWindowLevel];
  }

  Wrap(window_v, window);
  CAMLreturn(window_v);
}
