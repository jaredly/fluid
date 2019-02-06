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

void fluid_Window_hide(value window_v) {
  CAMLparam1(window_v);
  log("hide window\n");
  FluidWindow* window = (FluidWindow*)Unwrap(window_v);
  [window orderOut:nil];
  CAMLreturn0;
}

void fluid_Window_position(value window_v, value pos_v) {
  CAMLparam1(window_v);
  Unpack_record2_double(pos_v, x, y);

  log("move window\n");
  FluidWindow* window = (FluidWindow*)Unwrap(window_v);
  [window setFrameTopLeftPoint:CGPointMake(x, y)];
  CAMLreturn0;
}

void fluid_Window_resize(value window_v, value size_v) {
  CAMLparam1(window_v);
  Unpack_record2_double(size_v, width, height);

  log("resize window\n");
  FluidWindow* window = (FluidWindow*)Unwrap(window_v);
  // printf("Resize %f\n", heightChange);
  NSRect frameRect = [NSWindow frameRectForContentRect:NSMakeRect(
    window.frame.origin.x,
    window.frame.origin.y,
    width,
    height
  ) styleMask:window.styleMask];
  float heightChange = frameRect.size.height - window.frame.size.height;
  frameRect.origin.y -= heightChange;
  [window setFrame:frameRect display:YES];
  CAMLreturn0;
}

void fluid_Window_close(value window_v) {
  CAMLparam1(window_v);
  log("close window\n");
  FluidWindow* window = (FluidWindow*)Unwrap(window_v);
  log("Ok here\n");
  [window close];
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
  int onBlur;
}

- (instancetype)initWithOnBlur:(int)onBlurv {
  if (self = [super init]) {
    onBlur = onBlurv;
    // if (Check_optional(onBlur)) {
    //   log("Has a blur\n");
    // } else {
    //   log("Doesn't have a blur\n");
    // }
  }
  return self;
}

- (void)windowDidResignKey:(NSNotification *) notification {
  CAMLparam0();
  CAMLlocal1(window_v);
  log("Window blur\n");

  Wrap(window_v, [notification object]);

  static value * closure_f = NULL;
  if (closure_f == NULL) {
      closure_f = caml_named_value("fluid_window");
  }
  caml_callback2(*closure_f, Val_int(onBlur), window_v);
  // if (Check_optional(onBlur)) {
  //   caml_callback(Unpack_optional(onBlur), window_v);
  // } else {
  //   log("No blur handler\n");
  // }

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
    mask =
    // NSWindowStyleMaskBorderless;
      NSWindowStyleMaskTitled |
      NSWindowStyleMaskFullSizeContentView;
  } else {
    mask = 
      NSWindowStyleMaskClosable |
      NSWindowStyleMaskMiniaturizable |
      NSWindowStyleMaskResizable |
      NSWindowStyleMaskTitled;
  }

  WindowDelegate* delegate = [[WindowDelegate alloc] initWithOnBlur:Int_val(onBlur_v)];

  FluidWindow* window = [[FluidWindow alloc]
    initWithContentRect: NSMakeRect(left, top, width, height)
    styleMask: mask
    backing: NSBackingStoreBuffered
    defer: NO];
  if (isFloating == Val_true) {
    [window setBackgroundColor:[NSColor whiteColor]];
    [window setOpaque:NO];
    [window setTitlebarAppearsTransparent:YES];
    [window setTitleVisibility:NSWindowTitleHidden];
    [window standardWindowButton:NSWindowCloseButton].hidden = YES;
    [window setShowsToolbarButton:NO];
  }
  window.delegate = delegate;
  window.contentView = [[FlippedView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];

  [window setTitle:NSString_val(title_v)];

  if (isFloating == Val_true) {
    [window setLevel:NSStatusWindowLevel];
  }

  Wrap(window_v, window);
  CAMLreturn(window_v);
}
