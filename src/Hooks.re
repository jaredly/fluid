
/* thinking about a hooks state api */

/**
This is the CPS version, which doesn't require magic to get the types to work.
This seems like a good thing.
 */

type context('initial) = {
  hooks: 'initial,
  finish: 'initial => unit
};

type state('t) = {
  mutable prev: option('t),
  mutable current: 't,
};

type hooks('current) = {
  invalidate: unit => unit,
  current: option('current),
};

type effect('args) = {
  args: 'args,
  prevArgs: option('args),
  cleanup: option(unit => unit),
  fn: unit => (unit => unit),
};

let useState =
    (
      initial: 'a,
      hooks: hooks((option('next), state('a))),
      fin: (('a, 'a => unit), hooks('next)) => ('res, hooks('next)),
    )
    : ('res, hooks((option('next), state('a)))) => {
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

let newEffect = (fn, args) => {fn, args, prevArgs: None, cleanup: None};

let useEffect = (fn, args, hooks: hooks((option('next), effect('args))), fin) => {
  switch (hooks.current) {
    | None =>
      let effect = newEffect(fn, args);
      let (res, hooks) = fin({...hooks, current: None});
      (res, {...hooks, current: Some((hooks.current, effect))})
    | Some((next, effect)) =>
      let effect = {
        ...effect,
        fn,
        args,
        prevArgs: Some(effect.args)
      };
      let (res, hooks) = fin({...hooks, current: next});
      (res, {...hooks, current: Some((hooks.current, effect))})
  };
};

let myComponent = (~some, ~prop, {hooks, finish}) => {
  Js.log("Here");
  let (res, hooks) = useState(10, hooks, ((count, setCount), hooks) => {
    useState("name", hooks, ((name, setName), hooks) => {
      Js.log("Hi");
      useEffect(() => () => (), (), hooks, hooks => {
        Js.log("Ho");
        ("contents", hooks);
      });
    })
  });
  finish(hooks);
  res
};

/* <myComponent some=10 /> */

