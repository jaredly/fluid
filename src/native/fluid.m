// #include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>

#define CAML_NAME_SPACE

#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/callback.h>

// #include <string.h> // for memcpy
#include <caml/alloc.h>

@interface FlippedView : NSView
{
}
- (BOOL)isFlipped;
@end

@implementation FlippedView : NSView
- (BOOL)isFlipped {
  return YES;
}
@end

@interface MLButtonDelegate : NSObject

@end

@implementation MLButtonDelegate
{
  value onPress_v;
}

- (instancetype)initWithOnPress:(value)onPressv
{
  if (self = [super init]) {
    onPress_v = onPressv;
  }
  return self;
}

- (void)onPress {
  // value v = onPress_v;
  caml_callback(onPress_v, Val_unit);
}

@end

@interface MLApplicationDelegate : NSObject <NSApplicationDelegate>

@end

@implementation MLApplicationDelegate
{
  value onLaunch;
  NSWindow* window;
  NSString* title;
}

- (instancetype)initWithOnLaunch:(value)onLaunchv window:(NSWindow*)windowv title:(NSString*)titlev
{
  if (self = [super init]) {
    onLaunch = onLaunchv;
    window = windowv;
    title = titlev;
  }
  return self;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSNotification *)notification
{
  return YES;
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not
{

  printf("4\n");

  id menubar = [NSMenu new];
  id appMenuItem = [NSMenuItem new];
  [menubar addItem:appMenuItem];
  [NSApp setMainMenu:menubar];
  id appMenu = [NSMenu new];
  id appName = title;
  id aboutMenuItem = [[NSMenuItem alloc] initWithTitle:[@"About " stringByAppendingString:appName]
                                                action:@selector(orderFrontStandardAboutPanel:)
                                         keyEquivalent:@""];
  id hideMenuItem = [[NSMenuItem alloc] initWithTitle:[@"Hide " stringByAppendingString:appName]
                                               action:@selector(hide:)
                                        keyEquivalent:@"h"];
  id hideOthersMenuItem = [[NSMenuItem alloc] initWithTitle:@"Hide Others"
                                                     action:@selector(hideOtherApplications:)
                                              keyEquivalent:@"h"];
  [hideOthersMenuItem setKeyEquivalentModifierMask:(NSEventModifierFlagOption | NSEventModifierFlagCommand)];
  id showAllMenuItem = [[NSMenuItem alloc] initWithTitle:@"Show All"
                                                  action:@selector(unhideAllApplications:)
                                           keyEquivalent:@""];
  id closeMenuItem = [[NSMenuItem alloc] initWithTitle:@"Close window "
                                               action:@selector(terminate:)
                                        keyEquivalent:@"w"];
  id quitMenuItem = [[NSMenuItem alloc] initWithTitle:[@"Quit " stringByAppendingString:appName]
                                               action:@selector(terminate:)
                                        keyEquivalent:@"q"];
  [appMenu addItem:aboutMenuItem];
  [appMenu addItem:[NSMenuItem separatorItem]];
  [appMenu addItem:hideMenuItem];
  [appMenu addItem:hideOthersMenuItem];
  [appMenu addItem:showAllMenuItem];
  [appMenu addItem:[NSMenuItem separatorItem]];
  [appMenu addItem:closeMenuItem];
  [appMenu addItem:quitMenuItem];
  [appMenuItem setSubmenu:appMenu];


  window.contentView = [[FlippedView alloc] initWithFrame:NSMakeRect(0, 0, 800, 500)];
  printf("5\n");
  caml_callback(onLaunch, (value) (window.contentView));
  printf("6\n");

  [window makeKeyAndOrderFront:window];
  [window center];
  [window makeMainWindow];
  [NSApp unhide:nil];
}

@end







// @interface Win : NSWindow
// {
// }
// - (void)applicationDidFinishLaunching: (NSNotification *)notification;
// - (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSNotification *)notification;
// @end
 
 
// @implementation Win : NSWindow
// -(instancetype) init
// {
//   if ((self = [super 
//     initWithContentRect: NSMakeRect(0, 0, 800, 600)
//     styleMask: (NSTitledWindowMask | NSClosableWindowMask)
//     backing: NSBackingStoreBuffered
//     defer: NO])) {
 
//     [self setTitle: @"A Window"];
//     [self center];
//   }
//   return self;
// }
 
// - (void)applicationDidFinishLaunching: (NSNotification *)notification
// {
//   [self orderFront: self];
// }
 
// - (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSNotification *)notification
// {
//   return YES;
// }
// @end
 



// @interface Win : NSWindow
// {
// }
// - (void)applicationDidFinishLaunching: (NSNotification *)notification;
// - (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSNotification *)notification;
// @end


// @implementation Win : NSWindow

// - (void)applicationDidFinishLaunching: (NSNotification *)notification
// {
//   [self orderFront: self];
// }

// - (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSNotification *)notification
// {
//   return YES;
// }
// @end

// @implementation Win : NSWindow

// - (void)applicationDidFinishLaunching: (NSNotification *)notification
// {
//   [self orderFront: self];
// }

// - (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSNotification *)notification
// {
//   return YES;
// }
// @end









// #define Val_NSWindow(v) ((value)(v))
// #define NSWindow_val(v) ((NSWindow *)(v))

void fluid_startApp (value title_v, value callback)
{
  CAMLparam2 (title_v, callback);
  NSString *title = [NSString stringWithUTF8String:String_val (title_v)];


  printf("1\n");

  @autoreleasepool {

    printf("2\n");
    [NSApplication sharedApplication];

    NSWindow* window = [[NSWindow alloc]
      initWithContentRect: NSMakeRect(0, 0, 800, 600)
      styleMask: (
        NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable |
        NSWindowStyleMaskTitled
      )
      backing: NSBackingStoreBuffered
      defer: NO];

    [window setTitle:title];
    [window center];
    
    MLApplicationDelegate* delegate = [[MLApplicationDelegate alloc]
    initWithOnLaunch:callback window:window title:title];

    [NSApp setDelegate: delegate];

    [NSApp activateIgnoringOtherApps:true];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    printf("3\n");
    [NSApp run];
  }

  CAMLreturn0;
}

void fluid_NSView_appendChild(value view_v, value child_v) {
  CAMLparam2(view_v, child_v);
  printf("Append Child\n");
  NSView* view = (NSView*) view_v;
  NSView* child = (NSView*) child_v;

  printf("Before\n");

  NSView* last = view.subviews.lastObject;
  if (last != nil) {
    [view addSubview:child positioned:NSWindowBelow relativeTo:last];
  } else {
    [view addSubview:child];
  }
  printf("After\n");

  CAMLreturn0;
}

void fluid_NSView_removeChild(value view_v, value child_v) {
  CAMLparam2(view_v, child_v);
  printf("Remove Child\n");
  NSView* view = (NSView*) view;
  NSView* child = (NSView*) child;

  if (child.superview == view) {
    [child removeFromSuperview];
  }

  CAMLreturn0;
}

void fluid_NSView_replaceWith(value view_v, value replace_v) {
  CAMLparam2(view_v, replace_v);
  printf("Replace Child\n");
  NSView* view = (NSView*) view_v;
  NSView* replace = (NSView*) replace_v;

  if (view.superview != nil) {
    [view.superview replaceSubview:view with:replace];
  }

  CAMLreturn0;
}

CAMLprim value fluid_measureText(value text_v, value font_v, value fontSize_v) {
  CAMLparam3(text_v, font_v, fontSize_v);
  CAMLlocal1(result);

  NSString *textContent = [NSString stringWithUTF8String:String_val (text_v)];
  NSString *font = [NSString stringWithUTF8String:String_val (font_v)];
  double fontSize = Double_val(fontSize_v);

  NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont fontWithName:font size:fontSize], NSFontAttributeName, nil];
  NSAttributedString *text = [[NSAttributedString alloc] initWithString:textContent attributes: attributes];
  NSSize textSize = [text size];

  result = caml_alloc_tuple(2);
  Store_field (result, 0, caml_copy_double(textSize.width));
  Store_field (result, 1, caml_copy_double(textSize.height));

  CAMLreturn(result);
}

