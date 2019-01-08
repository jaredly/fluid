
module M = Flex.Layout;

module NativeInterface = {
  type nativeNode;


  external createTextNode: (string, ~pos: (float, float), ~size: (float, float)) => nativeNode = "fluid_create_NSTextView";
  external setTextContent: (nativeNode, string) => unit = "fluid_set_NSTextView_textContent";
  /* [@bs.get] external parentNode: nativeNode => nativeNode = "fluid_"; */
  external appendChild: (nativeNode, nativeNode) => unit = "fluid_NSView_appendChild";
  /* external insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit = "fluid_NSView_insertBefore"; */
  external removeChild: (nativeNode, nativeNode) => unit = "fluid_NSView_removeChild";

  external replaceWith: (nativeNode, nativeNode) => unit = "fluid_NSView_replaceWith";

  external createView: (
    ~onPress: option(unit => unit),
    ~pos: (float, float),
    ~size: (float, float)
  ) => nativeNode = "fluid_create_NSView";
  external createButton: (
    ~title: string,
    ~onPress: unit => unit,
    ~pos: (float, float),
    ~size: (float, float)
  ) => nativeNode = "fluid_create_NSButton";

  external startApp: (~title: string, nativeNode => unit) => unit = "fluid_startApp";

  external measureText: (~text: string, ~font: string, ~fontSize: float) => (float, float) = "fluid_measureText";

  let measureText = (text, _, _, _, _, _) => {
    let (width, height) = measureText(~text, ~font="Helvetica", ~fontSize=16.);
    {Layout.LayoutTypes.width, height}
  };

  type element =
    | View(option(unit => unit))
    | Button(string, unit => unit);

  let maybeUpdate = (~mounted, ~mountPoint, ~newElement) => {
    false
  };

  let createTextNode = (text, {Layout.LayoutTypes.layout: {top, left, width, height}}) =>
    createTextNode(text, ~pos=(top, left), ~size=(width, height));

  let inflate = (element, {Layout.LayoutTypes.layout: {width, height, top, left}}) => switch element {
    | View(onPress) => 
    Printf.printf("OCaml side %f,%f %f x %f\n", top, left, width, height);
    createView(~onPress, ~pos=(top, left), ~size=(width, height))
    | Button(title, onPress) =>
    createButton(~title, ~onPress, ~pos=(top, left), ~size=(width, height))
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
