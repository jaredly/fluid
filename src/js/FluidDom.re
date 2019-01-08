module NativeInterface = {
  type nativeProps;

  type nativeNode;

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

    snode##style##position #= "absolute";
    snode##style##left #= (string_of_float(layout.layout.left) ++ "px");
    snode##style##top #= (string_of_float(layout.layout.top) ++ "px");
    snode##style##bottom #= (string_of_float(layout.layout.bottom) ++ "px");
    snode##style##right #= (string_of_float(layout.layout.right) ++ "px");
    snode##style##width #= (string_of_float(layout.layout.width) ++ "px");
    snode##style##height #= (string_of_float(layout.layout.height) ++ "px");
    Js.log2("layout", layout);

    /* Js.log3("layout", [|
      layout.layout.left,
      layout.layout.top,
      layout.layout.right,
      layout.layout.bottom,
      layout.layout.width,
      layout.layout.height,
    |], node); */

    setDomProps(node, nativeProps);
    node;
  };

  let measureText = (text, _node, width, widthMode, height, heightMode) => {
        /* | Undefined /* 'undefined' */
    | Exactly /* 'exactly' */
    | AtMost 'at-most' */
    {
      Layout.LayoutTypes.width: float_of_int(String.length(text)) *. 20.,
      height: 16.,
    }
  };

  [@bs.scope "document"][@bs.val] external createTextNode: string => nativeNode = "";
  [@bs.set] external setTextContent: (nativeNode, string) => unit = "textContent";
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
    let div = (~id=?, ~_type=?, ~width=?, ~height=?, ~onclick=?, ~style=?, ()): NativeInterface.element => {
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
    };

    let button = (~id=?, ~_type=?, ~width=?, ~height=?, ~onclick=?, ~style=?, ()): NativeInterface.element => {
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
    };

    let input = (~id=?, ~_type=?, ~width=?, ~height=?, ~onchange=?, ~oninput=?, ~style=?, ()): NativeInterface.element => {
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
    };
  }
}