CAMLprim value fluid_create_NSButton(value title_v, value onPress_v, value pos_v, value size_v) {
  CAMLparam4(title_v, onPress_v, pos_v, size_v);
  printf("Create button\n");

  NSString *title = [NSString stringWithUTF8String:String_val (title_v)];

  MLButtonDelegate* delegate = [[MLButtonDelegate alloc] initWithOnPress:onPress_v];

  NSButton* button = [NSButton buttonWithTitle:title target:delegate action:@selector(onPress)];

  float top = Double_val(Field(pos_v, 0));
  float left = Double_val(Field(pos_v, 1));
  float width = Double_val(Field(size_v, 0));
  float height = Double_val(Field(size_v, 1));

  [button setFrameOrigin:NSMakePoint(left, top)];
  [button setFrameSize:NSMakeSize(width, height)];

  CAMLreturn((value) button);
}

CAMLprim value fluid_create_NSTextView(value contents_v, value pos_v, value size_v) {
  CAMLparam3(contents_v, pos_v, size_v);

  NSString *contents = [NSString stringWithUTF8String:String_val (contents_v)];
  NSTextField* text = [NSTextField labelWithString:contents];

  // NSMutableAttributedString *attrstr = [[NSMutableAttributedString alloc] initWithString:contents];
  // NSDictionary *attributes = @{
  //                             NSForegroundColorAttributeName : [NSColor blueColor],
  //                             NSFontAttributeName : [NSFont fontWithName:@"Ariel" size:16.f]
  //                             };
  // [attrstr setAttributes:attributes range:NSMakeRange(0, contents.length)];
  // text.attributedStringValue = attrstr;

  float top = Double_val(Field(pos_v, 0));
  float left = Double_val(Field(pos_v, 1));
  float width = Double_val(Field(size_v, 0));
  float height = Double_val(Field(size_v, 1));


  printf("Create text view %s, %f,%f %f x %f\n", String_val(contents_v), left, top, width, height);

  [text setFrameOrigin:NSMakePoint(left, top)];
  [text setFrameSize:NSMakeSize(width, height)];

  // NSView* view = [[NSView alloc] initWithFrame:NSMakeRect(left, top, width, height)];
  // [view addSubview:text];

  CAMLreturn((value) text);
}

