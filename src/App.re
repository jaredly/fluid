
open Fluid;
open Hooks;

module Style = {
  type style;
  [@bs.get] external style: domNode => style = "";
  [@bs.set] external transform: (style, string) => unit = "";
  [@bs.set] external opacity: (style, float) => unit = "";
};

let translate = (node, dx, offset) => {
  Animate.spring(
    ~dampingRatio=1.,
    ~frequencyResponseMs=10.,
    (amount) => {
      node->Style.style->Style.transform("translateX(" ++ string_of_float((1. -. amount) *. dx +. offset) ++ "px)");
    },
    () => {
      ()
      /* node->parentNode->removeChild(node) */
    }
  )
};

let abs: domNode => unit = [%bs.raw {|
function(node) {
  const box = node.getBoundingClientRect();
  node.style.position = 'absolute';
  node.style.pointerEvents = 'none';
  node.style.top = box.top + 'px';
  node.style.left = box.left + 'px';
}
|}]

let fade = (node, ~out) => {
  if (out) {
    abs(node);
  };
  Animate.spring(
    ~dampingRatio=1.,
    ~frequencyResponseMs=10.,
    (amount) => {
      node->Style.style->Style.opacity(out ? amount :  1. -. amount);
    },
    () => {
      if (out) {

      node->parentNode->removeChild(node)
      }
    }
  )
};

let toggle = (~on, ~off, ctx) => {
  let (res, hooks) = useState(false, ctx.hooks, ((isOn, setOn), hooks) => {
    hooks.setReconciler(isOn, (oldState, newState, mountedTree, newTree) => {
      Js.log3("reconciling I guess", oldState, newState);
      switch (oldState, newState) {
        | (false, true)
        | (true, false) =>
          let domNode = getDomNode(mountedTree);
          let newTree = inflateTree(instantiateTree(newTree));
          let newDomNode = getDomNode(newTree);

          let dist = 30.;

          domNode->translate(newState ? -. dist : dist, 0.);
          domNode->fade(~out=true);
          newDomNode->translate(newState ? -. dist : dist, newState ? dist : -. dist);
          newDomNode->fade(~out=false);

          domNode->parentNode->insertBefore(newDomNode, ~reference=domNode);

          newTree
        | _ => mountedTree
      }
    });

    Js.log2("Is On", isOn);
    (if (isOn) {
      on(() => setOn(false))
    } else {
      off(() => setOn(true))
    }, hooks)

  });
  ctx.finish(hooks);
  res
};

let awesomeComponent = (~value, ~toString, ctx) => {
  let (res, hooks) = useState("Awesome", ctx.hooks, ((state, setState), hooks) => {
    (<div>
      <div onclick={_evt => setState(state ++ "1")}>
        {String("Awesome " ++ toString(value) ++ " " ++ state)}
      </div>
    </div>, hooks)
  });

  ctx.finish(hooks);
  res
};

/** Yayy polymorphism and a normal props thing! */
/* module Awesome = {
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
}; */

let button = (~text, ~style, ~onClick, ctx) => {
  <button style onclick={_evt => onClick()}>(String(text))</button>
};

/* module Button = {
  let props = (~text, ~style, ~onClick, ()) => (text, style, onClick);
  let make = Maker.component(
    ~name="Button",
    ~render=((text, style, onClick)) => {
      <button style onclick={_evt => onClick()}>(String(text))</button>
    },
    ()
  );
}; */

[@bs.get] external target: Dom.event => Dom.eventTarget = "";
[@bs.get] external value: Dom.eventTarget => float = "";
[@bs.scope "document"] [@bs.val] external body: domNode = "";

let canvas = createElement("canvas", domProps(~width=500, ~height=200, ()));
appendChild(body, canvas)

let canvas2 = createElement("canvas", domProps(~width=500, ~height=500, ()));
appendChild(body, canvas2);

let log = createElement("div", domProps());
appendChild(body, log);

[@bs.send] external addEventListener: (domNode, string, 'evt => unit) => unit = "";
[@bs.set] external textContent: (domNode, string) => unit = "";

let plot: (float, float, float, string) => unit = [%bs.raw {|
function (x, y, scale, color) {
  const ctx = canvas.getContext('2d');
  ctx.fillStyle = color
  ctx.fillRect(x, y, scale, scale)
}
|}];

let visualize: (domNode, Spring.state, (. float, Spring.state) => Spring.state, (. Spring.state) => bool) => unit = [%bs.raw {|
  function (canvas, state, advance, isAtRest) {
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
      state = advance(16, state);
      ctx.lineTo(i, 250 + state[2] * 100)
    }
    ctx.stroke();
  }
|}];

let howLong = (stiffness, damping) => {
  let config = {
    Spring.damping,
    stiffness,
    restDisplacementThreshold: 0.001,
    restVelocityThreshold: 0.001,
  };
  let state = Spring.init(~initialVelocity = 0., config);
  let rec loop = (didOvershoot, state, t) => {
    if (t > 1500) {
      (None, 0)
    } else if (Spring.isAtRest(state)) {
      (Some(didOvershoot), t)
    } else {
      loop(didOvershoot || state.Spring.currentValue < 0., Spring.advance(16., state), t + 1)
    }
  };
  loop(false, state, 0)
};

