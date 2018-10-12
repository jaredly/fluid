
open Fluid;

module Style = {
  type style;
  [@bs.get] external style: domNode => style = "";
  [@bs.set] external transform: (style, string) => unit = "";
};

let zoom = node => {
  Animate.spring(
    ~dampingRatio=1.,
    ~frequencyResponseMs=1000.,
    (amount) => {
      node->Style.style->Style.transform("translateX(" ++ string_of_float(amount *. 100.) ++ "px)");
    },
    () => {
      node->parentNode->removeChild(node)
    }
  )
};

let fadeOut: domNode => unit = [%bs.raw {|function(node) {
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

let fadeIn: domNode => unit = [%bs.raw {|function(node) {
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
          zoom(domNode);
          /* fadeOut(domNode); */
          fadeIn(newDomNode);
          domNode->parentNode->insertBefore(newDomNode, ~reference=domNode);
          newTree
        | _ => mountedTree
      }
    }),
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
      <button style onclick={_evt => onClick()}>(String(text))</button>
    },
    ()
  );
};

[@bs.get] external target: Dom.event => Dom.eventTarget = "";
[@bs.get] external value: Dom.eventTarget => float = "";

let canvas = createElement("canvas", domProps());
[@bs.scope "document"] [@bs.val] external body: domNode = "";
appendChild(body, canvas)

let visualize: (Spring.state, (. float, Spring.state) => Spring.state, (. Spring.state) => bool) => unit = [%bs.raw {|
  function (state, advance, isAtRest) {
    canvas.width = 500
    canvas.height = 500
    const ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, 500, 500);
    ctx.strokeStyle = 'black'
    ctx.lineWidth = 3;
    ctx.beginPath();
    ctx.moveTo(0, 350);
    ctx.stroke()
    for (var i=0; i<500; i++) {
      if (isAtRest(state)) {
        break
      }
      state = advance(1, state);
      ctx.lineTo(i, 250 + state[2] * 100)
    }
    ctx.stroke();
  }
|}];

let first = <div id="awesome">
  {String("Hello")}
  <input _type="range" oninput={evt => {
    let v = evt->target->value;
    /* let config = Spring.niceConfig(~dampingRatio, ~frequencyResponse=frequencyResponseMs /. 1000.); */
    let stiffness = 10. *. (v +. 1.);
    let config = {
      Spring.damping: Spring.dampingFromStiffness(1., stiffness),
      stiffness: stiffness,
      restDisplacementThreshold: 0.001,
      restVelocityThreshold: 0.001,
    };
    let state = Spring.init(~initialVelocity = 0., config);
    visualize(state, (. delta, state) => Spring.advance(delta, state), (. state) => Spring.isAtRest(state));

  }} />
  <div id="here">
    <div>{String("What")}</div>
  </div>
  <Toggle
    on=(onClick => <div>
      (String("Click this to"))
      <Button style="background-color: #88ff88" onClick text="Turn Off" />
    </div>)
    off=(onClick => <div>
      <Button style="background-color: #ffacf0" onClick text="Turn On" />
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
