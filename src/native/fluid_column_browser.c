#include "./fluid_shared.h"


@interface BrowserDelegate: NSObject <NSBrowserDelegate>
  @property (nonatomic) int childrenCount;
  @property (nonatomic) int childOfItem;
  @property (nonatomic) int isLeafItem;
  @property (nonatomic) int displayForItem;
@end

@implementation BrowserDelegate {
  NSMutableDictionary<NSString *, NSString *> *objects;
}

- (instancetype)init {
  if (self = [super init]) {
    objects = [[NSMutableDictionary alloc] init];
    return self;
  }
  return nil;
}

- (id)objectForId:(NSString *)key {
  NSString *obj = [objects objectForKey:key];
  if (obj == nil) {
    obj = key;
    [objects setObject:key forKey:key];
  }
  return obj;
}

- (id)rootItemForBrowser:(NSBrowser *)browser {
  return [self objectForId:@""];
}

- (NSInteger)browser:(NSBrowser *)browser numberOfChildrenOfItem:(id)item {
  NSString *item_s = (NSString *)item;

  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_children_count");
  }

  return Int_val(caml_callback2(*closure_f, Val_int(self.childrenCount), Val_NSString(item_s)));
}

- (id)browser:(NSBrowser *)browser child:(NSInteger)index ofItem:(id)item {
  CAMLparam0();
  CAMLlocal1(tuple_v);

  NSString *item_s = (NSString *)item;

  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_child_of_item");
  }
  tuple_v = caml_alloc_tuple(2);
  Store_field(tuple_v, 0, Val_NSString(item_s));
  Store_field(tuple_v, 1, Val_int(index));
  NSString *result = NSString_val(caml_callback2(*closure_f, Val_int(self.childOfItem), tuple_v));
  NSString *child = [self objectForId:result];
  CAMLreturnT(id, child);
}

- (BOOL)browser:(NSBrowser *)browser 
     isLeafItem:(id)item {
  NSString *item_s = (NSString *)item;
  
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_is_leaf_item");
  }

  return Val_true == caml_callback2(*closure_f, Val_int(self.isLeafItem), Val_NSString(item_s));
}

- (id)browser:(NSBrowser *)browser 
objectValueForItem:(id)item {
  NSString *item_s = (NSString *)item;
  
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_display_for_item");
  }

  return NSString_val(caml_callback2(*closure_f, Val_int(self.displayForItem), Val_NSString(item_s)));
}

@end

CAMLprim value fluid_column_browser_create(value dims, value trackers) {
  CAMLparam2(dims, trackers);
  CAMLlocal1(browser_v);

  Unpack_record4_double(dims, left, top, width, height);
  NSRect frame = NSMakeRect(left, top, width, height);
  NSBrowser *browser = [[NSBrowser alloc] initWithFrame:frame];
  browser.hasHorizontalScroller = YES;
  browser.columnResizingType = NSBrowserUserColumnResizing;
  browser.minColumnWidth = 300.0;

  BrowserDelegate *delegate = [[BrowserDelegate alloc] init];
  delegate.childrenCount = Int_val(Field(trackers, 0));
  delegate.childOfItem = Int_val(Field(trackers, 1));
  delegate.isLeafItem = Int_val(Field(trackers, 2));
  delegate.displayForItem = Int_val(Field(trackers, 3));

  browser.delegate = delegate;

  Wrap(browser_v, browser);
  CAMLreturn(browser_v);
}

void fluid_column_browser_update(value browser_v, value trackers) {
  CAMLparam2(browser_v, trackers);

  NSBrowser *browser = (NSBrowser *)Unwrap(browser_v);
  BrowserDelegate *delegate = (BrowserDelegate *)browser.delegate;
  
  delegate.childrenCount = Int_val(Field(trackers, 0));
  delegate.childOfItem = Int_val(Field(trackers, 1));
  delegate.isLeafItem = Int_val(Field(trackers, 2));
  delegate.displayForItem = Int_val(Field(trackers, 3));

  CAMLreturn0;
}

void fluid_column_browser_reload_column(value browser_v, value column_v) {
  CAMLparam2(browser_v, column_v);

  NSBrowser *browser = (NSBrowser *)Unwrap(browser_v);
  [browser reloadColumn:Int_val(column_v)];

  CAMLreturn0;
}

CAMLprim value fluid_column_browser_selected_cell(value browser_v) {
  CAMLparam1(browser_v);
  CAMLlocal1(result);

  printf("Getting selected cell\n");
  NSBrowser *browser = (NSBrowser *)Unwrap(browser_v);
  printf("growser\n");
  if (browser.selectedCell == nil) {
    printf("none\n");
    result = Atom(0);
  } else {
    printf("some\n");
    NSIndexPath* selected = browser.selectionIndexPath;
    NSString* item = (NSString *)[browser itemAtIndexPath:selected];
    result = caml_alloc_tuple(1);
    Store_field(result, 0, Val_NSString(item));
  }
    printf("ok\n");

  CAMLreturn(result);
}