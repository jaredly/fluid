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


let colorSwitcher = ctx => {
  let%hook (color, setColor) = useState(red);
  <view>
    <view
      backgroundColor=color
      layout={Layout.style(~width=100., ~height=30., ())}
    />
    <button onPress={() => setColor(nextColor(color))} title="Toggle" />
    <view layout={Layout.style(~height=30., ())} />
  </view>;
};

let first = ctx => {
  let%hook (times, setTimes) = useState(0);

  <view layout={Layout.style(~marginHorizontal=10., ())}>
    {str("More world")}
    <view>
      <ColorSwitcher />
      <view
        layout={Layout.style(~width=40., ~height=40., ())}
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
    <view layout={Layout.style(~height=30., ())} />
  </view>;
};

Fluid.launchWindow(~title="Hello Fluid", ~root=<First />);