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

let first = hooks => {
  let%hook (times, setTimes) = useState(0);

  <view layout={Layout.style(~marginHorizontal=10., ())}>
    {str("More world")}
    <view>
      <ColorSwitcher />
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
          print_endline("Clicked");
          setTimes(times + 1);
        }
      }
      title="Hello to you too"
    />
  </view>;
};

/* type state = {value: float, elements: list(element)}; */
type op = [ | `plus | `div | `minus | `times ];

let opString = op => switch op {
  | `plus => "+"
  | `div => "/"
  | `minus => "-"
  | `times => "*"
};

let doOp = (left, op, right) => switch op {
  | `plus => left +. right
  | `div => left /. right
  | `minus => left -. right
  | `times => left *. right
};

type input = {neg: bool, left: list(int), right: option(list(int))};

type state =
  {value: input, prev: option((string, float, op)), history: list((string, string, float))};

let emptyValue = {left: [], right: None, neg: false};
let emptyState = {value: emptyValue, prev: None, history: []};

type action =
  | Number(int)
  | Op(op)
  | Eq
  | Clear
  | AC
  | Backspace
  | Dot;

let addNum = (input, num) => switch (input.right) {
  | None => {...input, left: input.left == [0] && num == 0 ? [0] : input.left @ [num]}
  | Some(right) => {...input, right: Some(right @ [num])}
};

let toString = ({left, right, neg}) => {
  if (left == []) {
    "0"
  } else {
  let left = String.concat("", left |> List.map(string_of_int));
  let left = neg ? "-" ++ left : left;
  switch right {
    | None => left
    | Some(right) =>
      let right = String.concat("", right |> List.map(string_of_int));
      left ++ "." ++ right
  }
  }
};

let toFloat = ({left, right, neg}) => {
  let left = String.concat("", left |> List.map(string_of_int));
  let left = neg ? "-" ++ left : left;
  print_endline("Left toFloat " ++ left);
  switch right {
    | None => left == "" || left == "-" ? 0. : float_of_string(left)
    | Some(right) =>
      let right = String.concat("", right |> List.map(string_of_int));
      float_of_string(left ++ "." ++ right)
  }
};

let eval = state => {
  switch (state) {
    | {value: {left: []}} => state
    | {prev: None, value, history} => 
      {value: emptyValue, prev: None, history: [(toString(value), toString(value), toFloat(value)), ...history]}
    | {prev: Some((pstr, pval, op)), value, history} =>
      let result = doOp(pval, op, toFloat(value));
      {value: emptyValue, prev: None, history: [(
        pstr ++ opString(op) ++ toString(value),
        string_of_float(result),
        result
      )]}
  }
};

let rec butLast = l => switch l {
  | [one] => []
  | [] => []
  | [one, ...more] => [one, ...butLast(more)]
};

let backspace = value => switch value {
  | {right: Some([])} => {...value, right: None}
  | {right: Some(right)} => {...value, right: Some(butLast(right))}
  | {left: []} => emptyValue
  | _ => {...value, left: butLast(value.left)}
};

let reduce = (state, action) => switch (action, state) {
  | (Number(d), _) => {...state, value: addNum(state.value, d)}
  | (Dot, {value: {right: Some(_)}}) => state
  | (Dot, _) => {...state, value: {...state.value, right: Some([])}}
  | (Backspace, _) => {...state, value: backspace(state.value)}
  /* | (Op((`plus | `minus) as op), {value: {left: [], neg}}) =>
    {...state, value: {...state.value, neg: op == `minus}} */
  | (Op(_), {value: {left: []}, history: []}) => state
  | (Op(op), {value: {left: []}, history: [(_, strv, v), ..._]}) => {...state, prev: Some((strv, v, op))}
  | (Op(op), {value}) => 
    let state = eval(state);
    switch (state.history) {
      | [] => state
      | [(_, strv, v), ..._] =>
    {...state, prev: Some((strv, v, op))}
    }
  | (Eq, _) => eval(state)
  | (Clear, state) => {...state, prev: None, value: emptyValue}
  | (AC, _) => emptyState
};

let showHistory = (prev, h) => {
  switch prev {
    | Some((s, _, op)) => s ++ opString(op)
    | None =>
  switch h {
  | [] => "Hi..........."
  | [(str, restr, _), ..._] => str == restr ? str : (str ++ "=" ++ restr)
};
  }
};

let calculator = hooks => {
  let%hook (state, dispatch) = useReducer(emptyState, reduce);
  let go = a => () => dispatch(a);
  Printf.printf("Current: %f\n", toFloat(state.value));
  print_endline("ok");

  <view>
    {str("Calculator")}
    <view layout={Layout.style(~height=30., ~alignSelf=AlignStretch, ())}>
      {str(~layout=(
      Layout.style(~flexGrow=1., ~alignSelf=AlignStretch, ())

      ), showHistory(state.prev, state.history))}
    </view>
    {str(~font={
      Fluid.NativeInterface.fontName: "Helvetica", fontSize: 20.
    }, ~layout={
      Layout.style(~flexGrow=1., ~alignSelf=AlignStretch, ())
    }, toString(state.value))}
    <view>
      <view layout={Layout.style(~flexDirection=Row, ())}>
        <button title="1" onPress={go(Number(1))} />
        <button title="2" onPress={go(Number(2))} />
        <button title="3" onPress={go(Number(3))} />
      </view>
      <view layout={Layout.style(~flexDirection=Row, ())}>
        <button title="4" onPress={go(Number(4))} />
        <button title="5" onPress={go(Number(5))} />
        <button title="6" onPress={go(Number(6))} />
      </view>
      <view layout={Layout.style(~flexDirection=Row, ())}>
        <button title="7" onPress={go(Number(7))} />
        <button title="8" onPress={go(Number(8))} />
        <button title="9" onPress={go(Number(9))} />
      </view>
      <view layout={Layout.style(~flexDirection=Row, ())}>
        <button title="0" onPress={go(Number(0))} />
        <button title="." onPress={go(Dot)} />
        <button title="+" onPress={go(Op(`plus))} />
      </view>
      <view layout={Layout.style(~flexDirection=Row, ())}>
        <button title="-" onPress={go(Op(`minus))} />
        <button title="=" onPress={go(Eq)} />
      </view>
    </view>
  </view>
};

/* let invalidate = hooks => {
  let%hook (count, setCount) = useState(0);
}; */

Fluid.launchWindow(~title="Hello Fluid", ~root=<Calculator />);