let showPlot = (stiffness, damping) => {
  let config = {
    Spring.damping,
    stiffness,
    restDisplacementThreshold: 0.001,
    restVelocityThreshold: 0.001,
  };
  let state = Spring.init(~initialVelocity = 0., config);
  visualize(canvas2, state, (. delta, state) => Spring.advance(delta, state), (. state) => Spring.isAtRest(state));
};

let scale = 1;
let zoom = 1.;

/*
237, 25.6; 95, 16.4; 12.5, 5.8; 3.5, 3; 2, 2.2; 246, 25.8
*/
[@bs.set] external awesome: (domNode, array((float, float))) => unit = "";

let changes = [||];

body->awesome(changes);

let sd = (x, y) => (x /. 2. /. zoom, 1. +. sqrt(y) /. 2. /. zoom);

let showLine = x => {
  let last = ref(Some(true));
  for (y in 0 to 200 / scale) {
    /* plot(x, y, ) */
    let x = float_of_int(x * scale);
    let y = float_of_int(y * scale);
    let (stiffness, damping) = sd(x, y);
    /* let stiffness = x /. 2. /. zoom;
    let damping = 1. +. y /. 5. /. zoom; */
    let (overshoot, t) = howLong(stiffness, damping);
    if (overshoot == Some(false) && last^ == Some(true)) {
      Js.log4("Change!", (overshoot, last^), stiffness, damping);
      Js.Array.push((stiffness, damping), changes) |> ignore;
    };
    last := overshoot;
    plot(x, y, float_of_int(scale), switch (overshoot) {
      | None => "black"
      | Some(true) => "rgba(255, 0, 0, " ++ string_of_float(float_of_int(t) /. 2000. +. 0.5) ++ ")"
      | Some(false) => "rgba(0, 255, 0, " ++ string_of_float(float_of_int(t) /. 2000. +. 0.5) ++ ")"
    })
  }
};

let rec loop = x => {
  showLine(x);
  if (x < 500 / scale) {
    Animate.requestAnimationFrame(() => loop(x + 1))
  }
};
/* loop(0); */

let f: unit => unit = [%bs.raw {|
  function(){
    const ctx = canvas.getContext('2d')
    ctx.beginPath()
    ctx.moveTo(0,0)
    for (let x = 0; x < 500; x++) {
      ctx.lineTo(x, Math.sqrt(2) * Math.sqrt(x / 2) * 5)
    }
    ctx.strokeStyle = 'black'
    ctx.lineWidth = 1
    ctx.stroke()
  }
|}];
f();

canvas->addEventListener("mousemove", evt => {
  let box = evt##target##getBoundingClientRect();
  let x = evt##clientX -. box##left;
  let y = evt##clientY -. box##top;
  let (stiffness, damping) = sd(x, y);
  /* let stiffness = 1. +. x /. 2.;
  let damping = 1. +. y /. 5.; */
  showPlot(stiffness, damping);
  let (overshoot, t) = howLong(stiffness, damping);
  log->textContent(Printf.sprintf("%f stiffness %f damping; %d steps", x /. 2., y /. 5., t))
});

let first = <div id="awesome" style="padding: 20px">
  {String("Hello")}
  <input _type="range" oninput={evt => {
    let v = evt->target->value;
    /* let config = Spring.niceConfig(~dampingRatio, ~frequencyResponse=frequencyResponseMs /. 1000.); */
    Js.log(v);
    /* 18 for 100. */
    let stiffness = 10. *. (v +. 1.);
    /* let config = Spring.niceConfig(
      ~dampingRatio=v /. 100.,
      ~frequencyResponse=0.1,
    ); */
    let config = {
      Spring.damping: v,
      /* Spring.damping: Spring.dampingFromStiffness(1., stiffness), */
      stiffness: 100.,
      restDisplacementThreshold: 0.001,
      restVelocityThreshold: 0.001,
    };
    let state = Spring.init(~initialVelocity = 0., config);
    visualize(canvas2, state, (. delta, state) => Spring.advance(delta, state), (. state) => Spring.isAtRest(state));

  }} />
  <div id="here">
    <div>{String("What")}</div>
  </div>
  <Toggle
    on=(onClick => <div>
      (String("Click this to"))
      {Custom(Maker.makeComponent(button, button(~style="background-color: #88ff88", ~onClick, ~text="Turn Off")))}
    </div>)
    off=(onClick => <div>
      {Custom(Maker.makeComponent(button, button(~style="background-color: #ffacf0", ~onClick, ~text="Turn On")))}
      (String("if you want"))
    </div>)
  />
  <AwesomeComponent value=5 toString=string_of_int />
  <AwesomeComponent value="Hi" toString={x => x} />
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
