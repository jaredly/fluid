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
  return NO;
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not {
  log("Finish Launching\n");
  caml_callback(onLaunch, Val_unit);
  [NSApp unhide:nil];
  [NSApp activateIgnoringOtherApps:true];
  caml_remove_global_root(&onLaunch);
}

@end

@interface MenuDelegate : NSObject

@end

@implementation MenuDelegate { }

- (void)dummySelect {
  // nothing
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
  id hideOthersMenuItem = [[NSMenuItem alloc] initWithTitle:@"Hide Them"
                                                     action:@selector(hideOtherApplications:)
                                              keyEquivalent:@"h"];
  [hideOthersMenuItem setKeyEquivalentModifierMask:(NSEventModifierFlagOption | NSEventModifierFlagCommand)];
  id closeMenuItem = [[NSMenuItem alloc] initWithTitle:@"Close window "
                                               action:@selector(terminate:)
                                        keyEquivalent:@"w"];
  id quitMenuItem = [[NSMenuItem alloc] initWithTitle:[@"Quit " stringByAppendingString:appName]
                                               action:@selector(terminate:)
                                        keyEquivalent:@"q"];
  [appMenu addItem:aboutMenuItem];
  [appMenu addItem:[NSMenuItem separatorItem]];
  // [appMenu addItem:hideMenuItem];
  // [appMenu addItem:hideOthersMenuItem];
  [appMenu addItem:[NSMenuItem separatorItem]];
  [appMenu addItem:closeMenuItem];
  [appMenu addItem:quitMenuItem];

  MenuDelegate* delegate = [MenuDelegate alloc];

  id menu2 = [NSMenu new];
  [menu2 addItem:[NSMenuItem separatorItem]];
  [menu2 addItem:[[NSMenuItem alloc] initWithTitle:@"Go places" action:@selector(terminate:) keyEquivalent:@"q"]];
  // // id menuTitle2 = [[NSMenuItem alloc] initWithTitle:@"Yeahp" action:nil keyEquivalent:@""];
  // id menuTitle2 = [NSMenuItem new];
  // [menuTitle2 setTitle:@"Yes"];
  // [menuTitle2 setSubmenu:menu2];

  // id appMenuItem = [NSMenuItem new];
  // [appMenuItem setSubmenu:appMenu];

  id menubar = [NSMenu new];
  NSMenuItem* appMenuItem = [menubar addItemWithTitle:@"Hello Folks" action:@selector(dummySelect) keyEquivalent:@"M"];
  [appMenuItem setSubmenu:appMenu];
  appMenuItem.target = delegate;
  // [menubar addItem:appMenuItem];

  NSMenuItem* item2 = [menubar addItemWithTitle:@"Yes" action:@selector(dummySelect) keyEquivalent:@""];
  item2.target = delegate;
  [menubar setSubmenu:menu2 forItem:item2];

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

@interface StatusClickTarget : NSObject
- (instancetype)initWithOnClick:(value)onClickv andItem:(NSStatusItem*)item;
@end

@implementation StatusClickTarget {
  value onClick;
  NSStatusItem* item;
}

- (instancetype)initWithOnClick:(value)onClickv andItem:(NSStatusItem*)itemv {
  if (self = [super init]) {
    onClick = onClickv;
    caml_register_global_root(&onClickv);
    item = itemv;
  }
  return self;
}

- (void)onClick {
  CAMLparam0();
  CAMLlocal1(pair);
  Create_double_pair(
    pair,
    item.button.window.frame.origin.x,
    item.button.window.frame.origin.y
  );
  
  caml_callback(onClick, pair);
  CAMLreturn0;
}
@end

void fluid_App_statusBarItem(value title_v, value onClick_v) {
  CAMLparam2(title_v, onClick_v);
  NSStatusItem* item = [NSStatusBar.systemStatusBar statusItemWithLength:NSSquareStatusItemLength];
  [item retain];
  item.button.title = NSString_val(title_v);
  StatusClickTarget* target = [[StatusClickTarget alloc] initWithOnClick:onClick_v andItem:item];
  item.button.target = target;
  item.button.action = @selector(onClick);
  // NSLog(@"Made an item %f, %f", item.button.window.frame.origin.x, item.button.window.frame.origin.y);
  CAMLreturn0;
}
