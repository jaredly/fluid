#include "./fluid_shared.h"

@interface MenuDelegate : NSObject
- (void)dummySelect;
@end

@implementation MenuDelegate { }
- (void)dummySelect {
  NSLog(@"Selected the menu thing");
  // nothing
}
@end

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

- (void)applicationDidFinishLaunching:(NSNotification *)__unused not {
  log("Finish Launching\n");
  caml_callback(onLaunch, Val_unit);
  [NSApp unhide:nil];
  [NSApp activateIgnoringOtherApps:true];
  // caml_remove_global_root(&onLaunch);
}

@end

CAMLprim value fluid_App_menuItem(value title_v, value action_v, value shortcut_v) {
  CAMLparam3(title_v, action_v, shortcut_v);
  CAMLlocal1(item_v);
  SEL action;
  if (Tag_val(action_v) == 1) {
    action = NSSelectorFromString(NSString_val(Field(action_v, 0)));
  } else {
    // Call
    action = @selector(onClick);
  }
  NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:NSString_val(title_v)
                                              action:action
                                              keyEquivalent:NSString_val(shortcut_v)];
  if (Tag_val(action_v) == 0) {
    int onClick = Int_val(Field(action_v, 0));
    // caml_register_global_root(&onClick);
    ClickTarget* delegate = [[ClickTarget alloc] initWithOnClick:onClick];
    item.target = delegate;
  }
  Wrap(item_v, item);

  CAMLreturn(item_v);
}

CAMLprim value fluid_App_separatorItem() {
  CAMLparam0();
  CAMLlocal1(item_v);
  NSMenuItem* item = [NSMenuItem separatorItem];
  Wrap(item_v, item);
  CAMLreturn(item_v);
}

CAMLprim value fluid_App_menu(value title_v, value items_v) {
  CAMLparam2(title_v, items_v);
  CAMLlocal1(menu_v);
  NSMenu* menu = [[NSMenu alloc] initWithTitle:NSString_val(title_v)];

  int length = Wosize_val(items_v);
  for (int i=0; i<length; i++) {
    [menu addItem:(NSMenuItem*)Unwrap(Field(items_v, i))];
  }

  NSMenuItem* wrapper = [[NSMenuItem alloc] initWithTitle:NSString_val(title_v) action:@selector(dummySelect) keyEquivalent:@""];
  MenuDelegate* delegate = [MenuDelegate alloc];
  wrapper.target = delegate;
  [wrapper setSubmenu:menu];

  Wrap(menu_v, wrapper);
  CAMLreturn(menu_v);
}

void fluid_App_setupAppMenu(value title_v, value appItems_v, value menus_v) {
  CAMLparam3(title_v, appItems_v, menus_v);


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
  int appItems_length = Wosize_val(appItems_v);
  if (appItems_length > 0) {
    [appMenu addItem:[NSMenuItem separatorItem]];
    for (int i=0; i<appItems_length; i++) {
      [appMenu addItem:(NSMenuItem*)Unwrap(Field(appItems_v, i))];
    }
  }
  [appMenu addItem:[NSMenuItem separatorItem]];
  [appMenu addItem:closeMenuItem];
  [appMenu addItem:quitMenuItem];

  NSMenuItem* appMenuItem = [NSMenuItem new];
  [appMenuItem setSubmenu:appMenu];

  id menubar = [NSMenu new];
  [menubar addItem:appMenuItem];

  int menus_length = Wosize_val(menus_v);
  for (int i=0; i<menus_length; i++) {
    [menubar addItem:(NSMenuItem*)Unwrap(Field(menus_v, i))];
  }

  [NSApp setMainMenu:menubar];

  CAMLreturn0;
}

void fluid_App_deactivate() {
  [NSApp deactivate];
}

void fluid_App_hide() {
  [NSApp hide:nil];
}

void fluid_App_setTimeout(value callback, value time) {
  CAMLparam2(callback, time);
  log("Set timeout\n");
  dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int_val(time)), dispatch_get_main_queue(), ^(void){
    caml_callback(callback, Val_unit);
  });
  CAMLreturn0;
}

