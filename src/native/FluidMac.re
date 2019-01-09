
module M = Flex.Layout;

type color = {r: float, g: float, b: float, a: float};

module NativeInterface = {
  type nativeNode;

  type font = {fontName: string, fontSize: float};

  external createTextNode: (string, ~pos: (float, float), ~size: (float, float), ~font: font) => nativeNode = "fluid_create_NSTextView";
  external setTextContent: (nativeNode, string, font) => unit = "fluid_set_NSTextView_textContent";
  /* [@bs.get] external parentNode: nativeNode => nativeNode = "fluid_"; */
  external appendChild: (nativeNode, nativeNode) => unit = "fluid_NSView_appendChild";
  /* external insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit = "fluid_NSView_insertBefore"; */
  external removeChild: (nativeNode, nativeNode) => unit = "fluid_NSView_removeChild";

  external replaceWith: (nativeNode, nativeNode) => unit = "fluid_NSView_replaceWith";

  type viewStyles = {backgroundColor: option(color)};

  external createNullNode: unit => nativeNode = "fluid_create_NullNode";

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

  external measureText: (~text: string, ~font: string, ~fontSize: float, ~maxWidth: option(float)) => (float, float) = "fluid_measureText";

  let defaultFont = {fontName: "Lucida Grande", fontSize: 12.};
  let setTextContent = (node, text, font) => {
    let font = switch font { | None => defaultFont | Some(f) => f};
    setTextContent(node, text, font);
  };
  let measureText = (text, font, _, width, widthMode, _, _) => {
    let {fontName, fontSize} = switch font { | None => defaultFont | Some(f) => f};
    let maxWidth = switch widthMode {
      | Layout.LayoutTypes.Exactly | AtMost => Some(width)
      | _ => None
    };

    let (width, height) = measureText(~text, ~font=fontName, ~fontSize, ~maxWidth);
    Printf.printf("Text: %s -- %f x %f", text, width, height);
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

  let createTextNode = (text, {Layout.LayoutTypes.layout: {top, left, width, height}}, font) => {
    let font = switch font { | None => defaultFont | Some(f) => f};
    createTextNode(text, ~pos=(top, left), ~size=(width, height), ~font);
  }

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
    open NativeInterface;
    let view = (~onPress=?, ~children=[], ~layout=?, ~backgroundColor=?, ()) => 
    `Builtin(
      View(onPress, {backgroundColor: backgroundColor}),
      children,
      layout,
      None
    )
      ;

    let button = (~onPress, ~title, ~layout=?, ()) => 
    `Builtin(
      Button(title, onPress),
      [],
      layout,
      Some((a, b, c, d, e) => {
        let {Layout.LayoutTypes.width, height}: Layout.LayoutTypes.dimensions = NativeInterface.measureText(title, None, a, b, c, d, e);
        {Layout.LayoutTypes.width: width +. 20., height: height +. 10.}
      })
    );

  }

  let launchWindow = (~title: string, ~root: element) => {
    let instances = instantiateTree(Native.view(~children=[root], ()));
    let instanceLayout = getInstanceLayout(instances);
    Layout.layout(instanceLayout);
    let tree = inflateTree(instances);
    let {Layout.LayoutTypes.width, height} = instanceLayout.layout;
    NativeInterface.startApp(~title, ~size=(width, height), node => {
      node->NativeInterface.appendChild(getNativeNode(tree))
    });
  };
}
