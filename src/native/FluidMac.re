
module M = Flex.Layout;

Printexc.record_backtrace(true);

type color = {r: float, g: float, b: float, a: float};
type dims = {left: float, top: float, width: float, height: float};

module NativeInterface = {
  type nativeInternal;
  type nativeNode = (nativeInternal, int);

  type font = {fontName: string, fontSize: float};

  external setImmediate: (unit => unit) => unit = "fluid_setImmediate";

  external createScrollView: (~dims: dims) => nativeInternal = "fluid_create_ScrollView";
  external createCustom: (~dims: dims, ~drawFn: unit => unit) => nativeInternal = "fluid_create_CustomView";
  external updateCustom: (nativeInternal, unit => unit) => unit = "fluid_update_CustomView";
  external createTextNode: (string, ~dims: dims, ~font: font, ~onChange: option(string => unit)) => nativeInternal = "fluid_create_NSTextView";
  external updateTextView: (nativeInternal, string, dims, font, option(string => unit)) => unit = "fluid_set_NSTextView_textContent";
  /* [@bs.get] external parentNode: nativeNode => nativeNode = "fluid_"; */

  type image;
  type imageSrc = | Preloaded(image) | Plain(string);
  external createImage: (~src: imageSrc, ~dims: dims) => nativeInternal = "fluid_create_NSImageView";
  external preloadImage: (~src: string, ~onDone: image => unit) => unit = "fluid_Image_load";

  external appendChild: (nativeInternal, nativeInternal) => unit = "fluid_NSView_appendChild";
  let appendChild = (a, b) => appendChild(fst(a), fst(b));
  /* external insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit = "fluid_NSView_insertBefore"; */
  external removeChild: (nativeInternal, nativeInternal) => unit = "fluid_NSView_removeChild";
  let removeChild = (a, b) => removeChild(fst(a), fst(b));

  external replaceWith: (nativeInternal, nativeInternal) => unit = "fluid_NSView_replaceWith";
  let replaceWith = (a, b) => replaceWith(fst(a), fst(b));

  type viewStyles = {backgroundColor: option(color)};

  let lastId = ref(0);

  let getNativeId = () => {
    lastId := lastId^ + 1;
    lastId^
  };

  external createNullNode: unit => nativeInternal = "fluid_create_NullNode";
  let createNullNode = () => (createNullNode(), getNativeId());

  let appendAfter = (one, two) => failwith("appendAfter not impl");

  external createView: (
    ~onPress: option(unit => unit),
    ~pos: (float, float),
    ~size: (float, float),
    ~style: viewStyles,
  ) => nativeInternal = "fluid_create_NSView";
  external createButton: (
    ~title: string,
    ~id: int,
    /* ~onPress: unit => unit, */
    ~pos: (float, float),
    ~size: (float, float)
  ) => nativeInternal = "fluid_create_NSButton";

  let callbacks = Hashtbl.create(100);

  let onPress = id => {
    switch (Hashtbl.find(callbacks, id)) {
      | exception Not_found =>
        Printf.printf("Trying to press button %d but no callback registered\n", id)
      | cb => 
      cb()
    }
  };

  Callback.register("fluid_button_press", onPress);

  /* let registerButtonPress = (fn) => {
    callbacks -> Hashtbl.replace(lastId^, fn);
  }; */
  let setButtonPress = (id, fn) => {
    callbacks -> Hashtbl.replace(id, fn)
  };
  let removeButtonPress = (id) => {
    callbacks -> Hashtbl.remove(id);
  };

  external updateButton: (nativeInternal, string) => unit = "fluid_update_NSButton";
  external updateView: (nativeInternal, option(unit => unit), viewStyles) => unit = "fluid_update_NSView";

  external updateViewLoc: (nativeInternal, dims) => unit = "fluid_update_NSView_loc";
  external updateButtonLoc: (nativeInternal, dims) => unit = "fluid_update_NSButton_loc";
  external updateTextLoc: (nativeInternal, dims) => unit = "fluid_update_Text_loc";

  /* external startApp: (~title: string, ~size: (float, float), ~floating: bool, nativeInternal => unit) => unit = "fluid_startApp"; */

  external measureText: (~text: string, ~font: string, ~fontSize: float, ~maxWidth: option(float)) => (float, float) = "fluid_measureText";

