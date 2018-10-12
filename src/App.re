
open Fluid;

module Toggle = {
  type props = {on: (unit => unit) => element, off: (unit => unit) => element};
  let props = (~on, ~off, ()) => {on, off};
  let maker = {
    name: "Toggle",
    initialState:(_) => false,
    reconcileTrees:Some((oldState, newState, mountedTree, newTree) => {
      switch (oldState, newState) {
        | (false, true) =>
          Fluid.reconcileTrees(mountedTree, newTree)
        | (true, false) =>
          Fluid.reconcileTrees(mountedTree, newTree)
        | _ => mountedTree
      }
      /* mountedTree */
    }),
    /* reconcileTrees:None, */
    newStateForProps: None,
    render:({on, off}, state, setState) => {
      if (state) {
        on(() => setState(false))
      } else {
        off(() => setState(true))
      }
    }
  };
  let make = props => Maker.makeComponent(maker, props);
};

/** Yayy polymorphism and a normal props thing! */
module Awesome = {
  let props = (~value, ~toString, ()) => (value, toString);
  let maker = {
    name: "Awesome",
    initialState: _props => "Folks",
    reconcileTrees: None,
    render: ((v, toString), state, setState) =>
      <div>
        <div onclick={_evt => setState(state ++ "1")}>
          {String("Awesome " ++ toString(v) ++ " " ++ state)}
        </div>
      </div>,
    newStateForProps: None
  };
  let make = props => Maker.makeComponent(maker, props);
};

module Button = {
  let props = (~text, ~onClick, ()) => (text, onClick);
  let make = Maker.component(
    ~name="Button",
    ~render=((text, onClick)) => {
      <button onclick={evt => onClick()}>(String(text))</button>
    },
    ()
  );
};

let first = <div id="awesome">
  {String("Hello")}
  <div id="here">
    <div>{String("What")}</div>
  </div>
  <Toggle
    on=(onClick => <Button onClick text="Turn Off" />)
    off=(onClick => <Button onClick text="Turn On" />)
  />
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
