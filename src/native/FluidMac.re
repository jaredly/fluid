
module M = Flex.Layout;

module NativeInterface = {
  type nativeNode;


  external createTextNode: string => nativeNode = "fluid_create_NSTextView";
  external setTextContent: (nativeNode, string) => unit = "fluid_set_NSTextView_textContent";
  /* [@bs.get] external parentNode: nativeNode => nativeNode = "fluid_"; */
  external appendChild: (nativeNode, nativeNode) => unit = "fluid_NSView_appendChild";
  /* external insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit = "fluid_NSView_insertBefore"; */
  external removeChild: (nativeNode, nativeNode) => unit = "fluid_NSView_removeChild";

  external replaceWith: (nativeNode, nativeNode) => unit = "fluid_NSView_replaceWith";

  external createView: (~onPress: unit => unit=?, unit) => nativeNode = "fluid_create_NSView";
  external createButton: (~title: string, ~onPress: unit => unit, unit) => nativeNode = "fluid_create_NSButton";

  external startApp: (~title: string, nativeNode => unit) => unit = "fluid_startApp";

  type element =
    | View(option(unit => unit))
    | Button(string, unit => unit);

  let maybeUpdate = (~mounted, ~mountPoint, ~newElement) => {
    false
  };
  let inflate = element => switch element {
    | View(onPress) => createView(~onPress?, ())
    | Button(title, onPress) => createButton(~title, ~onPress, ())
  }
};

module Fluid = {
  module NativeInterface = NativeInterface;

  include FluidMaker.F(NativeInterface);

  module Native = {
    let view = (~onPress=?, ()): NativeInterface.element => View(onPress);

    let button = (~onPress, ~title, ()): NativeInterface.element => Button(title, onPress);

  }
}
