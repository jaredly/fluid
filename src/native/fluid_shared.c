
#include "./fluid_shared.h"


@implementation ClickTarget {
  int onClick;
}

- (instancetype)initWithOnClick:(int)onClickv {
  if (self = [super init]) {
    onClick = onClickv;
  }
  return self;
}

- (void)onClick {
  callUnit(onClick);
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
    closure_f = caml_named_value("fluid_button_press");
  }
  logf("> Button press %d\n", id_v);
  caml_callback(*closure_f, Val_int(id_v));
}

@end


void callUnit(int fnId) {
  CAMLparam0();
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_unit_fn");
  }

  logf("Call unit %d\n", fnId);
  caml_callback2(*closure_f, Val_int(fnId), Val_unit);
  CAMLreturn0;
}

void callPos(int fnId, float x, float y) {
  CAMLparam0();
  CAMLlocal1(pos_v);
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_pos_fn");
  }

  logf("Call pos %d %f,%f\n", fnId, x, y);
  Create_record2_double(pos_v, x, y);
  caml_callback2(*closure_f, Val_int(fnId), pos_v);
  CAMLreturn0;
}

void callString(int fnId, const char* text) {
  CAMLparam0();
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_string_fn");
  }

  logf("Call string %d %s\n", fnId, text);
  caml_callback2(*closure_f, Val_int(fnId), caml_copy_string(text));
  CAMLreturn0;
}

void callRect(int fnId, float x, float y, float w, float h) {
  CAMLparam0();
  CAMLlocal1(rect_v);
  static value * closure_f = NULL;
  if (closure_f == NULL) {
    closure_f = caml_named_value("fluid_rect_fn");
  }

  logf("Call rect %d\n", fnId);
  Create_record4_double(rect_v, x, y, w, h);
  caml_callback2(*closure_f, Val_int(fnId), rect_v);
  CAMLreturn0;
}