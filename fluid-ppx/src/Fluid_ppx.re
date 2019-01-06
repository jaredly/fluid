
/***
 * https://ocsigen.org/lwt/dev/api/Ppx_lwt
 * https://github.com/zepalmer/ocaml-monadic
 */
let fail = (loc, txt) => raise(Location.Error(Location.error(~loc, txt)));

let rec process_bindings = (bindings, ident) =>
  Parsetree.(
    switch bindings {
    | [] => assert false
    | [binding] => (binding.pvb_pat, binding.pvb_expr)
    | [binding, ...rest] =>
      let (pattern, expr) = process_bindings(rest, ident);
      (
        Ast_helper.Pat.tuple(~loc=binding.pvb_loc, [binding.pvb_pat, pattern]),
          Ast_helper.Exp.apply(
            ~loc=binding.pvb_loc,
            Ast_helper.Exp.ident(~loc=binding.pvb_loc, Location.mkloc(Longident.Ldot(ident, "and_"), binding.pvb_loc)),
            [
              ("", binding.pvb_expr),
              ("", expr)
            ]
          )
      )
    }
  );

let parseLongident = txt => {
  let parts = Str.split(Str.regexp_string("."), txt);
  let rec loop = (current, parts) => switch (current, parts) {
    | (None, []) => assert(false)
    | (Some(c), []) => c
    | (None, [one, ...more]) => loop(Some(Longident.Lident(one)), more)
    | (Some(c), [one, ...more]) => loop(Some(Longident.Ldot(c, one)), more)
  };
  loop(None, parts)
};

let isLowerCase = str => String.lowercase(str) == str;

open Ast_helper;
open Longident;

let unCapitalize = text => {
  let f = String.lowercase(String.sub(text, 0, 1));
  let rest = String.sub(text, 1, String.length(text) - 1);
  f ++ rest
};

let mapper = _argv =>
  Parsetree.{
    ...Ast_mapper.default_mapper,
    expr: (mapper, expr) => {
      switch expr {
        | {
          pexp_attributes: [({txt: "JSX"}, Parsetree.PStr([]))],
          pexp_desc: Pexp_apply(target, args)
        } => {
          let args = args |. Belt.List.map(((a, b)) => (a, mapper.expr(mapper, b)));
          switch (target.pexp_desc) {
            | Pexp_ident({txt: Longident.Lident(name), loc}) when isLowerCase(String.sub(name, 0, 1)) =>
              let rec loop = args => switch args {
                | [] => assert(false)
                | [("children", arg), ...rest] => (arg, rest)
                | [arg, ...rest] => let (children, args) = loop(rest);
                  (children, [arg, ...args])
              };
              let (children, props) = loop(args);
              Exp.construct(
                Location.mkloc(Ldot(Lident("Fluid"), "Builtin"), loc),
                Some(Exp.tuple([
                  Exp.constant(Const_string(name, None)),
                  Ast_helper.Exp.apply(
                    Exp.ident(Location.mkloc(Ldot(Lident("Fluid"), "domProps"), loc)),
                    props
                  ),
                  /* TODO auto-up strings */
                  children
                ]))
              )
            | Pexp_ident({txt: Ldot(contents, "createElement"), loc}) =>
              let rec loop = args => switch args {
                | [] => (None, args)
                | [("children", {pexp_desc: Pexp_construct({txt: Lident("[]")}, None)}), ...rest] => (None, [])
                | [("children", arg), ...rest] => (Some(arg), [])
                | [arg, ...rest] => let (children, args) = loop(rest);
                  (children, [arg, ...args])
              };
              let (children, props) = loop(args);
              let props = switch children {
                | None => props
                | Some(arg) => props @ [("children", arg)]
              };
              let fn = Exp.ident(Location.mkloc(switch contents {
                  | Ldot(one, two) => Ldot(one, unCapitalize(two))
                  | Lident(one) => Lident(unCapitalize(one))
                  | _ => assert(false)
                }, loc));
              Exp.construct(
                Location.mkloc(Ldot(Lident("Fluid"), "Custom"), loc),
              Some(Exp.apply(
                Exp.ident(Location.mkloc(Ldot(Ldot(Lident("Fluid"), "Maker"), "makeComponent"), loc)),
                [
                  ("", fn),
                  ("", Exp.apply(
                fn,
                props
                ))])))
            | _ => Ast_mapper.default_mapper.expr(mapper, expr)
          }
        }
        | _ => Ast_mapper.default_mapper.expr(mapper, expr)
      };
    }
  };

let () = Ast_mapper.run_main(mapper);