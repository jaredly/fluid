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
    let createElement = (typ, nativeProps) => {
      let node = _createElement(typ);
      setDomProps(node, nativeProps);
      node;
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
    let inflate = ({tag, props}) => createElement(tag, props);
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