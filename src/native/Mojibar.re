open FluidMac;

let str = Fluid.string;

open Fluid.Hooks;

let text = Files.readFileExn("./emojis.json");
let emojis = Json.parse(text);
let force = x => switch x { |None => failwith("Force unwrapped nil") | Some(x) => x};
type emoji = {
  name: string,
  keywords: array(string),
  char: string,
  fitz: bool,
  category: string,
};
let emojis = force(Json.obj(emojis))->Belt.List.map(((name, emoji)) => {
  let keywords = Json.array(emoji |> Json.get("keywords") |> force)->force->Belt.List.map(item => force(Json.string(item)))->Belt.List.toArray;
  let char = Json.string(emoji |> Json.get("char") |> force) |> force;
  let category = Json.string(emoji |> Json.get("category") |> force) |> force;
  {
    name,
    keywords,
    char,
    fitz: false,
    category,
  }
});
let emojis = emojis->Belt.List.take(10)->force;
let (|?>) = (x, fn) => switch x { |None => None| Some(x) => fn(x)};

/** Count: 1570 */

/* let has = (text, rx) => switch (Str.search_forward(rx, text, 0)) {
  | exception Not_found => false
  | n => true
}; */


let has = (text, rx) => Str.string_match(rx, text, 0);

let main = hooks => {
  let%hook (text, setText) = useState("");

  let rx = Str.regexp(".*" ++ Str.quote(text) ++ ".*");
  let filtered = text == "" ? emojis : emojis->Belt.List.keep(emoji =>
    emoji.name->has(rx) ||
    emoji.keywords->Belt.Array.some(has(_, rx))
  );
  let size = 20.;
  let row = int_of_float(300. /. 20.);
  let rows = List.length(filtered) / row;

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
        layout={Layout.style(~alignSelf=AlignStretch, ~height=(float_of_int(rows) *. size), ())}
        draw={({top, left, width, height}) => {
          print_endline("Ok drawing " ++ string_of_float(top) ++ " " ++ string_of_float(height));
          filtered->Belt.List.forEachWithIndex((index, emoji) => {
            let x = index mod row |> float_of_int;
            let y = index / row |> float_of_int;
            if (y *. size +. size >= top && y *. size <= top +. height) {
              Fluid.Draw.text(
                emoji.char,
                {x: x *. size,
                y: y *. size}
              )
            }
          })
        }}
      />
        </view>
      ],
      ()
    )}
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
      print_endline("Launching");
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