void fluid_App_triggerString(value string_v) {
  CAMLparam1(string_v);

  NSString* string = NSString_val(string_v);

  // From https://stackoverflow.com/questions/9318690/generate-keyboard-events-for-the-frontmost-application

  // First, get the PSN of the currently front app
  ProcessSerialNumber psn;
  GetFrontProcess( &psn );

  // make some key events
  CGEventRef keyup, keydown;
  // int count = Wosize_val(keys);
  // for (int i=0; i<count; i++) {
  //   int key = Int_val(Field(keys, i));
    keydown = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)0, true);
    CGEventKeyboardSetUnicodeString(keydown, [string length], (const unichar*)[string cStringUsingEncoding:NSUnicodeStringEncoding]);
    keyup = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)0, false);

    // forward them to the frontmost app
    CGEventPostToPSN (&psn, keydown);
    CGEventPostToPSN (&psn, keyup);

    // and finally behave friendly
    CFRelease(keydown);
    CFRelease(keyup);
  // }
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

// @interface StatusBarButton : NSButton
//   @property (nonatomic) int onClick; 
//   @property (nonatomic) int onRightClick; 
// @end

@interface StatusClickTarget : NSObject
- (instancetype)initWithOnClick:(int)onClickv andItem:(NSStatusItem*)item;
@end

@implementation StatusClickTarget {
  int onClick;
  NSStatusItem* item;
}

- (instancetype)initWithOnClick:(int)onClickv andItem:(NSStatusItem*)itemv {
  if (self = [super init]) {
    onClick = onClickv;
    item = itemv;
  }
  return self;
}

- (void)onClick {
  callPos(
    onClick,
    item.button.window.frame.origin.x,
    item.button.window.frame.origin.y
  );
}
@end

CAMLprim value fluid_App_statusBarPos(value item_v) {
  CAMLparam1(item_v);
  CAMLlocal1(pos_v);

  NSStatusItem* item = (NSStatusItem*)Unwrap(item_v);
  Create_record2_double(pos_v,
    item.button.window.frame.origin.x,
    item.button.window.frame.origin.y
  );
  CAMLreturn(pos_v);
}

CAMLprim value fluid_App_statusBarItem(value title_v, value onClick_v) {
  CAMLparam2(title_v, onClick_v);
  CAMLlocal1(statusBar_v);
  NSStatusItem* item = [NSStatusBar.systemStatusBar statusItemWithLength:NSSquareStatusItemLength];
  [item retain];
  item.button.title = NSString_val(title_v);
  StatusClickTarget* target = [[StatusClickTarget alloc] initWithOnClick:Int_val(onClick_v) andItem:item];
  item.button.target = target;
  item.button.action = @selector(onClick);
  // NSLog(@"Made an item %f, %f", item.button.window.frame.origin.x, item.button.window.frame.origin.y);
  Wrap(statusBar_v, item);
  CAMLreturn(statusBar_v);
}

NSData* dataForText(NSString* text) {
  NSFont* nsFont = [NSFont systemFontOfSize:8.0];
  NSDictionary* attributes = @{NSFontAttributeName: nsFont};
  NSImage *img = [[NSImage alloc] initWithSize:[text sizeWithAttributes:attributes]];
  [img lockFocus];
  [text drawAtPoint:NSMakePoint(10, 10) withAttributes:attributes];
  [img unlockFocus];
  return [img TIFFRepresentation];
}

BOOL isEmojiSupported(NSString* text) {
  static NSData * UNAVAILABLE = NULL;
  if (UNAVAILABLE == NULL) {
    UNAVAILABLE = dataForText(@"\u1fff");
  }
  return ![dataForText(text) isEqualToData:UNAVAILABLE];
}

CAMLprim value fluid_App_isEmojiSupported(value text) {
  CAMLparam1(text);
  CAMLreturn(isEmojiSupported(NSString_val(text)) ? Val_true : Val_false);
}

CAMLprim value fluid_App_homeDirectory() {
  CAMLparam0();
  CAMLreturn(caml_copy_string([NSHomeDirectory() UTF8String]));
}