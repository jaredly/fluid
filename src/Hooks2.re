
/* thinking about a hooks state api */

/**
This has a more straighforward usage style (no CPS), but
it requires Obj.magic to unroll the created hooks stack,
which seems worse on the whole. I was planning to do a
ppx anyway, so here goes.
 */

type context('initial, 'final) = {
  hooks: 'initial,
  finish: 'final => unit
};

type hooks('current, 'prev) = {
  invalidate: unit => unit,
  current: option('current),
  prev: 'prev,
};

let unwrap: hooks(unit, unit) => unit = hooks => {
  ()
};

/* type state('contents) = {
  current: ref('contents),
  mutable setState: 'contents => unit,
}; */

type effect('args) = {
  args: 'args,
  prevArgs: option('args),
  cleanup: option(unit => unit),
  fn: unit => (unit => unit),
};

/* let newState = (initial, invalidate) => {
  let r = ref(initial);
  {current: r, setState: v => {
    r.contents = v;
    invalidate()
  }}
}; */

let useState = (initial: 'a, hooks: hooks(
  ('next, ref('a)),
  'prev
)): (('a, 'a => unit), hooks(
  't,
  (ref('a), 'prev)
))  => {
  let (state, context) = switch (hooks.current) {
    | None => 
      let st = ref(initial);
      (st, {...hooks, current: None, prev: (st, hooks.prev)})
    | Some((next, state)) =>
      (state, {...hooks, current: Some(next), prev: (state, hooks.prev)})
  };
  ((state.contents, v => {state.contents = v; hooks.invalidate()}), context)
};

let newEffect = (fn, args) => {fn, args, prevArgs: None, cleanup: None};

let useEffect = (fn, args, hooks: hooks(('next, effect('args)), 'orev)) => {
  switch (hooks.current) {
    | None =>
      {...hooks, current: None, prev: (newEffect(fn, args), hooks.prev)}
    | Some((next, effect)) =>
      {...hooks, current: Some(next), prev: ({
        ...effect,
        fn,
        args,
        prevArgs: Some(effect.args)
      }, hooks.prev)}
  };
};

let myComponent = (~some, ~prop, {hooks, finish}) => {
  let ((count, setCount), hooks) = useState(10, hooks);
  let ((name, setName), hooks) = useState("name", hooks);
  let hooks = useEffect(() => () => (), (), hooks);
  finish(hooks)
};
