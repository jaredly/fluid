#include "./fluid_shared.h"


@interface BrowserDelegate: NSObject <NSBrowserDelegate>
  @property (nonatomic) int childrenCount;
  @property (nonatomic) int childOfItem;
  @property (nonatomic) int isLeafItem;
  @property (nonatomic) int displayForItem;
@end

@implementation BrowserDelegate {
  NSMutableDictionary<NSNumber *, NSNumber *> *objects;
}

- (instancetype)init {
  if (self = [super init]) {
    objects = [[NSMutableDictionary alloc] init];
    return self;
  }
  return nil;
}

- (id)objectForId:(int)id {
  NSNumber *key = [NSNumber numberWithInt:id];
  NSNumber *obj = [objects objectForKey:key];
  if (obj == nil) {
    obj = key;
    [objects setObject:key forKey:key];
  }
  return obj;
}

- (id)rootItemForBrowser:(NSBrowser *)browser {
  return [self objectForId:0];
}

- (NSInteger)browser:(NSBrowser *)browser numberOfChildrenOfItem:(id)item {
  NSNumber *item_n = (NSNumber *)item;
  int item_i = [item_n integerValue];

  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_children_count");
  }

  return Int_val(caml_callback2(*closure_f, Val_int(self.childrenCount), Val_int(item_i)));
}

- (id)browser:(NSBrowser *)browser child:(NSInteger)index ofItem:(id)item {
  CAMLparam0();
  CAMLlocal1(tuple_v);

  NSNumber *item_n = (NSNumber *)item;
  int item_i = [item_n integerValue];

  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_child_of_item");
  }
  tuple_v = caml_alloc_tuple(2);
  Store_field(tuple_v, 0, Val_int(item_i));
  Store_field(tuple_v, 1, Val_int(index));
  int result = Int_val(caml_callback2(*closure_f, Val_int(self.childOfItem), tuple_v));
  NSNumber *child = [self objectForId:result];
  CAMLreturnT(id, child);
}

- (BOOL)browser:(NSBrowser *)browser 
     isLeafItem:(id)item {
  NSNumber *item_n = (NSNumber *)item;
  int item_i = [item_n integerValue];
  
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_is_leaf_item");
  }

  return Val_true == caml_callback2(*closure_f, Val_int(self.isLeafItem), Val_int(item_i));
}

- (id)browser:(NSBrowser *)browser 
objectValueForItem:(id)item {
  NSNumber *item_n = (NSNumber *)item;
  int item_i = [item_n integerValue];
  
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_column_browser_display_for_item");
  }

  return NSString_val(caml_callback2(*closure_f, Val_int(self.displayForItem), Val_int(item_i)));
}

@end


CAMLprim value fluid_column_browser_create(value dims, value trackers) {
  CAMLparam2(dims, trackers);
  CAMLlocal1(browser_v);

  Unpack_record4_double(dims, left, top, width, height);
  NSRect frame = NSMakeRect(left, top, width, height);
  NSBrowser *browser = [[NSBrowser alloc] initWithFrame:frame];

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