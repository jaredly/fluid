
open Basic;

let rec awesome = children => {
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
};

let first = Builtin("div", {"id": "awesome"}, [
  String("Hello"),
  Custom(awesome([
    String(">>"),
    Custom(awesome([]))
  ])),
  Builtin("div", {
    "id": "Inner",
    "onclick": (evt) => {
      Js.log2("Clicked!", evt)
    }
  }, [
    String("world")
  ])
]);

[@bs.val][@bs.scope "document"] external getElementById: string => option(domNode) = "";

switch (getElementById("root")) {
  | None => assert(false)
  | Some(node) => mount(first, node)
}
