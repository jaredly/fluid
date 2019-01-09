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


/* let invalidate = hooks => {
  let%hook (count, setCount) = useState(0);
}; */

Fluid.launchWindow(~title="Hello Fluid", ~root=<First />);