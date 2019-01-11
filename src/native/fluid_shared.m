
#include "./fluid_shared.h"

// @interface FlippedView : NSView
// { }
// - (BOOL)isFlipped;
// @end

@implementation FlippedView : NSView
- (BOOL)isFlipped {
  return YES;
}
@end

// @interface MLButtonDelegate : NSObject
// @end

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
  caml_callback(onPress_v, Val_unit);
}

@end
