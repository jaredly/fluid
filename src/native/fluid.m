#include "./fluid_shared.h"


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

  CAMLparam0();
  CAMLlocal1(contentView_v);
  Wrap(contentView_v, window.contentView);
  caml_callback(onLaunch, contentView_v);

  [window makeKeyAndOrderFront:window];
  [window center];
  [window makeMainWindow];
  [NSApp unhide:nil];

  CAMLreturn0;
}

@end






void fluid_startApp (value title_v, value size, value callback)
{
  CAMLparam3(title_v, size, callback);
  NSString *title = [NSString stringWithUTF8String:String_val (title_v)];

  float width = Double_val(Field(size, 0));
  float height = Double_val(Field(size, 1));


  @autoreleasepool {

    [NSApplication sharedApplication];

    NSWindow* window = [[NSWindow alloc]
      initWithContentRect: NSMakeRect(0, 0, width, height)
      styleMask: (
        NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable |
        NSWindowStyleMaskTitled
      )
      backing: NSBackingStoreBuffered
      defer: NO];
    window.contentView = [[FlippedView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];

    [window setTitle:title];
    [window center];
    
    MLApplicationDelegate* delegate = [[MLApplicationDelegate alloc]
      initWithOnLaunch:callback window:window title:title];

    [NSApp setDelegate: delegate];

    [NSApp activateIgnoringOtherApps:true];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    [NSApp run];
  }

  CAMLreturn0;
}
