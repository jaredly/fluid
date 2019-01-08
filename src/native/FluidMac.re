
module M = Flex.Layout;

type color = {r: float, g: float, b: float, a: float};

module NativeInterface = {
  type nativeNode;


  external createTextNode: (string, ~pos: (float, float), ~size: (float, float)) => nativeNode = "fluid_create_NSTextView";
  external setTextContent: (nativeNode, string) => unit = "fluid_set_NSTextView_textContent";
  /* [@bs.get] external parentNode: nativeNode => nativeNode = "fluid_"; */
  external appendChild: (nativeNode, nativeNode) => unit = "fluid_NSView_appendChild";
  /* external insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit = "fluid_NSView_insertBefore"; */
  external removeChild: (nativeNode, nativeNode) => unit = "fluid_NSView_removeChild";

  external replaceWith: (nativeNode, nativeNode) => unit = "fluid_NSView_replaceWith";

  type viewStyles = {backgroundColor: option(color)};

  external createView: (
    ~onPress: option(unit => unit),
    ~pos: (float, float),
    ~size: (float, float),
    ~style: viewStyles,
  ) => nativeNode = "fluid_create_NSView";
  external createButton: (
    ~title: string,
    ~onPress: unit => unit,
    ~pos: (float, float),
    ~size: (float, float)
  ) => nativeNode = "fluid_create_NSButton";

  external updateButton: (nativeNode, string, (unit => unit)) => unit = "fluid_update_NSButton";
  external updateView: (nativeNode, option(unit => unit), viewStyles) => unit = "fluid_update_NSView";

  external startApp: (~title: string, ~size: (float, float), nativeNode => unit) => unit = "fluid_startApp";

  external measureText: (~text: string, ~font: string, ~fontSize: float) => (float, float) = "fluid_measureText";

  let measureText = (text, _, _, _, _, _) => {
    let (width, height) = measureText(~text, ~font="Helvetica", ~fontSize=16.);
    {Layout.LayoutTypes.width, height}
  };

  type element =
    | View(option(unit => unit), viewStyles)
    | Button(string, unit => unit);

  let maybeUpdate = (~mounted, ~mountPoint, ~newElement) => {
    switch (mounted, newElement) {
      | (View(aPress, aStyle), View(onPress, style)) => 
        if (aPress != onPress || aStyle != style) {
          updateView(mountPoint, onPress, style)
        };
        true

      | (Button(atitle, apress), Button(btitle, bpress)) =>
        if (atitle != btitle || apress !== bpress) {
          updateButton(mountPoint, btitle, bpress)
        };
        true

      | _ => false
    }
  };

  let createTextNode = (text, {Layout.LayoutTypes.layout: {top, left, width, height}}) =>
    createTextNode(text, ~pos=(top, left), ~size=(width, height));

  let inflate = (element, {Layout.LayoutTypes.layout: {width, height, top, left}}) => switch element {
    | View(onPress, style) => 
    Printf.printf("OCaml side %f,%f %f x %f\n", top, left, width, height);
    createView(~onPress, ~pos=(top, left), ~size=(width, height), ~style)
    | Button(title, onPress) =>
    createButton(~title, ~onPress, ~pos=(top, left), ~size=(width, height))
  }
};

module Fluid = {
  module NativeInterface = NativeInterface;

  include FluidMaker.F(NativeInterface);

  module Native = {
    let view = (~onPress=?, ~backgroundColor=?, ()): NativeInterface.element => View(onPress, {backgroundColor: backgroundColor});

    let button = (~onPress, ~title, ()): NativeInterface.element => Button(title, onPress);

  }

  let launchWindow = (~title: string, ~root: element) => {
    let instances = instantiateTree(root);
    let instanceLayout = getInstanceLayout(instances);
    Layout.layout(instanceLayout);
    let tree = inflateTree(instances);
    let {Layout.LayoutTypes.width, height} = instanceLayout.layout;
    NativeInterface.startApp(~title, ~size=(width, height), node => {
      node->NativeInterface.appendChild(getNativeNode(tree))
    });
  };
}
