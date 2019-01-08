open FluidMac;

print_endline("Hello folks");

let first = ctx => {
  open Fluid.Hooks;
  let%hook (times, setTimes) = useState(0);

  <view>
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
  </view>;
};

FluidMac.NativeInterface.startApp(~title="Hello Fluid", node =>  {

  Fluid.mount(<First />, node);
  print_endline("Done");
}
);

