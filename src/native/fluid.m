#include "./fluid_shared.h"


@interface MLApplicationDelegate : NSObject <NSApplicationDelegate>
@end

@implementation MLApplicationDelegate {
  value onLaunch;
}

- (instancetype)initWithOnLaunch:(value)onLaunchv {
  if (self = [super init]) {
    onLaunch = onLaunchv;
  }
  return self;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSNotification *)notification {
  return YES;
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not {
  log("Finish Launching\n");
  caml_callback(onLaunch, Val_unit);
  [NSApp unhide:nil];
  [NSApp activateIgnoringOtherApps:true];
  caml_remove_global_root(&onLaunch);
}

@end

void fluid_App_setupMenu(value title_v) {
  CAMLparam1(title_v);
  id appMenu = [NSMenu new];
  id appName = NSString_val(title_v);
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

  id appMenuItem = [NSMenuItem new];
  // id appMenuItem = [[NSMenuItem alloc] initWithTitle:title action:nil keyEquivalent:@""];
  [appMenuItem setSubmenu:appMenu];

  // id menu2 = [NSMenu new];
  // [menu2 addItem:[NSMenuItem separatorItem]];
  // [menu2 addItem:[[NSMenuItem alloc] initWithTitle:@"Go places" action:@selector(terminate:) keyEquivalent:@"q"]];
  // // id menuTitle2 = [[NSMenuItem alloc] initWithTitle:@"Yeahp" action:nil keyEquivalent:@""];
  // id menuTitle2 = [NSMenuItem new];
  // [menuTitle2 setTitle:@"Yes"];
  // [menuTitle2 setSubmenu:menu2];

  id menubar = [NSMenu new];
  [menubar addItem:appMenuItem];
  // id item2 = [menubar addItemWithTitle:@"Yes" action:nil keyEquivalent:@""];
  // [menubar setSubmenu:menu2 forItem:item2];

  [NSApp setMainMenu:menubar];
  CAMLreturn0;
}





void fluid_App_launch (value isAccessory, value callback)
{
  CAMLparam2(isAccessory, callback);

  @autoreleasepool {

    [NSApplication sharedApplication];
    
    MLApplicationDelegate* delegate = [[MLApplicationDelegate alloc] initWithOnLaunch:callback];

    if (isAccessory == Val_true) {
      [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
    } else {
      [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    }

    [NSApp setDelegate: delegate];

    [NSApp run];
  }

  CAMLreturn0;
}
