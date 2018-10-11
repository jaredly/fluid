
open Basic;

let awesome =
  Maker.statefulComponent(
    ~initialState= _props => "Folks",
    ~render= (props, state, setState) =>
      Builtin(
        "div",
        Js.Obj.empty(),
        [
          Builtin(
            "div",
            {"onclick": _evt => setState(state ++ "1")},
            [String(state)],
          ),
          ...props,
        ],
      ),
  );

let recursive_ = Maker.statefulComponent(
    ~initialState= ((_loop, depth)) => ("Recursion!", depth),
    ~render= ((loop, parentNum), (text, depth), setState) => {
      let rec recur = num => num > 0 ? [Custom(loop(depth - 1)), ...recur(num - 1)] : [];
      Builtin(
        "div",
        Js.Obj.empty(),
        [
          String("[depth " ++ string_of_int(depth) ++ ":" ++ string_of_int(parentNum) ++ "]"),
          Builtin(
            "div",
            {"onclick": _evt => setState((text ++ "1", depth - 1)), "style": "cursor: pointer"},
            [String(text)],
          ),
          Builtin(
            "div",
            {"style": "padding-left: 20px; border-left: 2px solid #aaa"},
            recur(depth)
          ),
        ],
      )
    }
  );
let rec recursive = props => recursive_((recursive, props));


/* let awesomeLarge = children => {
  init: () => {
    /* let state = ref("Folks"); */
    let onChange = ref(_state => ());
    WithState({
      identity: awesome,
      state: "Folks",
      props: children,
      render: (props, state) => Builtin("div", Js.Obj.empty(), [
        Builtin("div", {"onclick": evt => {
        onChange^(state ++ "1")
      }}, [String(state)]),
        ...props
      ]),
      onChange: (handler) => onChange := handler
    })
  },
  clone: (WithState({identity} as contents)) => {
    if (Obj.magic(identity) === awesome) {
      Some(WithState({
        ...Obj.magic(contents),
        props: children,
      }))
    } else {
      None
    }
  }
}; */

let first = Builtin("div", {"id": "awesome"}, [
  String("Hello"),
  Custom(awesome([
    String(">>"),
    Custom(awesome([]))
  ])),
  Builtin("div", {
    "id": "Inner",
  }, [
    String("world"),
    Custom(recursive(3))
  ])
]);

[@bs.val][@bs.scope "document"] external getElementById: string => option(domNode) = "";

switch (getElementById("root")) {
  | None => assert(false)
  | Some(node) => mount(first, node)
}
