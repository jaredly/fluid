
open Basic;

let first = Builtin("div", {"id": "awesome"}, [
  String("Hello"),
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
