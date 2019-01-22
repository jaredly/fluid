open FluidMac;

let str = Fluid.string;

open Fluid.Hooks;

let text = Files.readFileExn("./emojis.json");
let emojis = Json.parse(text);
let force = x => switch x { |None => failwith("Force unwrapped nil") | Some(x) => x};
let emojis = force(Json.obj(emojis)) -> Belt.List.take(200) -> force;
let (|?>) = (x, fn) => switch x { |None => None| Some(x) => fn(x)};

/** Count: 1570 */

let main = hooks => {
  let%hook (text, setText) = useState("");

  <view layout={Layout.style(
    ~width=300.,
    ~height=200.,
    ~padding=10.,
    ()
  )}
  >
    <text
      contents=text
      layout={Layout.style(~alignSelf=AlignStretch, ~marginVertical=10., ())}
      onChange={setText}
    />
    <text contents="Top"/>
    {Fluid.Native.scrollView(
      ~layout={Layout.style(
        ~overflow=Scroll,
        ~flexGrow=1.,
        ~alignSelf=AlignStretch,
        ~flexShrink=1.,
        ()
      )},
      ~children=[
        <view layout={
          Layout.style(~alignSelf=AlignStretch, ())
        }>
      <text contents="Hello"/>
      <custom
        layout={Layout.style(~alignSelf=AlignStretch, ~height=100., ())}
        draw={() => {
          print_endline("Ok drawing");
        }}
      />
        </view>
      ],
      ()
    )}
      /* <text contents="Hello"/> */
      /* <custom
        layout={Layout.style(~alignSelf=AlignStretch, ~height=100., ())}
        draw={() => {
          print_endline("Ok drawing");
        }}
      /> */
    /* </scrollView> */
    <text contents="Bottom"/>
    /* {Fluid.Native.view(~layout={Layout.style(
      ~flexWrap=CssWrap,
      ~flexDirection=Row,
      ()
      )},
    ~children={
      emojis->Belt.List.map(((name, defn)) => {
        let char = force(defn |> Json.get("char") |?> Json.string);
        <button
          title=char
          onPress={() => {
            print_endline("Selected: " ++ char)
          }}
        />
      })
    },
    ()
    )} */
  </view>
};


Fluid.App.launch(
  ~isAccessory=true,
  () => {
  Fluid.App.setupAppMenu(
    ~title="Mojibar",
    ~appItems=[||],
    ~menus=[| Fluid.App.defaultEditMenu() |]
  )

  Fluid.App.statusBarItem(
    ~title="Mojibar",
    ~onClick=pos => {
      Fluid.launchWindow(
        ~title="Hello Fluid",
        ~floating=true,
        ~pos,
        ~onBlur=win => {
          print_endline("Blurred! Ok cleaning now");
          Fluid.Window.close(win);
        },
        <Main />
      );
    }
  )
});
