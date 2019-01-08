
module Awesome = {
  type props = Props;
  let props = (~name, ~age, ()) => Props;
  let make = (props: props) => "Good";
};

module Fluid = {
  let builtin = (name: string, nativeProps: unit, children: list(string)) => "Hello";
  module NativeInterface = {
    let nativeProps = (~id=?, ~onclick=?, ()) => ();
  }
  type m =
  | Custom(unit)
  | String(string)
  | Builtin(unit, list(m), option(unit));
  module Maker = {
    let makeComponent = (fn, render) => ()
  };
  module Native = {
    let div = (~id=?, ()) => ();
    let span = (~id=?, ()) => ();
  };
  type context('a) = {
    hooks: unit,
    finish: 'a => unit,
  };
  let finish = x => ();
};

let awesome = (~name, ~age, ctx) => ();

let x = <div id=10>
  {Fluid.String("Hello")}
  <Awesome name="hello" age=45 />
  {Fluid.String("Here")}
  <span id="Hello">{Fluid.String("Things")}</span>
</div>;

let useState = (v, hooks, fn) => {
  let (a, b) = fn((v, a => ()), hooks);
  (a, b)
}

let awesome = (awesome, ctx) => {
  let%hook (x, _) = useState(10);
  let%hook (y, _) = useState(10);
  15 + x + y
};

let myComponent = (~some, ~prop, ctx) => {
  Js.log("Here");
  /* let%hook (state, dispatch) = useReducer(None, action => switch action {
    | `Awesome => Some(10)
    | `Nope => None
  }); */
  let%hook (count, setCount) = useState(10);
  let%hook (name, setName) = useState("name");
  Js.log("Hi");
  /* let%hook () = useEffect(() => {
    () => ()
  }, ()); */
  Js.log("Ho");
  "contents"
};

