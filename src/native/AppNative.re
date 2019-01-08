open FluidMac;

print_endline("Hello folks");

let first = ctx => {
  open Fluid.Hooks;
  let%hook (times, setTimes) = useState(0);

  <view layout={Layout.style(~marginHorizontal=50., ~height=100., ())}>
    {Fluid.String("Hello world " ++ string_of_int(times))}
    <button
      onPress={
        () => {
          print_endline("Clicked");
          setTimes(times + 1);
        }
      }
      title="Hello to you too"
    />
    {Fluid.String("More world")}
    <view>
    <view layout={Layout.style(~width=40., ~height=40., ())} />
    {Fluid.String("These are a few of my best things")}
    {Fluid.String("Better now")}
    </view>
  </view>;
};

FluidMac.NativeInterface.startApp(~title="Hello Fluid", node =>  {

  Fluid.mount(<First />, node);
  print_endline("Done");
}
);

