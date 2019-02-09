
module type C = {type arg; type res;let name: string;let once: bool};
module type Tracker = (C: C) => {
  type callbackId;
  type arg = C.arg;
  type res = C.res;
  type fn = arg => res;
  let track: fn => callbackId;
  let maybeTrack: option(fn) => option(callbackId);
  let untrack: fn => unit;
  let maybeUntrack: option(fn) => unit;
  let size: unit => int;
};

module C = (C: {type arg; let name: string}) => {
  type arg = C.arg;
  type res = unit;
  let name = C.name;
  let once = false;
};

module F: Tracker = (C: C) => {
  type callbackId = int;
  type arg = C.arg;
  type res = C.res;
  type fn = C.arg => res;

  let fns: Hashtbl.t(callbackId, fn) = Hashtbl.create(100);
  let ids = ref([]);

  let cur = ref(0);
  let next = () => {cur := cur^ + 1; cur^};

  let track: fn => callbackId = fn => {
    switch (Belt.List.getAssoc(ids^, (fn), (===))) {
      | None =>
        let id = next();
        Hashtbl.replace(fns, id, fn);
        ids := [(fn, id), ...ids^];
        id
      | Some(id) => id
    }
  };
  let maybeTrack: (option(fn)) => option(callbackId) = fn => switch fn {
    | None => None
    | Some(fn) => Some(track(fn))
  };

  let untrack = fn => {
    switch (Belt.List.getAssoc(ids^, (fn), (===))) {
      | None => print_endline("> but not there")
      | Some(id) =>
        ids := ids^ ->Belt.List.keep(a => fst(a) !== fn);
        Hashtbl.remove(fns, id)
    };
  };
  let maybeUntrack: (option(fn)) => unit = fn => switch fn {
    | None => ()
    | Some(fn) => untrack(fn)
  };

  let size = () => Hashtbl.length(fns);

  let call = (id: callbackId, arg: C.arg): res => {
    switch (Hashtbl.find(fns, id)) {
      | exception Not_found =>
        failwith("Failed to find callback! " ++ string_of_int(id))
      | fn => 
        if (C.once) {
          untrack(fn)
        };
        fn(arg)
    };
  };
  Callback.register(C.name, call);
};
