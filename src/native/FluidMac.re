
module M = Flex.Layout;

Printexc.record_backtrace(true);

type pos = {x: float, y: float};
type color = {r: float, g: float, b: float, a: float};
type dims = {left: float, top: float, width: float, height: float};

let showDims = ({left,  top, width, height}) => Printf.sprintf("%f, %f - %f x %f", left, top, width, height);

module Tracker = (C: {type arg;let name: string}): {
  type callbackId;
  type arg = C.arg;
  type fn = arg => unit;
  let track: (arg => unit) => callbackId;
  let maybeTrack: option(arg => unit) => option(callbackId);
  let untrack: fn => unit;
  let maybeUntrack: option(fn) => unit;
  let size: unit => int;
} => {
  type callbackId = int;
  type arg = C.arg;
  type fn = C.arg => unit;

  let fns: Hashtbl.t(callbackId, C.arg => unit) = Hashtbl.create(100);
  let ids = ref([]);

  /* module Id = Belt.Id.MakeHashable({
    type t = fn;
    let hash = Hashtbl.hash;
    let eq = (===);
  });
  let ids: Belt.HashMap.t(fn, callbackId, Id.identity) = Belt.HashMap.make(~hintSize=1000, ~id=(
    module Id:Belt.Id.Hashable
    with type t = fn
    and type identity = Id.identity
  )); */


  let cur = ref(0);
  let next = () => {cur := cur^ + 1; cur^};

  let track: fn => callbackId = fn => {
    /* print_endline("Track"); */
    switch (Belt.List.getAssoc(ids^, (fn), (===))) {
      | None =>
        let id = next();
        Hashtbl.replace(fns, id, fn);
        ids := [(fn, id), ...ids^];
        /* Belt.HashMap.set(ids, fn, id); */
        id
      | Some(id) => id
    }
  };
  let maybeTrack: (option(C.arg => unit)) => option(callbackId) = fn => switch fn {
    | None => None
    | Some(fn) => Some(track(fn))
  };

  let untrack = fn => {
    /* print_endline("Untrack"); */
    switch (Belt.List.getAssoc(ids^, (fn), (===))) {
      | None => print_endline("> but not there")
      | Some(id) =>
        ids := ids^ ->Belt.List.keep(a => fst(a) !== fn);
        /* Belt.HashMap.remove(ids, fn); */
        Hashtbl.remove(fns, id)
    };
  };
  let maybeUntrack: (option(C.arg => unit)) => unit = fn => switch fn {
    | None => ()
    | Some(fn) => untrack(fn)
  };

  let size = () => Hashtbl.length(fns);

  let call = (id: callbackId, arg: C.arg): unit => switch (Hashtbl.find(fns, id)) {
    | exception Not_found =>
      print_endline("Failed to find callback! " ++ string_of_int(id))
    | fn => fn(arg)
  };
  Callback.register(C.name, call);
};


let compareFns = (a, b) => switch (a, b) {
  | (None, None) => true
  | (Some(a), Some(b)) => a === b
  | _ => false
};


module DrawTracker = Tracker({type arg = dims; let name = "fluid_rect_fn"});
module StringTracker = Tracker({type arg = string; let name = "fluid_string_fn"});
module UnitTracker = Tracker({type arg = unit; let name = "fluid_unit_fn"});
module PosTracker = Tracker({type arg = pos; let name = "fluid_pos_fn"});

type mouseHandlers = {
  down: option(PosTracker.fn),
  up: option(PosTracker.fn),
  move: option(PosTracker.fn),
  drag: option(PosTracker.fn),
  rightDown: option(PosTracker.fn),
};
type mouseTrackers = {
  down_: option(PosTracker.callbackId),
  up_: option(PosTracker.callbackId),
  move_: option(PosTracker.callbackId),
  drag_: option(PosTracker.callbackId),
  rightDown_: option(PosTracker.callbackId),
};
let compareMouseHandlers = (a, b) =>
  compareFns(a.down, b.down) &&
  compareFns(a.up, b.up) &&
  compareFns(a.move, b.move) &&
  compareFns(a.drag, b.drag);
let trackMouseHandlers = ({down, up, move, drag, rightDown}) => ({
  down_: PosTracker.maybeTrack(down),
  up_: PosTracker.maybeTrack(up),
  move_: PosTracker.maybeTrack(move),
  drag_: PosTracker.maybeTrack(drag),
  rightDown_: PosTracker.maybeTrack(rightDown),
});
let untrackMouseHandlers = ({down, up, move, drag, rightDown}) => {
  PosTracker.maybeUntrack(down);
  PosTracker.maybeUntrack(up);
  PosTracker.maybeUntrack(move);
  PosTracker.maybeUntrack(drag);
  PosTracker.maybeUntrack(rightDown);
};