  let measureCache = Hashtbl.create(100);

  let defaultFont = {fontName: "Lucida Grande", fontSize: 12.};
  let updateTextView = (node, text, dims, font, onChange) => {
    let font = switch font { | None => defaultFont | Some(f) => f};
    updateTextView(node, text, dims, font, onChange);
  };
  let measureText = (text, font, _, width, widthMode, _, _) => {
    let {fontName, fontSize} = switch font { | None => defaultFont | Some(f) => f};
    let key = (fontName, fontSize, text, width, widthMode);
    switch (Hashtbl.find(measureCache, key)) {
      | exception Not_found => 
        let maxWidth = switch widthMode {
          | Layout.LayoutTypes.Exactly | AtMost => Some(width)
          | _ => None
        };

        let (width, height) = measureText(~text, ~font=fontName, ~fontSize, ~maxWidth);
        /* Printf.printf("Text: %s -- %f x %f", text, width, height); */
        let res = {Layout.LayoutTypes.width, height};
        Hashtbl.replace(measureCache, key, res);
        res
    | x => x
    }
  };

  type element =
    | Custom(unit => unit)
    | ScrollView
    | View(option(unit => unit), viewStyles)
    | Button(string, unit => unit)
    | String(string, option(font), option(string => unit))
    | Image(imageSrc);

  let canUpdate = (~mounted, ~mountPoint, ~newElement) => {
    switch (mounted, newElement) {
      | (Custom(adrawFn), Custom(drawFn)) => true

      | (View(aPress, aStyle), View(onPress, style)) => 
        true

      | (Button(atitle, apress), Button(btitle, bpress)) =>
        true

      | (String(atext, afont, _), String(btext, bfont, _)) => 
        true

      | _ => false
    }
  };

  let dims = ({Layout.LayoutTypes.layout: {width, height, top, left}}) => {left, top, width, height};

  let updateLayout = (mounted, (mountPoint, _), layout: Layout.node) => {
    switch (mounted) {
      | ScrollView
      | View(_, _)
      | Image(_)
      | Custom(_) => updateViewLoc(mountPoint, dims(layout))
      | Button(_, _) => updateButtonLoc(mountPoint, dims(layout))
      | String(_, _, _) => updateTextLoc(mountPoint, dims(layout))
    }
  };

  let update = (mounted, (mountPoint, id), newElement, layout) => {
    switch (mounted, newElement) {
      | (View(aPress, aStyle), View(onPress, style)) => 
        if (aPress != onPress || aStyle != style) {
          updateView(mountPoint, onPress, style)
        };

      | (Button(atitle, apress), Button(btitle, bpress)) =>
        if (atitle != btitle || apress !== bpress) {
          updateButton(mountPoint, btitle);
          setButtonPress(id, bpress);
        };

      | (Custom(a), Custom(draw)) => updateCustom(mountPoint, draw)

      | (String(atext, afont, aonChange), String(btext, bfont, bonChange)) => 
        if (atext != btext || afont != bfont || aonChange !== bonChange) {
          updateTextView(mountPoint, btext, dims(layout), bfont, bonChange)
        };

      | _ => ()
    }
  };

  /* let createTextNode = (text, {Layout.LayoutTypes.layout: {top, left, width, height}}, font) => {
    let font = switch font { | None => defaultFont | Some(f) => f};
    createTextNode(text, ~pos=(top, left), ~size=(width, height), ~font);
  } */

  let inflate = (element, {Layout.LayoutTypes.layout: {width, height, top, left}}) => switch element {
    | ScrollView => (createScrollView(~dims={left, top, width, height}), getNativeId())
    | Custom(drawFn) =>
      let native = createCustom(~drawFn, ~dims={left, top, width, height});
      (native, getNativeId())
    | View(onPress, style) => 
      Printf.printf("OCaml side %f,%f %f x %f\n", top, left, width, height);
      let native = createView(~onPress, ~pos=(top, left), ~size=(width, height), ~style);
      (native, getNativeId())
    | Button(title, onPress) =>
      let id = getNativeId();
      let native = createButton(~title, ~id, ~pos=(top, left), ~size=(width, height));
      setButtonPress(id, onPress);
      (native, id)

    | String(contents, font, onChange) =>
      let font = switch font { | None => defaultFont | Some(f) => f};
      let native = createTextNode(contents, ~dims={left, top, width, height}, ~font, ~onChange);
      (native, getNativeId())

    | Image(src) =>
      let native = createImage(~src, ~dims={left, top, width, height});
      (native, getNativeId())
  }
};

