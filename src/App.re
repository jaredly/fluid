
open Basic;

/*
Thoughts about props...

seems like with bs.deriving abstract you could transform

WARNING: I wouldn't be able to use the stock one, because it only adds the final
unit argument if there's an optional argument, and I need to not care about whether
there is one or not.

<Awesome a=2 b=3 />

/* Assuming there's a default on `children=[]` */
Awesome.make(Awesome.props(~a=2, ~b=3, ()));

<Awesome a=2 b=3>
  <div />
  <input />
  <Bomb />
</Awesome>

Awesome.make(Awesome.props(~a=2, ~b=3, ~children=[<div />, <input />, <Bomb />], ()))

 */

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