type keyHandlers = {
  enter: option(UnitTracker.fn),
  tab: option(UnitTracker.fn),
  shiftTab: option(UnitTracker.fn),
  change: option(StringTracker.fn),
  escape: option(UnitTracker.fn),
};

let compareKeyHandlers = (a, b) =>
  compareFns(a.enter, b.enter) &&
  compareFns(a.tab, b.tab) &&
  compareFns(a.shiftTab, b.shiftTab) &&
  compareFns(a.change, b.change);

type keyTrackers = {
  enter_: option(UnitTracker.callbackId),
  tab_: option(UnitTracker.callbackId),
  shiftTab_: option(UnitTracker.callbackId),
  change_: option(StringTracker.callbackId),
  escape_: option(UnitTracker.callbackId),
};
let trackKeyHandlers = ({enter, tab, shiftTab, change, escape}) => ({
  enter_: UnitTracker.maybeTrack(enter),
  tab_: UnitTracker.maybeTrack(tab),
  shiftTab_: UnitTracker.maybeTrack(shiftTab),
  change_: StringTracker.maybeTrack(change),
  escape_: UnitTracker.maybeTrack(escape),
});
let untrackKeyHandlers = ({enter, tab, shiftTab, change, escape}) => ({
  UnitTracker.maybeUntrack(enter);
  UnitTracker.maybeUntrack(tab);
  UnitTracker.maybeUntrack(shiftTab);
  StringTracker.maybeUntrack(change);
  UnitTracker.maybeUntrack(escape);
});

module NativeInterface = {
  type nativeInternal;
  type nativeNode = (nativeInternal, int);

  type font = {fontName: string, fontSize: float};

  external setImmediate: (unit => unit) => unit = "fluid_setImmediate";

  type invalidated = Partial(array(dims)) | Full;

  external createScrollView: (~dims: dims) => nativeInternal = "fluid_create_ScrollView";
  external createCustom: (~dims: dims, ~drawFn: DrawTracker.callbackId, ~mouseHandlers: mouseTrackers) => nativeInternal = "fluid_create_CustomView";
  external updateCustom: (nativeInternal, DrawTracker.callbackId, mouseTrackers, option(invalidated)) => unit = "fluid_update_CustomView";
  external createTextNode: (string, ~dims: dims, ~font: font, ~handlers: keyTrackers) => nativeInternal = "fluid_create_NSTextView";
  external updateTextView: (nativeInternal, string, dims, font, keyTrackers) => unit = "fluid_set_NSTextView_textContent";

  type image;
  type imageSrc = | Preloaded(image) | Plain(string);
  external createImage: (~src: imageSrc, ~dims: dims) => nativeInternal = "fluid_create_NSImageView";
  external preloadImage: (~src: string, ~onDone: image => unit) => unit = "fluid_Image_load";

  external appendChild: (nativeInternal, nativeInternal) => unit = "fluid_NSView_appendChild";
  let appendChild = (a, b) => appendChild(fst(a), fst(b));
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

  type invalidated_ = [`Full | `CompareFn | `Partial(list(dims)) | `None];

  type element =
    | Custom(dims => unit, mouseHandlers, invalidated_)
    | ScrollView
    | View(option(unit => unit), viewStyles)
    | Button(string, unit => unit)
    | String(string, option(font), keyHandlers)
    | Image(imageSrc);

  let canUpdate = (~mounted, ~mountPoint, ~newElement) => {
    switch (mounted, newElement) {
      | (ScrollView, ScrollView) => true
      | (Custom(adrawFn, _, _), Custom(drawFn, _, _)) => true
      | (View(aPress, aStyle), View(onPress, style)) => true
      | (Button(atitle, apress), Button(btitle, bpress)) => true
      | (String(atext, afont, _), String(btext, bfont, _)) => true
      | _ => false
    }
  };

  let dims = ({Layout.LayoutTypes.layout: {width, height, top, left}}) => {left, top, width, height};

  let updateLayout = (mounted, (mountPoint, _), layout: Layout.node) => {
    switch (mounted) {
      | ScrollView
      | View(_, _)
      | Image(_)
      | Custom(_, _, _) => updateViewLoc(mountPoint, dims(layout))
      | Button(_, _) => updateButtonLoc(mountPoint, dims(layout))
      | String(_, _, _) => updateTextLoc(mountPoint, dims(layout))
    }
  };


