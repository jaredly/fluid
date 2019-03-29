
module M = Flex.Layout;

Printexc.record_backtrace(true);

type pos = {x: float, y: float};
type color = {r: float, g: float, b: float, a: float};
type dims = {left: float, top: float, width: float, height: float};

let showDims = ({left,  top, width, height}) => Printf.sprintf("%f, %f - %f x %f", left, top, width, height);

let compareFns = (a, b) => switch (a, b) {
  | (None, None) => true
  | (Some(a), Some(b)) => a === b
  | _ => false
};

module C = Tracker.C;
module Tracker = Tracker.F;

module DrawTracker = Tracker(C({type arg = dims; let name = "fluid_rect_fn"}));
module StringTracker = Tracker(C({type arg = string; let name = "fluid_string_fn"}));
module UnitTracker = Tracker(C({type arg = unit; let name = "fluid_unit_fn"}));
module PosTracker = Tracker(C({type arg = pos; let name = "fluid_pos_fn"}));

type mouseHandlers('a) = {
  down: option('a),
  up: option('a),
  move: option('a),
  drag: option('a),
  rightDown: option('a),
};

let compareMouseHandlers = (a, b) =>
  compareFns(a.down, b.down) &&
  compareFns(a.up, b.up) &&
  compareFns(a.move, b.move) &&
  compareFns(a.drag, b.drag);
let trackMouseHandlers = ({down, up, move, drag, rightDown}) => ({
  down: PosTracker.maybeTrack(down),
  up: PosTracker.maybeTrack(up),
  move: PosTracker.maybeTrack(move),
  drag: PosTracker.maybeTrack(drag),
  rightDown: PosTracker.maybeTrack(rightDown),
});
let untrackMouseHandlers = ({down, up, move, drag, rightDown}) => {
  PosTracker.maybeUntrack(down);
  PosTracker.maybeUntrack(up);
  PosTracker.maybeUntrack(move);
  PosTracker.maybeUntrack(drag);
  PosTracker.maybeUntrack(rightDown);
};


type keyHandlers('unit, 'change) = {
  enter: option('unit),
  tab: option('unit),
  shiftTab: option('unit),
  change: option('change),
  escape: option('unit),
};
let compareKeyHandlers = (a, b) =>
  compareFns(a.enter, b.enter) &&
  compareFns(a.tab, b.tab) &&
  compareFns(a.shiftTab, b.shiftTab) &&
  compareFns(a.change, b.change);
let trackKeyHandlers = ({enter, tab, shiftTab, change, escape}) => ({
  enter: UnitTracker.maybeTrack(enter),
  tab: UnitTracker.maybeTrack(tab),
  shiftTab: UnitTracker.maybeTrack(shiftTab),
  change: StringTracker.maybeTrack(change),
  escape: UnitTracker.maybeTrack(escape),
});
let untrackKeyHandlers = ({enter, tab, shiftTab, change, escape}) => ({
  UnitTracker.maybeUntrack(enter);
  UnitTracker.maybeUntrack(tab);
  UnitTracker.maybeUntrack(shiftTab);
  StringTracker.maybeUntrack(change);
  UnitTracker.maybeUntrack(escape);
});

type _nativeInternal;

module ColumnBrowser = {
  module ChildrenCountTracker =
    Tracker({
      type arg = string;
      type res = int;
      let name = "fluid_column_browser_children_count";
      let once = false;
    });
  module ChildOfItemTracker =
    Tracker({
      type arg = (string, int);
      type res = string;
      let name = "fluid_column_browser_child_of_item";
      let once = false;
    });
  module IsLeafItemTracker =
    Tracker({
      type arg = string;
      type res = bool;
      let name = "fluid_column_browser_is_leaf_item";
      let once = false;
    });
  module DisplayForItemTracker =
    Tracker({
      type arg = string;
      type res = string;
      let name = "fluid_column_browser_display_for_item";
      let once = false;
    });

  type trackers = {
    childrenCount: ChildrenCountTracker.callbackId,
    childOfItem: ChildOfItemTracker.callbackId,
    isLeafItem: IsLeafItemTracker.callbackId,
    displayForItem: DisplayForItemTracker.callbackId,
  };

  type handlers = {
    childrenCount: ChildrenCountTracker.fn,
    childOfItem: ChildOfItemTracker.fn,
    isLeafItem: IsLeafItemTracker.fn,
    displayForItem: DisplayForItemTracker.fn,
  };

  let compare = (a, b) =>
    a.childrenCount === b.childrenCount &&
    a.childOfItem === b.childOfItem &&
    a.isLeafItem === b.isLeafItem &&
    a.displayForItem === b.displayForItem;

  let track = ({childrenCount, childOfItem, isLeafItem, displayForItem}): trackers => ({
    childrenCount: ChildrenCountTracker.track(childrenCount),
    childOfItem: ChildOfItemTracker.track(childOfItem),
    isLeafItem: IsLeafItemTracker.track(isLeafItem),
    displayForItem: DisplayForItemTracker.track(displayForItem),
  });
  let untrack = ({childrenCount, childOfItem, isLeafItem, displayForItem}) => ({
    ChildrenCountTracker.untrack(childrenCount);
    ChildOfItemTracker.untrack(childOfItem);
    IsLeafItemTracker.untrack(isLeafItem);
    DisplayForItemTracker.untrack(displayForItem);
  });
  external reloadColumn: (_nativeInternal, int) => unit = "fluid_column_browser_reload_column";
  external selectedCell: (_nativeInternal) => option(string) = "fluid_column_browser_selected_cell";
  external create: (~dims: dims, ~handlers: trackers) => _nativeInternal = "fluid_column_browser_create";
  let create = (~dims, ~handlers) => create(~dims, ~handlers=track(handlers));
  external update: (_nativeInternal, trackers) => unit = "fluid_column_browser_update";
  let update = (mountPoint, handlers) => update(mountPoint, track(handlers));

};


module NativeInterface = {
  type nativeInternal = _nativeInternal;
  type nativeNode = (nativeInternal, int);

  type font = {fontName: string, fontSize: float};

  external setImmediate: (unit => unit) => unit = "fluid_setImmediate";

  type invalidated = Partial(array(dims)) | Full;

  external createScrollView: (~dims: dims) => nativeInternal = "fluid_create_ScrollView";
  /* external createColumnBrowser: (~dims: dims) => nativeInternal = "fluid_create_ScrollView"; */
  external createCustom: (~dims: dims, ~drawFn: DrawTracker.callbackId, ~mouseHandlers: mouseHandlers(PosTracker.callbackId)) => nativeInternal = "fluid_create_CustomView";
  external updateCustom: (nativeInternal, DrawTracker.callbackId, mouseHandlers(PosTracker.callbackId), option(invalidated)) => unit = "fluid_update_CustomView";
  external createTextNode: (string, ~dims: dims, ~font: font, ~selectable: bool, ~handlers: keyHandlers(UnitTracker.callbackId, StringTracker.callbackId)) => nativeInternal = "fluid_create_NSTextView";
  external updateTextView: (nativeInternal, string, dims, font, keyHandlers(UnitTracker.callbackId, StringTracker.callbackId)) => unit = "fluid_set_NSTextView_textContent";

  type image;
  type imageSrc = | Preloaded(image) | Plain(string);
  external createImage: (~src: imageSrc, ~dims: dims) => nativeInternal = "fluid_create_NSImageView";
  external preloadImage: (~src: string, ~onDone: image => unit) => unit = "fluid_Image_load";

  external clearChildren: (nativeInternal) => unit = "fluid_NSView_clearChildren";
  let clearChildren = (a) => clearChildren(fst(a));
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
    | Custom(dims => unit, mouseHandlers(PosTracker.fn), invalidated_)
    | ScrollView
    | View(option(unit => unit), viewStyles)
    | Button(string, unit => unit)
    | String(string, option(font), bool, keyHandlers(UnitTracker.fn, StringTracker.fn))
    | Image(imageSrc)
    | ColumnBrowser(option(ref(option(nativeInternal))), ColumnBrowser.handlers);

  let canUpdate = (~mounted, ~mountPoint, ~newElement) => {
    switch (mounted, newElement) {
      | (ScrollView, ScrollView) => true
      | (ColumnBrowser(_, _), ColumnBrowser(_, _)) => true
      | (Custom(adrawFn, _, _), Custom(drawFn, _, _)) => true
      | (View(aPress, aStyle), View(onPress, style)) => true
      | (Button(atitle, apress), Button(btitle, bpress)) => true
      | (String(atext, afont, _, _), String(btext, bfont, _, _)) => true
      | _ => false
    }
  };

  let dims = ({Layout.LayoutTypes.layout: {width, height, top, left}}) => {left, top, width, height};

  let updateLayout = (mounted, (mountPoint, _), layout: Layout.node) => {
    switch (mounted) {
      | ScrollView
      | View(_, _)
      | Image(_)
      | ColumnBrowser(_, _)
      | Custom(_, _, _) => updateViewLoc(mountPoint, dims(layout))
      | Button(_, _) => updateButtonLoc(mountPoint, dims(layout))
      | String(_, _, _, _) => updateTextLoc(mountPoint, dims(layout))
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

      | (ColumnBrowser(aref, ahandlers), ColumnBrowser(bref, bhandlers)) =>
        if (aref !== bref || !ColumnBrowser.compare(ahandlers, bhandlers)) {
          ColumnBrowser.untrack(ahandlers);
          ColumnBrowser.update(mountPoint, bhandlers);
        }
      | (String(atext, afont, aselectable, ahandlers), String(btext, bfont, bselectable, bhandlers)) => 
        if (atext != btext || aselectable != bselectable || afont != bfont || !compareKeyHandlers(ahandlers, bhandlers)) {
          untrackKeyHandlers(ahandlers);
          /* TODO update selectable */
          updateTextView(mountPoint, btext, dims(layout), bfont, 
            trackKeyHandlers(bhandlers)
          )
        };

      | _ => ()
    }
  };

  let inflate = (element, {Layout.LayoutTypes.layout: {width, height, top, left}}) => switch element {
    | ScrollView => (createScrollView(~dims={left, top, width, height}), getNativeId())
    | ColumnBrowser(ref, handlers) =>
      let node = ColumnBrowser.create(~dims={left, top, width, height}, ~handlers);
      switch ref {
        | None => ()
        | Some(r) => r := Some(node)
      };
      (node, getNativeId())
    | Custom(drawFn, mouseHandlers, _) =>
      let native = createCustom(
        ~drawFn=DrawTracker.track(drawFn),
        ~mouseHandlers=trackMouseHandlers(mouseHandlers),
        ~dims={left, top, width, height});
      (native, getNativeId())
    | View(onPress, style) => 
      /* Printf.printf("OCaml side %f,%f %f x %f\n", top, left, width, height); */
      let native = createView(~onPress, ~pos=(top, left), ~size=(width, height), ~style);
      (native, getNativeId())
    | Button(title, onPress) =>
      let id = getNativeId();
      let native = createButton(~title, ~id, ~pos=(top, left), ~size=(width, height));
      setButtonPress(id, onPress);
      (native, id)

    | String(contents, font, selectable, handlers) =>
      let font = switch font { | None => defaultFont | Some(f) => f};
      let native = createTextNode(contents, ~dims={left, top, width, height}, ~font, ~selectable, ~handlers=trackKeyHandlers(handlers));
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

    let text =
        (
          ~layout=?,
          ~font=?,
          ~selectable=false,
          ~onChange=?,
          ~onEnter=?,
          ~onTab=?,
          ~onShiftTab=?,
          ~onEscape=?,
          ~contents,
          (),
        ) => {
      Builtin(
        String(
          contents,
          font,
          selectable,
          {
            change: onChange,
            enter: onEnter,
            tab: onTab,
            shiftTab: onShiftTab,
            escape: onEscape,
          },
        ),
        [],
        layout,
        Some(NativeInterface.measureText(contents, font)),
      );
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
    let columnBrowser = (
      ~layout=?,
      ~ref=?,
      ~childrenCount,
      ~isLeafItem,
      ~childOfItem,
      ~displayForItem,
      ()) => Builtin(ColumnBrowser(ref, {
        ColumnBrowser.childrenCount,
        isLeafItem,
        childOfItem,
        displayForItem,
      }), [], layout, None);
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
    external statusBarItem: (~title: statusTitle, ~onClick: PosTracker.callbackId, ~isVariableLength: bool) => statusBarItem = "fluid_App_statusBarItem";
    let statusBarItem = (~isVariableLength=false, ~title, ~onClick) => statusBarItem(~title, ~onClick=PosTracker.track(onClick), ~isVariableLength);
    external statusBarPos: statusBarItem => pos = "fluid_App_statusBarPos";
    external statusBarSetTitle: (statusBarItem, statusTitle) => unit = "fluid_App_setStatusBarItemTitle";

    external homeDirectory: unit => string = "fluid_App_homeDirectory";

    external triggerString: (string) => unit = "fluid_App_triggerString";

    module TimeoutTracker = Tracker({type arg = unit; let name = "fluid_timeout_cb"; let once = true; type res = unit});
    external setTimeout: (TimeoutTracker.callbackId, int) => unit = "fluid_App_setTimeout";
    let setTimeout = (fn, time) => setTimeout(TimeoutTracker.track(fn), time);

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
    module Tracker = Tracker(C({type arg = window; let name = "fluid_window"}));
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
    external resize: (window, pos) => unit = "fluid_Window_resize";
    /* external show: (window) => unit = "fluid_Window_show"; */

    external activate: (window) => unit = "fluid_Window_activate";
    external contentView: (window) => NativeInterface.nativeInternal = "fluid_Window_contentView";

    let showAtPos = (win, pos) => {
      position(win, pos);
      activate(win);
    }
  }

  let string = (~layout=?, ~font=?, contents) => Native.text(~layout?, ~font?, ~contents, ());

  let launchWindow = (
    ~title: string,
    ~id=NativeInterface.getNativeId(),
    ~pos=?,
    ~hidden=false,
    ~onBlur=(_) => (),
    ~onResize=({width, height}, window) => {
      window->Window.resize({x: width, y: height})
    },
    ~floating=false,
    root: element
  ) => {
    let win = ref(None);
    let prevSize = ref({x: 0., y: 0.});
    let window = preMount(root, layout => {
      switch (win^) {
        | None => ()
        | Some(win) =>
          let {Layout.LayoutTypes.left, top, width, height} = layout;
          let {x, y} = prevSize^;
          if (x != width || y != height) {
            prevSize := {x: width, y: height};
            onResize({left: 0., top: 0., width, height}, win)
          }
      }
    }, (~size as (width, height), onNode) => {
      let (left, top) = switch pos {
        | None => (0., 0.)
        | Some((x, y)) => (x, y -. height)
      };
      prevSize := {x: width, y: height};
      let window =
        Window.make(
          ~title,
          ~onBlur=Window.Tracker.track(onBlur),
          ~dims={left, top, width, height},
          ~isFloating=floating,
        );
      /* print_endline("made"); */
      let node = (Window.contentView(window), id);
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
    });
    win := Some(window);
    window
  };

  module Hot = {
    let initialized = ref(false);
    let ready = ref(false);

    let pendingWindows = ref([]);

    let init = (cma, fn) => {
      let cmx = Dynlink.adapt_filename(cma);
      print_endline("Hello " ++ cma);
      if (!initialized^) {
        print_endline("Init here");
        initialized := true;
        App.launch(() => {
          fn();
          ready := true;
          // (pendingWindows^)->Belt.List.forEach(fn => fn());
          // pendingWindows := [];
          let lastTime = ref(Unix.stat(cmx).st_mtime);
          // App.setTimeout(() => {
          //   try {
          //     Dynlink.loadfile_private(cmx);
          //   } {
          //     | Dynlink.Error(error) => print_endline(Dynlink.error_message(error))
          //   };
          // }, 1000);
          let check = () => {
            try {
              let {Unix.st_mtime} = Unix.stat(cmx);
              print_endline("Check");
              if (st_mtime > lastTime^) {
                print_endline("Reloading " ++ cmx);
                lastTime := st_mtime;
                Dynlink.loadfile_private(cmx);
              }
            } {
              | _ => print_endline("Didn't exist")
            }
          };
          let rec loop = () => {
            App.setTimeout(() => {
              check();
              loop();
            }, 1000)
          };
          loop()
        });
      } else {
        print_endline("Called from reloaded");
      }
    };

    type savedWindow = {
      nativeId: int,
      title: string,
      onBlur: Window.window => unit,
      onResize: (dims, Window.window) => unit,
      window: Window.window,
    };

    let windows = Hashtbl.create(10);

    let launchWindow = (
      ~title: string,
      ~id=title,
      ~pos=?,
      ~hidden=false,
      ~onBlur=(_) => (),
      ~onResize=({width, height}, window) => {
        window->Window.resize({x: width, y: height})
      },
      ~floating=false,
      root: element
    ) => {
      // if (!initialized^) {
      //   failwith("Must call `Fluid.Hot.init()` before `Fluid.Hot.launchWindow()`");
      // };
      print_endline("Launching " ++ id);
      let doit = () => {
        if (windows->Hashtbl.mem(id)) {
          print_endline(" > it's a reload");
          let {nativeId, title, window} = windows->Hashtbl.find(id);
          // Window.setWindowTitle
          windows->Hashtbl.replace(id, {nativeId, title, onBlur, onResize, window})
          // TOOD remove the old one
          NativeInterface.clearChildren((window->Window.contentView, nativeId));
          mount(root, (window->Window.contentView, nativeId))
          // TODO remount the root here
        } else {
          let window = launchWindow(
            ~title,
            ~pos=?pos,
            ~hidden,
            ~onBlur=window => {
              let {onBlur} = windows->Hashtbl.find(id);
              onBlur(window)
            },
            ~onResize=(dims, window) => {
              let {onResize} = windows->Hashtbl.find(id);
              onResize(dims, window)
            },
            ~floating,
            root
          );
          windows->Hashtbl.replace(id, {title, onBlur, onResize, window, nativeId: NativeInterface.getNativeId()});
        }
      };
      if (ready^) {
        doit();
      } else {
        pendingWindows := [doit, ...pendingWindows^];
      }
    };
  };
}
