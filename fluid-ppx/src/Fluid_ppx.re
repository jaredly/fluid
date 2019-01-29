open Migrate_parsetree;
open OCaml_402.Ast;

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

open Parsetree;

let mapper = 
  Parsetree.{
    ...Ast_mapper.default_mapper,
    expr: (mapper, expr) => {
      switch expr {
        | {pexp_desc: Pexp_extension((
            {txt: "hook", loc},
            PStr([{ pstr_desc: Pstr_eval({pexp_loc, pexp_desc: Pexp_let(Nonrecursive, [{
              pvb_pat, pvb_loc, pvb_expr: {pexp_loc: apploc, pexp_desc: Pexp_apply(target, args)}
            }], continuation)}, attributes)}])
          ))} =>
          Exp.let_(
            ~loc=pexp_loc,
            Nonrecursive,
            [Vb.mk(~loc=pvb_loc,
              Pat.tuple(~loc=pvb_loc, [pvb_pat, [%pat? hooks]]),
              Exp.apply(~loc=apploc, target, args @ [("", Exp.ident(~loc=loc, Location.mkloc(Lident("hooks"), loc)))])
            )],
            mapper.expr(mapper, continuation)
          )

        | {
          pexp_attributes: [({txt: "JSX"}, Parsetree.PStr([]))],
          pexp_desc: Pexp_apply(target, args)
        } => {
          let args = args |> List.map(((a, b)) => (a, mapper.expr(mapper, b)));
          switch (target.pexp_desc) {
            | Pexp_ident({txt: Longident.Lident(name), loc}) when isLowerCase(String.sub(name, 0, 1)) =>
              let rec loop = args => switch args {
                | [] => args
                | [("children", {pexp_desc: Pexp_construct({txt: Lident("[]")}, None)}), ...rest] => rest
                | [("children", arg), ...rest] => args
                | [arg, ...rest] => [arg, ...loop(rest)];
              };
              let props = loop(args);

              Exp.apply(~loc,
                Exp.ident(~loc, Location.mkloc(Ldot(Ldot(Lident("Fluid"), "Native"), name), loc)),
                props
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
                  ("", props == [] ? fn : Exp.apply(
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

/* let () = Ast_mapper.run_main(mapper); */
let () = Driver.register(~name="ppx_fluid",~args=[], Versions.ocaml_402, (_config, _cookies) => mapper);