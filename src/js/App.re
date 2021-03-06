[%%debugger.chrome];
Belt.Debug.setupChromeDebugger();
open FluidDom;
/* open Hooks; */

module Style = {
  type style;
  [@bs.get] external style: NativeInterface.nativeNode => style = "";
  [@bs.set] external transform: (style, string) => unit = "";
  [@bs.set] external opacity: (style, float) => unit = "";
};

let str = Fluid.string;

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

let toggle = (~on, ~off, hooks) => {
  open Fluid.Hooks;
  let%hook (isOn, setOn) = useState(false);

  /* let%hook () = useReconciler(isOn, (oldState, newState, mountedTree, newTree) => {
    Js.log3("reconciling I guess", oldState, newState);
    switch (oldState, newState) {
      | (false, true)
      | (true, false) =>
        open Fluid;
        let nativeNode = getNativeNode(mountedTree);
        let newInst = instantiateTree(newTree);
        Layout.layout(Fluid.getInstanceLayout(newInst));
        let newTree = inflateTree(newInst);
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
  }); */

  Js.log2("Is On", isOn);
  if (isOn) {
    on(() => setOn(false))
  } else {
    off(() => setOn(true))
  }
};


/* [@memo] */
let awesomeComponent = (~value, ~toString, hooks) => {
  let%hook (state, setState) = Fluid.Hooks.useState("Awesome");
  <div>
    <div onclick={_evt => setState(state ++ "1")}>
      {str("Folkx " ++ toString(value) ++ " " ++ state)}
    </div>
  </div>
};

let button = (~text, ~style, ~onClick, hooks) => {
  <button style layout={Layout.style(~paddingVertical=4., ~paddingHorizontal=8., ())} onclick={_evt => onClick()}>(str(text))</button>
};

[@bs.get] external target: Dom.event => Dom.eventTarget = "";
[@bs.get] external value: Dom.eventTarget => float = "";
[@bs.scope "document"] [@bs.val] external body: NativeInterface.nativeNode = "";

[@bs.send] external addEventListener: (NativeInterface.nativeNode, string, 'evt => unit) => unit = "";
[@bs.set] external textContent: (NativeInterface.nativeNode, string) => unit = "";

let simple = <div id="awesome" layout={Layout.style(~width=500., ~height=500., ())}>
  {str("Hello")}
  <div id="here" layout={Layout.style(~width=100., ~height=50., ())}>
    <div>{str("What")}</div>
  </div>
</div>;

module ImageCache = Fluid.Cache({
  type arg = string;
  type result = unit;
  let reason = Fluid.noReason;
  let fetch = (arg, fin) => {
    Js.Global.setTimeout(fin, 500 + 10 * Random.int(100))->ignore;
    ()
  }
});

let imageLoader = (~src, hooks) => {
  let data = ImageCache.fetch(src);

  <img src layout={Layout.style(~width=200., ~height=200., ())} />
};

let loading = (~children=[], hooks) => {
  let%hook suspended = Fluid.Hooks.useSuspenseHandler();

  Js.log2("Rerender", suspended);
  if (suspended != []) {
    <div>{str("Preloading " ++ string_of_int(List.length(suspended)) ++ " images...")}</div>
  } else {
    Fluid.Native.div(~children, ())
  }
};

let loadExample = hooks => <div layout={Layout.style(~width=600., ~padding=20., ())}>
  {str(~font={fontName: "system-ui", fontSize: 40.}, "Suspense y'all")}
  <Loading>
    <div layout={Layout.style(~flexDirection=Row, ~flexWrap=CssWrap, ())}>
      <ImageLoader src="./fluid-macos.png" />
      <ImageLoader src="./fluid-js.png" />
    </div>
  </Loading>
</div>

let fakeLoader = (~contents, hooks) => {
  let data = ImageCache.fetch("");

  contents
};

let toggle2 = hooks => {
  let%hook (on, set) = Fluid.Hooks.useState(false);
  if (on) {
    <div>
      {str("Yes")}
      <Button style="" onClick=(() => set(!on)) text="Click me" />
    </div>
  } else {
    <div>
    <div>
      {str("Hello")}
      <Button style="" onClick=(() => set(!on)) text="Turn on" />
      {str("On")}
      {str("More")}
      </div>
    </div>
  }
};

let toggle3 = hooks => {
  let%hook (on, set) = Fluid.Hooks.useState(false);
  if (on) {
    <div>
      {str("Yes")}
      <Button style="" onClick=(() => set(!on)) text="Click me" />
    </div>
  } else {
    <div>
      {str("Hello")}
      <Button style="" onClick=(() => set(!on)) text="Turn on" />
      {str("On")}
      {str("More")}
    </div>
  }
};

let first = (hooks) => {
  <div id="awesome" layout={Layout.style(~width=500., ())}>
  {str("Hello")}
  <div id="here" style="background-color: #aaa" layout={Layout.style(~width=100., ~height=50., ())}>
    <div>{str("What")}</div>
  </div>
  <Loading>
    <div layout={Layout.style(~flexDirection=Row, ~flexWrap=CssWrap, ())}>
      <ImageLoader src="./fluid-macos.png" />
      <ImageLoader src="./fluid-macos1.png" />
      <ImageLoader src="./fluid-macos2.png" />
      <ImageLoader src="./fluid-macos3.png" />
      <ImageLoader src="./fluid-macos4.png" />
      <ImageLoader src="./fluid-macos5.png" />
    </div>
  </Loading>
  <img src="./fluid-macos.png" layout={Layout.style(~width=200., ~height=200., ())} />
  <Toggle
    on=(onClick => <div layout={Layout.style(~flexDirection=Row, ~alignItems=AlignCenter, ())}>
      (str("Click this to"))
      <Button style="background-color: #88ff88" onClick text="Turn Off" />
    </div>)
    off=(onClick => <div layout={Layout.style(~flexDirection=Row, ~alignItems=AlignCenter, ())}>
      <Button style="background-color: #ffacf0" onClick text="Turn On" />
      (str("if you want"))
    </div>)
  />
  <AwesomeComponent value=5 toString=string_of_int />
  <AwesomeComponent value="Hi" toString={x => x} />
  <div
    id="Inner"
  >
    {str("world")}
  </div>
</div>;
};

[@bs.val][@bs.scope "document"] external getElementById: string => option(NativeInterface.nativeNode) = "";

switch (getElementById("root")) {
  | None => assert(false)
  | Some(node) => Fluid.mount(<LoadExample />, node)
}
