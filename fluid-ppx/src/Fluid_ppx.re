
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

let mapper = _argv =>
  Parsetree.{
    ...Ast_mapper.default_mapper,
    /* TODO throw error on structure items */
    expr: (mapper, expr) => {
      switch expr {
        | {
          pexp_attributes: [({txt: "JSX"}, Parsetree.PStr([]))],
          pexp_desc: Pexp_apply(target, args)
        } => {
          let args = args |. Belt.List.map(((a, b)) => (a, mapper.expr(mapper, b)));
          switch (target.pexp_desc) {
            | Pexp_ident({txt: Longident.Lident(name)}) when isLowerCase(String.sub(name, 0, 1)) =>
              let rec loop = args => switch args {
                | [] => assert(false)
                | [("children", arg), ...rest] => (arg, rest)
                | [arg, ...rest] => let (children, args) = loop(rest);
                  (children, [arg, ...args])
              };
              let (children, props) = loop(args);
              Exp.apply(
                Exp.ident(Location.mknoloc(Ldot(Lident("Fluid"), "builtin"))),
                [
                  ("", Exp.constant(Const_string(name, None))),
                  ("", Ast_helper.Exp.apply(
                    Exp.ident(Location.mknoloc(Ldot(Lident("Fluid"), "domProps"))),
                    props
                  )),
                  /* TODO auto-up strings */
                  ("", children)
                ]
              )
            | Pexp_ident({txt: Ldot(contents, "createElement")}) =>
              let rec loop = args => switch args {
                | [] => (None, args)
                | [("children", {pexp_desc: Pexp_construct({txt: Lident("[]")}, None)}), ...rest] => (None, rest)
                | [("children", arg), ...rest] => (Some(arg), rest)
                | [arg, ...rest] => let (children, args) = loop(rest);
                  (children, [arg, ...args])
              };
              let (children, props) = loop(args);
              let props = switch children {
                | None => props
                | Some(arg) => props @ [("children", arg)]
              };
              Exp.apply(
                Exp.ident(Location.mknoloc(Ldot(contents, "make"))),
                [("", Ast_helper.Exp.apply(
                  Exp.ident(Location.mknoloc(Ldot(contents, "props"))),
                  props
                ))]
              )
            | _ => Ast_mapper.default_mapper.expr(mapper, expr)
          }
        }
        | _ => Ast_mapper.default_mapper.expr(mapper, expr)
      };

      /* switch expr.pexp_desc {
      | Pexp_extension(({txt, loc}, PStr([{pstr_desc: Pstr_eval({pexp_loc, pexp_desc: Pexp_try(value, handlers)}, attributes)}]))) => {
        let ident = parseLongident(txt);
        let last = Longident.last(ident);
        if (last != String.capitalize(last)) {
          Ast_mapper.default_mapper.expr(mapper, expr)
        } else {
          let handlerLocStart = List.hd(handlers).pc_lhs.ppat_loc;
          let handlerLocEnd = List.nth(handlers, List.length(handlers) - 1).pc_rhs.pexp_loc;
          let handlerLoc = {...handlerLocStart, loc_end: handlerLocEnd.loc_end};
          let try_ = Ast_helper.Exp.ident(~loc=pexp_loc, Location.mkloc(Longident.Ldot(ident, "try_"), loc));
          Ast_helper.Exp.apply(
            ~loc,
            try_,
            [
              ("", mapper.expr(mapper, value)),
              ("", Ast_helper.Exp.function_(~loc=handlerLoc, handlers))
            ]
          )
        }
      }
      | Pexp_extension(({txt, loc}, PStr([{pstr_desc: Pstr_eval({pexp_desc: Pexp_let(Nonrecursive, bindings, continuation)}, attributes)}]))) => {
        let ident = parseLongident(txt);
        let last = Longident.last(ident);
        if (last != String.capitalize(last)) {
          Ast_mapper.default_mapper.expr(mapper, expr)
        } else {
          let (pat, expr) = process_bindings(bindings, ident);
          let let_ = Ast_helper.Exp.ident(~loc, Location.mkloc(Longident.Ldot(ident, "let_"), loc));
          Ast_helper.Exp.apply(
            ~loc,
            let_,
            [
              ("", mapper.expr(mapper, expr)),
              ("", Ast_helper.Exp.fun_(~loc, "", None, pat, mapper.expr(mapper, continuation)))
            ]
          )
        }
      }
      | _ => Ast_mapper.default_mapper.expr(mapper, expr)
      } */
    }
  };

let () = Ast_mapper.run_main(mapper);