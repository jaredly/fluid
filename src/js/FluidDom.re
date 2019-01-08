module NativeInterface = {
  type nativeProps;

  type nativeNode;
  type font = {fontName: string, fontSize: float};

  [@bs.obj] external nativeProps: (
    ~id:string=?,
    ~_type:string=?,
    ~onchange:Dom.event => unit=?,
    ~oninput:Dom.event => unit=?,
    ~width:int=?,
    ~height:int=?,
    ~onclick:Dom.event => unit=?,
    ~style:string=?,
    unit
    ) => nativeProps = "";

  /* external opaqueProps: domProps('a) => nativeProps = "%identity"; */
  [@bs.scope "document"][@bs.val] external _createElement: string => nativeNode = "createElement";
  let setDomProps: (nativeNode, nativeProps) => unit = [%bs.raw (node, props) => {|
    Object.keys(props).forEach(key => {
      if (key === 'checked' || key === 'value') {
        node[key] = props[key]
      } else if (typeof props[key] === 'function') {
        node[key] = props[key]
      } else {
        node.setAttribute(key, props[key])
      }
    })
  |}];

  let string_of_float = f => {
    if (float_of_int(int_of_float(f)) == f) {
      string_of_float(f) ++ "0"
    } else {
      string_of_float(f)
    }
  };

  let createElement = (typ, nativeProps, layout: Layout.node) => {
    let node = _createElement(typ);
    let snode = Obj.magic(node);

    setDomProps(node, nativeProps);

    snode##style##position #= "absolute";
    snode##style##left #= (string_of_float(layout.layout.left) ++ "px");
    snode##style##top #= (string_of_float(layout.layout.top) ++ "px");
    snode##style##bottom #= (string_of_float(layout.layout.bottom) ++ "px");
    snode##style##right #= (string_of_float(layout.layout.right) ++ "px");
    snode##style##width #= (string_of_float(layout.layout.width) ++ "px");
    snode##style##height #= (string_of_float(layout.layout.height) ++ "px");
    /* Js.log2("layout", (layout.layout.width, layout.layout.height, layout.layout.top, layout.layout.left)); */

    /* Js.log3("layout", [|
      layout.layout.left,
      layout.layout.top,
      layout.layout.right,
      layout.layout.bottom,
      layout.layout.width,
      layout.layout.height,
    |], node); */

    node;
  };

  let defaultFont = {fontName: "system-ui", fontSize: 16.};

  let measureWithCanvas: (. string, font) => Layout.LayoutTypes.dimensions = [%bs.raw {|
  function() {
    var canvas = document.createElement('canvas');
    var context = canvas.getContext('2d');
    var cache = {};
    document.body.appendChild(canvas);
    return function (text, font) {
      const key = `${text}:${font[0]}:${font[1]}`;
      if (cache[key]) {
        return cache[key]
      }
      context.font = font[1] + 'px ' + font[0]
      const dims = context.measureText(text);
      cache[key] = [dims.width, font[1] * 1.2]
      return cache[key]
    }
  }()
  |}];

  let measureText = (text, font, _node, width, widthMode, height, heightMode) => {
    let font = switch font { | None => defaultFont | Some(f) => f };
    measureWithCanvas(. text, font)
        /* | Undefined /* 'undefined' */
    | Exactly /* 'exactly' */
    | AtMost 'at-most' */
    /* {
      Layout.LayoutTypes.width: float_of_int(String.length(text)) *. 15.,
      height: 16.,
    } */
  };

  [@bs.scope "document"][@bs.val] external createTextNode: string => nativeNode = "";
  [@bs.set] external setTextContent: (nativeNode, string) => unit = "textContent";
  let setTextContent = (el, text, font) => {
    let font = switch font { | None => defaultFont | Some(f) => f };
    setTextContent(el, text);
    let style = Obj.magic(el)##style;
    style##fontFamily #= font.fontName;
    style##fontSize #= (string_of_float(font.fontSize) ++ "px");
  };
  let createTextNode = (text, layout, font) => {
    let el = createElement("span", Js.Obj.empty() |> Obj.magic, layout);
    setTextContent(el, text, font);
    el
  };
  [@bs.get] external parentNode: nativeNode => nativeNode = "";
  [@bs.send] external appendChild: (nativeNode, nativeNode) => unit = "";
  [@bs.send] external insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit = "";
  [@bs.send] external removeChild: (nativeNode, nativeNode) => unit = "";
  let updateNativeProps = (node, _oldProps, newProps) => {
    setDomProps(node, newProps);
  };
  [@bs.send] external replaceWith: (nativeNode, nativeNode) => unit = "";


  type element = {tag: string, props: nativeProps};
  let maybeUpdate = (~mounted, ~mountPoint, ~newElement) => {
    if (mounted.tag == newElement.tag) {
      updateNativeProps(mountPoint, mounted.props, newElement.props);
      true
    } else {
      false
    }
  };
  let inflate = ({tag, props}, layout) => createElement(tag, props, layout);
};

module Fluid = {
  module NativeInterface = NativeInterface;

  include FluidMaker.F(NativeInterface);

  module Native = {
    let div = (~id=?, ~children=[], ~layout=?, ~_type=?, ~width=?, ~height=?, ~onclick=?, ~style=?, ()) => 
    Builtin({
      NativeInterface.tag: "div",
      props:
        NativeInterface.nativeProps(
          ~id?,
          ~_type?,
          ~width?,
          ~height?,
          ~onclick?,
          ~style?,
          (),
        ),
    }, children, layout, None);

    let button = (~id=?, ~children, ~layout=?, ~_type=?, ~width=?, ~height=?, ~onclick=?, ~style=?, ()) => 
    Builtin({
      NativeInterface.tag: "button",
      props:
        NativeInterface.nativeProps(
          ~id?,
          ~_type?,
          ~width?,
          ~height?,
          ~onclick?,
          ~style?,
          (),
        ),
    }, children, layout, None);

    let input = (~id=?, ~_type=?, ~width=?, ~height=?, ~onchange=?, ~oninput=?, ~style=?, ()) => 
    Builtin({
      NativeInterface.tag: "input",
      props:
        NativeInterface.nativeProps(
          ~id?,
          ~_type?,
          ~width?,
          ~height?,
          ~onchange?,
          ~oninput?,
          ~style?,
          (),
        ),
    }, [], None, None);
  }
}