  let update = (mounted, (mountPoint, id), newElement, layout) => {
    switch (mounted, newElement) {
      | (ScrollView, ScrollView) => ()
      | (View(aPress, aStyle), View(onPress, style)) => 
        if (aPress != onPress || aStyle != style) {
          updateView(mountPoint, onPress, style)
        };

      | (Button(atitle, apress), Button(btitle, bpress)) =>
        if (atitle != btitle || apress !== bpress) {
          updateButton(mountPoint, btitle);
          setButtonPress(id, bpress);
        };

      | (Custom(a, aMouse, inv), Custom(draw, bMouse, binv)) =>
        if (a !== draw || inv != binv) {
          untrackMouseHandlers(aMouse);
          DrawTracker.untrack(a);
          updateCustom(mountPoint, DrawTracker.track(draw), trackMouseHandlers(bMouse), switch binv {
            | `None => None
            | `Partial(dims) => Some(Partial(Belt.List.toArray(dims)))
            | `Full => Some(Full)
            | `CompareFn => a !== draw ? Some(Full) : None
          });
          /* print_endline("updated\n"); */
        }

      | (String(atext, afont, ahandlers), String(btext, bfont, bhandlers)) => 
        if (atext != btext || afont != bfont || !compareKeyHandlers(ahandlers, bhandlers)) {
          untrackKeyHandlers(ahandlers);
          updateTextView(mountPoint, btext, dims(layout), bfont, 
            trackKeyHandlers(bhandlers)
          )
        };

      | _ => ()
    }
  };