module Fluid = {
  module NativeInterface = NativeInterface;

  include FluidMaker.F(NativeInterface);

  module Native = {
    open NativeInterface;
    let view = (~onPress=?, ~children=[], ~layout=?, ~backgroundColor=?, ()) => 
    Builtin(
      View(onPress, {backgroundColor: backgroundColor}),
      children,
      layout,
      None
    )
      ;

    let image = (~src, ~layout=?, ()) =>
      Builtin(Image(src), [], layout, None);

    let button = (~onPress, ~title, ~layout=?, ()) => 
    Builtin(
      Button(title, onPress),
      [],
      layout,
      Some((a, b, c, d, e) => {
        let {Layout.LayoutTypes.width, height}: Layout.LayoutTypes.dimensions = NativeInterface.measureText(title, None, a, b, c, d, e);
        {Layout.LayoutTypes.width: width +. 20., height: height +. 10.}
      })
    );

    let text = (~layout=?, ~font=?, ~onChange=?, ~contents, ()) => {
      Builtin(
        String(contents, font, onChange),
        [],
        layout,
        Some(NativeInterface.measureText(contents, font))
      )
    };

    let custom = (~layout=?, ~draw, ()) => Builtin(Custom(draw), [], layout, None);

    let scrollView = (~layout=?, ~children, ()) => Builtin(ScrollView, children, layout, None);

  }

  module App = {
    external launch: (~isAccessory: bool, unit => unit) => unit = "fluid_App_launch";
    let launch = (~isAccessory=false, cb) => launch(~isAccessory, cb);
    external statusBarItem: (~title: string, ~onClick: (((float, float)) => unit)) => unit = "fluid_App_statusBarItem";

    type menuItem;
    type menuAction =
    | Call(unit => unit)
    | Selector(string);
    external menuItem:
      (~title: string, ~action: menuAction, ~shortcut: string) => menuItem =
      "fluid_App_menuItem";
    external separatorItem: unit => menuItem = "fluid_App_separatorItem";
    external menu: (~title: string, ~items: array(menuItem)) => menuItem =
      "fluid_App_menu";
    external setupAppMenu:
      (~title: string, ~appItems: array(menuItem), ~menus: array(menuItem)) =>
      unit =
      "fluid_App_setupAppMenu";

    let defaultEditMenu = () =>
      menu(
        ~title="Edit",
        ~items=[|
          menuItem(~title="Copy", ~action=Selector("copy:"), ~shortcut="c"),
          menuItem(~title="Paste", ~action=Selector("paste:"), ~shortcut="v"),
          menuItem(~title="Cut", ~action=Selector("cut:"), ~shortcut="x"),
          menuItem(~title="Select All", ~action=Selector("selectAll:"), ~shortcut="a"),
        |]
      );
  };

  module Window = {
    type window;
    external make: (~title: string, ~onBlur: option(window => unit), ~dims: dims, ~isFloating: bool) => window = "fluid_Window_make";
    external center: (window) => unit = "fluid_Window_center";
    external close: (window) => unit = "fluid_Window_close";
    external activate: (window) => unit = "fluid_Window_activate";
    external contentView: (window) => NativeInterface.nativeInternal = "fluid_Window_contentView";
  }

  let string = (~layout=?, ~font=?, contents) => Native.text(~layout?, ~font?, ~contents, ());

  let launchWindow = (~title: string, ~pos=?, ~onBlur=?, ~floating=false, root: element) => {
    preMount(root, (~size as (width, height), onNode) => {
      let (left, top) = switch pos {
        | None => (0., 0.)
        | Some((x, y)) => (x, y -. height)
      };
      let window = Window.make(~title, ~onBlur, ~dims={left, top, width, height}, ~isFloating=floating);
      let node = (Window.contentView(window), NativeInterface.getNativeId());
      onNode(node);
      /* if (!floating) {
        App.setupMenu(~title);
      } */
      if (pos == None) {
        Window.center(window);
      };
      Window.activate(window);
    })
  };
}
