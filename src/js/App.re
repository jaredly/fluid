[%%debugger.chrome];
open FluidDom;
/* open Hooks; */

module Style = {
  type style;
  [@bs.get] external style: NativeInterface.nativeNode => style = "";
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

/* let abs: NativeInterface.nativeNode => unit = [%bs.raw {|
function(node) {
  const box = node.getBoundingClientRect();
  node.style.position = 'absolute';
  node.style.pointerEvents = 'none';
  node.style.top = box.top + 'px';
  node.style.left = box.left + 'px';
}
|}] */

let fade = (node, ~out) => {
  /* if (out) {
    abs(node);
  }; */
  Animate.spring(
    ~dampingRatio=1.,
    ~frequencyResponseMs=10.,
    (amount) => {
      node->Style.style->Style.opacity(out ? amount :  1. -. amount);
    },
    () => {
      if (out) {

      node->NativeInterface.parentNode->NativeInterface.removeChild(node)
      }
    }
  )
};

let toggle = (~on, ~off, ctx) => {
  open Fluid.Hooks;
  let%hook (isOn, setOn) = useState(false);

  let%hook () = useReconciler(isOn, (oldState, newState, mountedTree, newTree) => {
    Js.log3("reconciling I guess", oldState, newState);
    switch (oldState, newState) {
      | (false, true)
      | (true, false) =>
        open Fluid;
        let nativeNode = getNativeNode(mountedTree);
        let newTree = inflateTree(instantiateTree(newTree));
        let newNativeNode = getNativeNode(newTree);

        let dist = 30.;

        nativeNode->translate(newState ? -. dist : dist, 0.);
        nativeNode->fade(~out=true);
        newNativeNode->translate(newState ? -. dist : dist, newState ? dist : -. dist);
        newNativeNode->fade(~out=false);

        nativeNode->NativeInterface.parentNode->NativeInterface.insertBefore(newNativeNode, ~reference=nativeNode);

        newTree
      | _ => mountedTree
    }
  });

  Js.log2("Is On", isOn);
  if (isOn) {
    on(() => setOn(false))
  } else {
    off(() => setOn(true))
  }
};


/* [@memo] */
let awesomeComponent = (~value, ~toString, ctx) => {
  let%hook (state, setState) = Fluid.Hooks.useState("Awesome");
  <div>
    <div onclick={_evt => setState(state ++ "1")}>
      {String("Folkx " ++ toString(value) ++ " " ++ state)}
    </div>
  </div>
};

let button = (~text, ~style, ~onClick, ctx) => {
  <button style onclick={_evt => onClick()}>(String(text))</button>
};

[@bs.get] external target: Dom.event => Dom.eventTarget = "";
[@bs.get] external value: Dom.eventTarget => float = "";
[@bs.scope "document"] [@bs.val] external body: NativeInterface.nativeNode = "";

[@bs.send] external addEventListener: (NativeInterface.nativeNode, string, 'evt => unit) => unit = "";
[@bs.set] external textContent: (NativeInterface.nativeNode, string) => unit = "";

let simple = <div id="awesome" layout={Layout.style(~width=500., ~height=500., ())}>
  {String("Hello")}
  <div id="here" layout={Layout.style(~width=100., ~height=50., ())}>
    <div>{String("What")}</div>
  </div>
</div>;

let first = <div id="awesome" layout={Layout.style(~width=500., ~height=500., ())}>
  {String("Hello")}
  <div id="here" layout={Layout.style(~width=100., ~height=50., ())}>
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
  <AwesomeComponent value=5 toString=string_of_int />
  <AwesomeComponent value="Hi" toString={x => x} />
  <div
    id="Inner"
  >
    {String("world")}
  </div>
</div>;

[@bs.val][@bs.scope "document"] external getElementById: string => option(NativeInterface.nativeNode) = "";

switch (getElementById("root")) {
  | None => assert(false)
  | Some(node) => Fluid.mount(first, node)
}