CAMLprim value fluid_create_NSView(value onPress_v, value pos_v, value size_v) {
  CAMLparam3(onPress_v, pos_v, size_v);

  float top = Double_val(Field(pos_v, 0));
  float left = Double_val(Field(pos_v, 1));
  float width = Double_val(Field(size_v, 0));
  float height = Double_val(Field(size_v, 1));
  printf("Create view %f,%f %f x %f\n", top, left, width, height);

  NSView* view = [[FlippedView alloc] initWithFrame:NSMakeRect(left, top, width, height)];
  view.layer.backgroundColor = CGColorCreateGenericRGB(255, 1, 0, 1);

  CAMLreturn((value) view);
}

void fluid_set_NSTextView_textContent(value text_v, value contents_v) {
  CAMLparam2(text_v, contents_v);
  printf("set text contents\n");

  NSTextField* text = (NSTextField*)text_v;
  NSString *contents = [NSString stringWithUTF8String:String_val (contents_v)];

  NSMutableAttributedString *attrstr = [[NSMutableAttributedString alloc] initWithString:contents];
  NSDictionary *attributes = @{
                              NSForegroundColorAttributeName : [NSColor blueColor],
                              NSFontAttributeName : [NSFont fontWithName:@"Ariel" size:16.f]
                              };
  [attrstr setAttributes:attributes range:NSMakeRange(0, contents.length)];

  text.attributedStringValue = attrstr;

  CAMLreturn0;
}
