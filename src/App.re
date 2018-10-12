
open Fluid;

/*
Thoughts about props...

seems like with bs.deriving abstract you could transform

WARNING: I wouldn't be able to use the stock one, because it only adds the final
unit argument if there's an optional argument, and I need to not care about whether
there is one or not.

<Awesome a=2 b=3 />

/* Assuming there's a default on `children=[]` */
Awesome.make(Awesome.props(~a=2, ~b=3, ()));

<Awesome a=2 b=3>
  <div />
  <input />
  <Bomb />
</Awesome>

Awesome.make(Awesome.props(~a=2, ~b=3, ~children=[<div />, <input />, <Bomb />], ()))




Ok, now what do I need to prototype?


 */

let awesome = props =>
  Maker.statefulComponent(
    ~initialState= _props => "Folks",
    ~render= ((v, toString), state, setState) =>
      <div>
        <div onclick={_evt => setState(state ++ "1")}>
          {String(toString(v))}
        </div>
      </div>,
      props
    );


/** Yayy polymorphism and a normal props thing! */
module Awesome = {
  let props = (~value, ~toString, ()) => (value, toString);
  let maker = {
    initialState: _props => "Folks",
    render: ((v, toString), state, setState) =>
      <div>
        <div onclick={_evt => setState(state ++ "1")}>
          {String("Awesome " ++ toString(v))}
        </div>
      </div>,
    newStateForProps: None
  };
  let make = props => Maker.makeComponent(maker, props);
};



let awesome =
  Maker.statefulComponent(
    ~initialState= _props => "Folks",
    ~render= (props, state, setState) => {
      let children = [
          <div onclick={_evt => setState(state ++ "1")}>
            {String(state)}
          </div>,
          ...props,
        ];

      <div> ...children </div>
    }
  );

let recursive =
  Maker.recursiveComponent(
    Maker.statefulComponent(
      ~initialState=((_loop, depth)) => ("Recursion!", depth),
      ~render=
        ((loop, parentNum), (text, depth), setState) => {
          let rec recur = num =>
            num > 0 ? [Custom(loop(depth - 1)), ...recur(num - 1)] : [];
          <div>
            {
              String(
                "[depth "
                ++ string_of_int(depth)
                ++ ":"
                ++ string_of_int(parentNum)
                ++ "]",
              )
            }
            <div
                onclick={_evt => setState((text ++ "1", depth - 1))}
                style="cursor: pointer"
              >
              {String(text)}
            </div>
            {
              let children = recur(depth);
              <div style="padding-left: 20px; border-left: 2px solid #aaa">
                ...children
              </div>
            }
          </div>;
        },
    ),
  );

let first = <div id="awesome">
  {String("Hello")}
  <div id="here">
    <div>{String("What")}</div>
  </div>
  <Awesome value=5 toString=string_of_int />
  <Awesome value="Hi"  toString=(x => x) />
  {Custom(awesome([
    String(">>"),
    Custom(awesome([]))
  ]))}
  <div
    id="Inner"
  >
    {String("world")}
    {Custom(recursive(3))}
  </div>
</div>;

[@bs.val][@bs.scope "document"] external getElementById: string => option(domNode) = "";

switch (getElementById("root")) {
  | None => assert(false)
  | Some(node) => mount(first, node)
}
