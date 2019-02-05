
open FluidMac;
open Fluid.Native;
open Fluid.Hooks;

let str = Fluid.string;

let%component drawing = (hooks) => {
  let%hook draw = useCallback((rect) => {
    open Fluid.Draw;
    text(~fontSize=20., "Hello", {x: 10., y: 0.});
    rect({left: 3., top: 3., width: 20., height: 20.}, {r: 1., g: 0., b: 0., a: 1.});
    fillRect({left: 18., top: 7., width: 10., height: 10.}, {r: 0., g: 1., b: 1., a: 1.});
  }, ());

  <custom layout={Layout.style(~width=60., ~height=30., ())} draw />
};

let%component scrolling = hooks => {
  <scrollView
  layout={Layout.style(
    ~height=40.,
    ~alignItems=AlignStretch,
    ~alignSelf=AlignStretch,
    ~overflow=Scroll,
    (),
  )}>
    <view layout={Layout.style(~alignItems=AlignStretch, ())}>
      {str("One")}
      {str("Two")}
      {str("Three")}
      {str("Four")}
      {str("Five")}
      {str("Six")}
      {str("Seven")}
    </view>
  </scrollView>
};

let%component main = (hooks) => {
  let%hook (on, setOn) = useState(false);
  <view>
    {str("Hello folks")}
    {str(~font={fontName: "Helvetica", fontSize: 24.}, "Hello folks")}
    <view layout={Layout.style(~flexDirection=Row, ())}>
      <view
        backgroundColor={r: 1., g: 0., b: 1., a: 0.4}
        layout={Layout.style(~padding=10., ())}
      >
        <image
          src={ Plain("http://google.com/favicon.ico")}
          layout={Layout.style(~width=32., ~height=32., ())}
        />
      </view>
      <drawing />
    </view>
    <button onPress={() => setOn(!on)} title="Do things I guess" />
    {on
      ? str("It's on!")
      : Fluid.Null}
    <scrolling />
  </view>
};

Fluid.App.launch(() => {
  Fluid.App.setupAppMenu(
    ~title="KitchenSink",
    ~appItems=[||],
    ~menus=[| Fluid.App.defaultEditMenu() |]
  );
  let win = Fluid.launchWindow(
    ~title="KitchenSink",
    <main />
  )
});