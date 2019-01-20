
#include "./fluid_shared.h"

// @interface FlippedView : NSView
// { }
// - (BOOL)isFlipped;
// @end


@implementation ClickTarget {
  value onClick;
}

- (instancetype)initWithOnClick:(value)onClickv {
  if (self = [super init]) {
    onClick = onClickv;
    caml_register_global_root(&onClickv);
  }
  return self;
}

- (void)onClick {
  caml_callback(onClick, Val_unit);
}
@end



@implementation FlippedView : NSView
- (BOOL)isFlipped {
  return YES;
}
@end

@implementation FluidButton
{
  int id_v;
}

+ (FluidButton*)createWithTitle:(NSString*)title id:(int)id
{
  FluidButton* button = [super buttonWithTitle:title target:NULL action:@selector(onPress)];
  button.target = button;
  [button setId:id];
  return button;
}

- (void)setId:(int)idnum
{
  id_v = idnum;
}

- (void)onPress {
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    /* First time around, look up by name */
    closure_f = caml_named_value("fluid_button_press");
  }
  logf("> Button press %d\n", id_v);
  caml_callback(*closure_f, Val_int(id_v));
}

@end
