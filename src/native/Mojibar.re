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
let emojis = emojis->Belt.List.keep(emoji => Fluid.App.isEmojiSupported(emoji.char));
/* let emojis = emojis->Belt.List.take(10)->force; */
let (|?>) = (x, fn) => switch x { |None => None| Some(x) => fn(x)};

/** Count: 1570 */

/* let has = (text, rx) => switch (Str.search_forward(rx, text, 0)) {
  | exception Not_found => false
  | n => true
}; */


let has = (text, rx) => Str.string_match(rx, text, 0);

let main = (~onDone, hooks) => {
  let%hook (text, setText) = useState("");
  let%hook (selection, setSelection) = useState(0);

  let rx = Str.regexp(".*" ++ Str.quote(text) ++ ".*");
  let filtered = text == "" ? emojis : emojis->Belt.List.keep(emoji =>
    emoji.name->has(rx) ||
    emoji.keywords->Belt.Array.some(has(_, rx))
  );
  let fontSize = 17.;
  let size = fontSize *. 1.6;

  let rowf = 280. /. size;
  let row = int_of_float(rowf);
  let rows = ceil(float_of_int(List.length(filtered)) /. rowf)->int_of_float;

  /* print_endline("Render emojis: " ++ string_of_int(List.length(filtered))); */

  let%hook draw = useCallback(({top, left, width, height}) => {
    /* print_endline(
      "Ok drawing " ++ string_of_float(top) ++ " " ++ string_of_float(height),
    ); */
    filtered->Belt.List.forEachWithIndex((index, emoji) => {
      let x = index mod row |> float_of_int |> (*.)(size);
      let y = index / row |> float_of_int |> (*.)(size);
      if (y +. size >= top && y <= top +. height) {
        if (index == selection) {
          Fluid.Draw.rect({
            left: x,
            top: y,
            width: size,
            height: size,
          }, {
            r: 0.,
            g: 0.,
            b: 1.,
            a: 0.5,
          })
        };
        Fluid.Draw.text(~fontSize, emoji.char, {x: x +. 2., y: y +. 2.});
      };
    });
  }, (text, selection));

  <view layout={Layout.style(
    ~width=300.,
    ~height=200.,
    /* ~padding=10., */
    ()
  )}
  >
    <text
      contents=text
      layout={Layout.style(
        ~alignSelf=AlignStretch,
        ~marginHorizontal=10.,
        ~marginBottom=15.,
        ~marginTop=5.,
        ())}
      onEnter={text => {
        switch (Belt.List.get(filtered, selection)) {
          | None => onDone(None)
          | Some({char}) => onDone(Some(char))
        };
        setSelection(0);
        setText("");
      }}
      onEscape={() => {
        if (text == "") {
          onDone(None)
        } else {
          setSelection(0);
          setText("");
        }
      }}
      onTab={() => {
        setSelection(selection + 1)
      }}
      onShiftTab={() => {
        setSelection(max(0, (selection == 0 ? List.length(filtered) : selection) - 1))
      }}
      onChange={text => {
        /* print_endline("Onchange text " ++ text); */
        setText(text)
        setSelection(0)
      }}
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
          Layout.style(~padding=10., ~alignSelf=AlignStretch, ())
        }>
      <custom
        layout={Layout.style(~alignSelf=AlignStretch, ~height=(float_of_int(rows) *. size), ())}
        draw={draw}
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
  );
  Fluid.Hotkeys.init();

  let closeWindow = ref(() => ());

  let win = Fluid.launchWindow(
    ~title="Hello Fluid",
    ~floating=true,
    ~hidden=true,
    /* ~pos, */
    ~onBlur=win => {
      Fluid.Window.hide(win);
    },
    <Main onDone={text => {
      switch (text) {
        | Some(text) =>
          closeWindow^();
          Fluid.App.hide();
          Fluid.App.setTimeout(() => {
            Fluid.App.triggerString(text)
          }, 1000 * 1000 * 100)
        | None =>
          closeWindow^();
          Fluid.App.hide();
      }
    }}/>
  );

  closeWindow := () => Fluid.Window.hide(win);

  let statusBarItem = Fluid.App.statusBarItem(
    ~title="Mojibar",
    ~onClick=pos => {
      Fluid.Window.showAtPos(win, pos)
    }
  );

  Fluid.Hotkeys.register(~key=0x31, () => {
    print_endline("Got it!");
    Fluid.Window.showAtPos(win, Fluid.App.statusBarPos(statusBarItem));
  })->ignore;

  Fluid.Window.showAtPos(win, Fluid.App.statusBarPos(statusBarItem));
});
