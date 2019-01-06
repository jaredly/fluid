
/* thinking about a hooks state api */

/**
This is the CPS version, which doesn't require magic to get the types to work.
This seems like a good thing.


hrrrrm but will I need magic to process the hooks?
this is a good question.
maybe the context can have functions that I call
like `registerEffect` or sth

and in this way we can collect the lifecycle things.

because state is the only iffy bit, types-wise, right?
that and 'ref's. well we don't actually need refs. or do we?
I guess for things that you won't want to trigger a re-render.
 */

type state('t) = {
  mutable prev: option('t),
  mutable current: 't,
};

open Fluid;

let useRef = (initial, hooks, fin) => {
  let (r, hooks) = switch (hooks.current) {
    | None =>
      (ref(initial), {...hooks, current: None})
    | Some((inner, r)) => (r, {...hooks, current: inner})
  };
  let (res, hooks) = fin(r, hooks);
  (res, {...hooks, current: Some((hooks.current, r))})
};

let useState =
    (
      initial: 'a,
      hooks: hooksContainer((option('next), state('a)), 'z),
      fin: (('a, 'a => unit), hooksContainer('next, 'z)) => ('res, hooksContainer('next, 'z)),
    )
    : ('res, hooksContainer((option('next), state('a)), 'z)) => {
  let (state, hooks) =
    switch (hooks.current) {
    | None =>
      let st = {current: initial, prev: None};
      (st, {...hooks, current: None});
    | Some((next, state)) => (state, {...hooks, current: next})
    };
  let (res, hooks) =
    fin(
      (
        state.current,
        v => {
          state.prev = Some(state.current);
          state.current = v;
          hooks.invalidate();
        },
      ),
      hooks,
    );
  (res, {...hooks, current: Some((hooks.current, state))});
};

/* [@hook]
let useReducer = (reducer, initial) => {
  let%hook (state, setState) = useState(initial);
  (state, action => setState(reducer(state, action)))
}; */

let useReducer = (initial, reducer, hooks, fin) => {
  useState(initial, hooks, ((state, setState), hooks) => {
    fin((state, action => setState(reducer(state, action))), hooks)
  });
};

type effect('args) = {
  args: 'args,
  cleanup: ref(option(unit => unit)),
  fn: unit => (unit => unit),
};

let newEffect = (fn, args) => {fn, args, cleanup: ref(None)};

let useEffect = (fn, args, hooks: hooksContainer((option('next), effect('args)), 'z), fin) => {
  switch (hooks.current) {
    | None =>
      let effect = newEffect(fn, args);
      hooks.triggerEffect(~cleanup=effect.cleanup.contents, ~fn, ~setCleanup=v => {
        effect.cleanup.contents = Some(v)
      });
      let (res, hooks) = fin((), {...hooks, current: None});
      (res, {...hooks, current: Some((hooks.current, effect))})
    | Some((next, effect)) =>
      let effect = if (effect.args != args) {
        hooks.triggerEffect(~cleanup=effect.cleanup.contents, ~fn, ~setCleanup=v => {
          effect.cleanup.contents = Some(v)
        });
        {
          ...effect,
          fn,
          args,
        };
      } else {
        effect
      };

      let (res, hooks) = fin((), {...hooks, current: next});
      (res, {...hooks, current: Some((hooks.current, effect))})
  };
};

let useMemo = (fn, args, hooks, fin) => {
  let (value, current) =
    switch (hooks.current) {
    | None => (fn(), None)
    | Some((next, (value, prevArgs))) =>
      let value = prevArgs == args ? value : fn();
      (value, next);
    };
  let (res, hooks) = fin(value, {...hooks, current});
  (res, {...hooks, current: Some((hooks.current, (value, args)))});
};

let useCallback = (fn, args, hooks, fin) => {
  useMemo(() => fn, args, hooks, fin)
};

/* let myComponent = (~some, ~prop, {hooks, finish}) => {
  Js.log("Here");
  let%hook (state, dispatch) = useReducer(None, action => switch action {
    | `Awesome => Some(10)
    | `Nope => None
  });
  let%hook (count, setCount) = useState(10);
  let%hook (name, setName) = useState("name");
  Js.log("Hi");
  let%hook () = useEffect(() => {
    () => ()
  }, ());
  Js.log("Ho");
  "contents"
}; */


let myComponent = (props, {hooks, finish}) => {
  Js.log("Here");
  let (res, hooks) = useState(10, hooks, ((count, setCount), hooks) => {
    useState("name", hooks, ((name, setName), hooks) => {
      Js.log("Hi");
      useEffect(() => () => (), (), hooks, ((), hooks) => {
        Js.log("Ho");
        useReducer(None, (_state, action) => switch action {
          | `Awesome => Some(10)
          | `Nope => None
        }, hooks, ((state, dispatch), hooks) => {
          ("contents", hooks);
        });
      });
    })
  });
  finish(hooks);
  res
};

/* <myComponent some=10 /> */

