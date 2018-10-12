
open Fluid;

let fadeOut = [%bs.raw {|function(node) {
  const box = node.getBoundingClientRect();
  node.style.position = 'absolute';
  node.style.pointerEvents = 'none';
  node.style.top = box.top + 'px';
  node.style.left = box.left + 'px';
  const max = 30;
  let timer = max;
  const loop = () => {
    timer -= 1;
    node.style.opacity = timer / max
    if (timer > 0) {
      requestAnimationFrame(loop)
    } else {
      node.parentNode.removeChild(node);
    }
  };
  requestAnimationFrame(loop)
}|}];

let fadeIn = [%bs.raw {|function(node) {
  const max = 30;
  let timer = max;
  node.style.opacity = 0;
  const loop = () => {
    timer -= 1;
    node.style.opacity = 1 - timer / max
    if (timer > 0) {
      requestAnimationFrame(loop)
    }
  };
  requestAnimationFrame(loop)
}|}];

module Toggle = {
  type props = {on: (unit => unit) => element, off: (unit => unit) => element};
  let props = (~on, ~off, ()) => {on, off};
  let maker = {
    name: "Toggle",
    initialState:(_) => false,
    reconcileTrees:Some((oldState, newState, mountedTree, newTree) => {
      switch (oldState, newState) {
        | (false, true)
        | (true, false) =>
          let domNode = getDomNode(mountedTree);
          let newTree = inflateTree(instantiateTree(newTree));
          let newDomNode = getDomNode(newTree);
          fadeOut(domNode);
          fadeIn(newDomNode);
          domNode->parentNode->insertBefore(newDomNode, ~reference=domNode);
          newTree
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
  let props = (~text, ~style, ~onClick, ()) => (text, style, onClick);
  let make = Maker.component(
    ~name="Button",
    ~render=((text, style, onClick)) => {
      <button style onclick={evt => onClick()}>(String(text))</button>
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
    on=(onClick => <div>
      (String("Click this to"))
      <Button style="background-color: green" onClick text="Turn Off" />
    </div>)
    off=(onClick => <div>
      <Button style="background-color: pink" onClick text="Turn On" />
      (String("if you want"))
    </div>)
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
