
open Fluid;

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


let first = <div id="awesome">
  {String("Hello")}
  <div id="here">
    <div>{String("What")}</div>
  </div>
  <Awesome value=5 toString=string_of_int />
  <Awesome value="Hi"  toString=(x => x) />
  <div
    id="Inner"
  >
    {String("world")}
  </div>
</div>;

[@bs.val][@bs.scope "document"] external getElementById: string => option(domNode) = "";

switch (getElementById("root")) {
  | None => assert(false)
  | Some(node) => mount(first, node)
}
