open FluidMac;

let str = Fluid.string;

open Fluid.Hooks;

let red = {r: 1., g: 0., b: 0., a: 1.};
let green = {r: 0., g: 1., b: 0., a: 1.};
let blue = {r: 0., g: 0., b: 1., a: 1.};

let nextColor = color =>
  if (color == red) {
    green;
  } else if (color == green) {
    blue;
  } else {
    red;
  };


let colorSwitcher = hooks => {
  let%hook (color, setColor) = useState(red);
  <view>
    <view
      backgroundColor=color
      layout={Layout.style(~width=100., ~height=30., ())}
    />
    <view layout={Layout.style(~flexDirection=Row, ())}>
    <button onPress={() => setColor(red)} title="Red" />
    <button onPress={() => setColor(green)} title="Green" />
    <button onPress={() => setColor(blue)} title="Blue" />
    </view>
  </view>;
};

type async('t) = ('t => unit) => unit;

type suspendReason = ..;

type suspendReason += LoadingImage(string);

type suspendEvent = {
  reason: suspendReason,
  payload: async(unit)
};

/*

Because what I want is for the parent to be able to have an idea of what it's waiting on.

hrmmm and I guess we don't need to track the type of the thing that's going to be returned... ok.

 */

/* let useSuspense = (~filter=(_) => true, (), hooks) => {
  let next = switch (hooks.current^) {
    | None => ref(None)
    | Some((r, next)) =>
      next
  };
  hooks.current := Some(((), Next))
  ((), hooks)
}; */

let first = hooks => {
  let%hook (times, setTimes) = useState(0);

  /* let%hook status = useSuspense(~filter=(reason) => switch reason {
    | LoadingImage(string) => true
    | _ => false
    /*
    I think the type of useSuspense should be
    let useSuspense: (~filter: (list(suspendEvent)))
     */
  }, ()); */

  <view layout={Layout.style(~marginHorizontal=10., ())}>
    {str("More world")}
    <view>
      <ColorSwitcher />
      <image src="./fluid-js.png" layout={Layout.style(~margin=20., ~width=100., ~height=100., ())} />
      <view
        layout={Layout.style(~width=40., ~height=40., ~alignSelf=AlignCenter, ())}
        backgroundColor={r: 1., g: 0., b: 0., a: 1.}
      />
      {str("These are a few of my best things")}
      {str("Better now")}
    </view>
    {str("Hello world " ++ string_of_int(times))}
    <button
      onPress={
        () => {
          /* print_endline("Clicked"); */
          setTimes(times + 1);
        }
      }
      title="Hello to you too"
    />
  </view>;
};


/* let invalidate = hooks => {
  let%hook (count, setCount) = useState(0);
}; */

Fluid.App.launch(() => {
  Fluid.launchWindow(~title="Hello Fluid", ~floating=false, <First />);
});

/* Fluid.launchWindow(~title="Hello Fluid", ~root=<Calculator.Calculator />); */