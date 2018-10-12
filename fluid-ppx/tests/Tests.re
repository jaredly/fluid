
module Awesome = {
  type props = Props;
  let props = (~name, ~age, ()) => Props;
  let make = (props: props) => "Good";
};

module Fluid = {
  let builtin = (name: string, domProps: unit, children: list(string)) => "Hello";
  let domProps = (~id=?, ~onclick=?, ()) => ();
  type m =
  | Custom(string)
  | Builtin(string, unit, list(unit));
};

let x = <div>
  "Hello"
  <Awesome name="hello" age=45 />
  "Here"
  <span id="Hello">"Things"</span>
</div>;
