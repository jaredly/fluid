
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

@implementation FluidButton
{
  value onPress_v;
}

+ (FluidButton*)createWithTitle:(NSString*)title onPress:(value)onPressv
{
  FluidButton* button = [super buttonWithTitle:title target:NULL action:@selector(onPress)];
  button.target = button;
  [button setOnPress:onPressv];
  return button;
}

- (void)setOnPress:(value)onPressv
{
  onPress_v = onPressv;
}

- (void)onPress {
  log("> Button onPress\n");
  caml_callback(onPress_v, Val_unit);
  log("< Button pressed\n");
}

@end
