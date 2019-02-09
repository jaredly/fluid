open Flex;

module Node = {
  type context = ref(int);
  /* Ignored - only needed to create the dummy instance. */
  let nullContext = {contents: 0};
};

let rootContext = {contents: 0};

module Encoding = FloatEncoding;
module Layout = Layout.Create(Node, Encoding);
module LayoutSupport = Layout.LayoutSupport;
module LayoutTypes = LayoutSupport.LayoutTypes;


type style = LayoutTypes.cssStyle;
type node = LayoutTypes.node;
let null = LayoutSupport.theNullNode;
let defaultStyle = LayoutSupport.defaultStyle;

let createNode = (children, style) =>
  LayoutSupport.createNode(
    ~withChildren=children,
    ~andStyle=style,
    rootContext,
  );
let createNodeWithMeasure = (children, style, measure) =>
  LayoutSupport.createNode(
    ~withChildren=children,
    ~andStyle=style,
    ~andMeasure=measure,
    rootContext,
  );
let layout = (layoutNode) => {
  Layout.layoutNode(
    layoutNode,
    Encoding.cssUndefined,
    Encoding.cssUndefined,
    Ltr,
  );
};
/* let printCssNode = root =>
  LayoutPrint.printCssNode((
    root,
    {printLayout: true, printChildren: true, printStyle: true},
  )); */

let style =
    (
      ~width=Encoding.cssUndefined,
      ~height=Encoding.cssUndefined,
      ~maxWidth=Encoding.cssUndefined,
      ~maxHeight=Encoding.cssUndefined,
      ~minWidth=Encoding.cssUndefined,
      ~minHeight=Encoding.cssUndefined,
      ~flexBasis=Encoding.cssUndefined,
      ~flexDirection=LayoutTypes.Column,
      ~flexGrow=Encoding.cssUndefined,
      ~flexShrink=Encoding.cssUndefined,
      ~alignItems=LayoutTypes.AlignFlexStart,
      ~alignSelf=LayoutTypes.AlignAuto,
      ~justifyContent=LayoutTypes.JustifyFlexStart,
      ~position=LayoutTypes.Relative,
      ~top=Encoding.cssUndefined,
      ~bottom=Encoding.cssUndefined,
      ~left=Encoding.cssUndefined,
      ~right=Encoding.cssUndefined,
      ~overflow=LayoutTypes.Visible,
      ~marginTop=Encoding.cssUndefined,
      ~marginLeft=Encoding.cssUndefined,
      ~marginRight=Encoding.cssUndefined,
      ~marginBottom=Encoding.cssUndefined,
      ~margin=Encoding.cssUndefined,
      ~flexWrap=LayoutTypes.CssNoWrap,
      ~marginVertical=Encoding.cssUndefined,
      ~marginHorizontal=Encoding.cssUndefined,
      ~padding=Encoding.cssUndefined,
      ~paddingVertical=Encoding.cssUndefined,
      ~paddingHorizontal=Encoding.cssUndefined,
      ~paddingTop=Encoding.cssUndefined,
      ~paddingLeft=Encoding.cssUndefined,
      ~paddingRight=Encoding.cssUndefined,
      ~paddingBottom=Encoding.cssUndefined,
      ()
    ) => { 
  let ret: LayoutTypes.cssStyle = {
    ...LayoutSupport.defaultStyle,
    positionType: position,
    overflow,
    top,
    left,
    bottom,
    flexBasis,
    flexDirection,
    flexGrow,
    flexShrink,
    flexWrap,
    alignItems,
    alignSelf,
    justifyContent,
    right,
    width,
    height,
    maxWidth,
    minHeight,
    minWidth,
    maxHeight,
    marginTop,
    marginLeft,
    marginRight,
    marginBottom,
    margin,
    marginVertical,
    marginHorizontal,
    padding,
    paddingVertical,
    paddingHorizontal,
    paddingTop,
    paddingLeft,
    paddingRight,
    paddingBottom,
  };

  ret;
};

open LayoutTypes;
type measureType = (node, unitOfM, measureMode, unitOfM, measureMode) => dimensions;