open FluidMac;

let str = Fluid.string;

open Fluid.Hooks;

let fuzzysearch = (needle, haystack) => {
  let hlen = String.length(haystack);
  let nlen = String.length(needle);
  if (nlen > hlen) {
    false;
  } else if (nlen == hlen) {
    needle == haystack;
  } else if (nlen == 0) {
    true
  } else {
    let rec loop = (nch, ni, hi) => String.get(haystack, hi) == nch
    ? (
      ni == nlen - 1 ? true : (
        hi == hlen - 1 ? false : loop(String.get(needle, ni + 1), ni + 1, hi + 1)
      )
    ) : (
      hi == hlen - 1 ? false : loop(nch, ni, hi + 1)
    );
    loop(String.get(needle, 0), 0, 0)

    /* let rec loop = (ni, hi) => {
      let nch = String.get(needle, ni);
      let rec inner = hi => {
        if (hi >= hlen) {
          false
        } else {
          if (String.get(haystack, hi) == nch) {
            nch >= nlen - 1 ? true : loop(ni + 1, hi + 1)
          } else {
            inner(hi + 1)
          }
        }
      };
      inner(hi)
    };
    loop(0, 0) */
  }
};


type emoji = {
  name: string,
  keywords: array(string),
  char: string,
  fitz: bool,
  category: string,
};


let loadEmojis = fileName => {
  let text = Files.readFileExn(fileName);
  let emojis = Json.parse(text);
  let force = x => switch x { |None => failwith("Force unwrapped nil") | Some(x) => x};
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
  emojis->Belt.List.keep(emoji => Fluid.App.isEmojiSupported(emoji.char));
}

let (|?>) = (x, fn) => switch x { |None => None| Some(x) => fn(x)};


let has = (text, rx) => Str.string_match(rx, text, 0);

let main = (~emojis, ~onDone, hooks) => {
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

  let%hook mouseDown = useCallback(({x, y}) => {
    let x = x /. size |> int_of_float;
    let y = y /. size |> int_of_float;
    let pos = y * row + x;
    /* switch (filtered->Belt.List.get(pos)) {
      | None => ()
      | Some({name}) =>
        setText(name)
        setSelection(0);
    } */
    if (pos < List.length(filtered)) {
      setSelection(pos);
    }
  }, filtered)

  let%hook draw = useCallback(({top, left, width, height}) => {
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
            r: 0.4,
            g: 0.4,
            b: 0.4,
            a: 0.5,
          })
        };
        Fluid.Draw.text(~fontSize, emoji.char, {x: x +. 2., y: y +. 4.});
      };
    });
  }, (text, selection));

  let selected = filtered->Belt.List.get(selection);

  <view layout={Layout.style(
    ~width=300.,
    ~height=250.,
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
      onTab={() => setSelection(selection == List.length(filtered) - 1 ? 0 : selection + 1)}
      onShiftTab={() => {
        setSelection(max(0, (selection == 0 ? List.length(filtered) : selection) - 1))
      }}
      onChange={text => {
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
          Layout.style(~paddingHorizontal=10., ~alignSelf=AlignStretch, ())
        }>
      <custom
        layout={Layout.style(~alignSelf=AlignStretch, ~height=(float_of_int(rows) *. size), ())}
        onMouseDown={mouseDown}
        onMouseDragged={mouseDown}
        draw={draw}
      />
        </view>
      ],
      ()
    )}
    {switch selected {
      | None => <view />
      | Some(emoji) =>
        <view
          layout={Layout.style(~alignSelf=AlignStretch, ~paddingBottom=10., ~paddingHorizontal=10., ())}
        >
          <view
            backgroundColor={r: 0.9, g: 0.9, b: 0.9, a: 1.}
            layout={
              Layout.style(~height=1., ~alignSelf=AlignStretch, ())
            }
          />
          {str(emoji.name)}
          {str(emoji.keywords |> Belt.List.fromArray |> String.concat(", "))}
        </view>
    }}
  </view>
};

let run = assetDir => {
  let (/+) = Filename.concat;
  let emojis = loadEmojis(assetDir /+ "emojis.json");

  Fluid.App.launch(
    ~isAccessory=true,
    () => {
    Fluid.App.setupAppMenu(
      ~title="🙃",
      ~appItems=[||],
      ~menus=[| Fluid.App.defaultEditMenu() |]
    );
    Fluid.Hotkeys.init();

    let closeWindow = ref(() => ());

    let win = Fluid.launchWindow(
      ~title="Hello Fluid",
      ~floating=true,
      ~hidden=true,
      ~onBlur=win => {
        Fluid.Window.hide(win);
      },
      <Main emojis onDone={text => {
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
      ~title="🙃",
      ~onClick=pos => {
        Fluid.Window.showAtPos(win, pos)
      }
    );

    Fluid.Hotkeys.register(~key=0x31, () => {
      print_endline("Got it!");
      Fluid.Window.showAtPos(win, Fluid.App.statusBarPos(statusBarItem));
    })->ignore;
  });

};