  let inflate = (element, {Layout.LayoutTypes.layout: {width, height, top, left}}) => switch element {
    | ScrollView => (createScrollView(~dims={left, top, width, height}), getNativeId())
    | Custom(drawFn, mouseHandlers, _) =>
      let native = createCustom(
        ~drawFn=DrawTracker.track(drawFn),
        ~mouseHandlers=trackMouseHandlers(mouseHandlers),
        ~dims={left, top, width, height});
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

    | String(contents, font, handlers) =>
      let font = switch font { | None => defaultFont | Some(f) => f};
      let native = createTextNode(contents, ~dims={left, top, width, height}, ~font, ~handlers=trackKeyHandlers(handlers));
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

    let text = (~layout=?, ~font=?, ~onChange=?, ~onEnter=?, ~onTab=?, ~onShiftTab=?, ~onEscape=?, ~contents, ()) => {
      Builtin(
        String(contents, font, {change: onChange, enter: onEnter, tab: onTab, shiftTab: onShiftTab, escape: onEscape}),
        [],
        layout,
        Some(NativeInterface.measureText(contents, font))
      )
    };

    let custom = (
      ~invalidated=`CompareFn,
      ~layout=?, ~onMouseDown=?, ~onRightMouseDown=?, ~onMouseUp=?, ~onMouseMove=?, ~onMouseDragged=?, ~draw, ()) => Builtin(Custom(
        draw, {
          down: onMouseDown,
          move: onMouseMove,
          up: onMouseUp,
          drag: onMouseDragged,
          rightDown: onRightMouseDown,
        }, invalidated), [], layout, None);

    let scrollView = (~layout=?, ~children, ()) => Builtin(ScrollView, children, layout, None);
  }

  module Hotkeys = {
    external init: unit => unit = "fluid_Hotkeys_init";
    external register: (~id: int, ~key: int) => unit = "fluid_Hotkeys_register";
    let cur = ref(0);
    let next = () => {cur := cur^ + 1; cur^};
    let fns = Hashtbl.create(100);
    Callback.register("fluid_hotkeys_triggered", id => {
      switch (Hashtbl.find(fns, id)) {
        | exception Not_found => print_endline("Got hotkey without registered handler " ++ string_of_int(id))
        | fn => fn()
      }
    });
    let register = (~key: int, fn) => {
      let id = next();
      Hashtbl.replace(fns, id, fn);
      register(~id, ~key);
      id
    };
  }

  module App = {
    external launch: (~isAccessory: bool, unit => unit) => unit = "fluid_App_launch";
    external deactivate: unit => unit = "fluid_App_deactivate";
    external hide: unit => unit = "fluid_App_hide";
    let launch = (~isAccessory=false, cb) => launch(~isAccessory, cb);
    type statusBarItem;
    type statusTitle = String(string) | Image(NativeInterface.image);
    external statusBarItem: (~title: statusTitle, ~onClick: PosTracker.callbackId) => statusBarItem = "fluid_App_statusBarItem";
    let statusBarItem = (~title, ~onClick) => statusBarItem(~title, ~onClick=PosTracker.track(onClick));
    external statusBarPos: statusBarItem => pos = "fluid_App_statusBarPos";

    external homeDirectory: unit => string = "fluid_App_homeDirectory";

    external triggerString: (string) => unit = "fluid_App_triggerString";
    external setTimeout: (unit => unit, int) => unit = "fluid_App_setTimeout";

    type menu;
    type menuItem;
    type menuAction =
    | Call(unit => unit)
    | Selector(string);
    module B = {
      type boundMenuAction =
      | Call(UnitTracker.callbackId)
      | Selector(string);
    };
    let bindMenuAction = fun
    | Call(fn) => B.Call(UnitTracker.track(fn))
    | Selector(s) => B.Selector(s);
    external menuItem:
      (~title: string, ~action: B.boundMenuAction, ~shortcut: string) => menuItem =
      "fluid_App_menuItem";
    let menuItem = (~title, ~action, ~shortcut) => menuItem(~title, ~shortcut, ~action=bindMenuAction(action));
    external separatorItem: unit => menuItem = "fluid_App_separatorItem";
    external menu: (~title: string, ~items: array(menuItem)) => menu =
      "fluid_App_menu";
    external submenu: (~title: string, ~menu: menu) => menuItem = "fluid_App_menu_item";
    let submenu = (~title, ~items) => submenu(~title, ~menu=menu(~title, ~items));
    external setupAppMenu:
      (~title: string, ~appItems: array(menuItem), ~menus: array(menuItem)) =>
      unit =
      "fluid_App_setupAppMenu";

    let defaultEditMenu = () =>
      submenu(
        ~title="Edit",
        ~items=[|
          menuItem(~title="Copy", ~action=Selector("copy:"), ~shortcut="c"),
          menuItem(~title="Paste", ~action=Selector("paste:"), ~shortcut="v"),
          menuItem(~title="Cut", ~action=Selector("cut:"), ~shortcut="x"),
          menuItem(~title="Select All", ~action=Selector("selectAll:"), ~shortcut="a"),
        |]
      );
  };

  module Draw = {
    external text: (string, pos, ~fontName: string, ~fontSize: float) => unit = "fluid_Draw_text";
    let text = (~fontName="", ~fontSize=10., string, pos) => {
      text(string, pos, ~fontName, ~fontSize);
    };
    external fillRect: (dims, color) => unit = "fluid_Draw_fillRect";
    external rect: (dims, color) => unit = "fluid_Draw_rect";
  }

  module Window = {
    type window;
    module Tracker = Tracker({type arg = window; let name = "fluid_window"});
    external make: (
      ~title: string,
      ~onBlur: Tracker.callbackId,
      ~dims: dims,
      ~isFloating: bool
    ) => window = "fluid_Window_make";
    external center: (window) => unit = "fluid_Window_center";
    external close: (window) => unit = "fluid_Window_close";
    external hide: (window) => unit = "fluid_Window_hide";

    external position: (window, pos) => unit = "fluid_Window_position";
    /* external show: (window) => unit = "fluid_Window_show"; */

    external activate: (window) => unit = "fluid_Window_activate";
    external contentView: (window) => NativeInterface.nativeInternal = "fluid_Window_contentView";

    let showAtPos = (win, pos) => {
      position(win, pos);
      activate(win);
    }
  }

  let string = (~layout=?, ~font=?, contents) => Native.text(~layout?, ~font?, ~contents, ());

  let launchWindow = (~title: string, ~pos=?, ~hidden=false, ~onBlur=(_) => (), ~floating=false, root: element) => {
    preMount(root, (~size as (width, height), onNode) => {
      let (left, top) = switch pos {
        | None => (0., 0.)
        | Some((x, y)) => (x, y -. height)
      };
      /* print_endline("making"); */
      let window =
        Window.make(
          ~title,
          ~onBlur=Window.Tracker.track(onBlur),
          ~dims={left, top, width, height},
          ~isFloating=floating,
        );
      /* print_endline("made"); */
      let node = (Window.contentView(window), NativeInterface.getNativeId());
      onNode(node);
      /* if (!floating) {
        App.setupMenu(~title);
      } */
      if (!hidden) {
        if (pos == None) {
          Window.center(window);
        };
        Window.activate(window);
      };
      window
    })
  };
}
