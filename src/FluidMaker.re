open Belt;

module type NativeInterface = {
  type element;
  type nativeNode;
  type font;

  let setImmediate: (unit => unit) => unit;


  /* If it returns true, then the elements were of the same type and the mounted node was updated.
  If false, they weren't of the same type and no update happened.
   */
  let canUpdate: (~mounted: element, ~mountPoint: nativeNode, ~newElement: element) => bool;

  let update: (element, nativeNode, element) => unit;

  let inflate: (element, Layout.node) => nativeNode;

  let measureText: (string, option(font)) => Layout.measureType;

  let createNullNode: unit => nativeNode;
  /* let createTextNode: (string, Layout.node, option(font)) => nativeNode; */
  /* let setTextContent: (nativeNode, string, option(font)) => unit; */
  let appendChild: (nativeNode, nativeNode) => unit;
  let appendAfter: (nativeNode, nativeNode) => unit;
  /* let insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit; */
  let removeChild: (nativeNode, nativeNode) => unit;
  let replaceWith: (nativeNode, nativeNode) => unit;
};

/* module type Fluid = {
  module NativeInterface: NativeInterface;
  type customWithState;
  type hooksContainer('hooks, 'reconcileData);
  type custom;
  type effect;
  type element =
  | String(string)
  | Builtin(NativeInterface.element, list(element), option(Layout.measureType)): element
  | Custom(custom /* already contains its props & children */)

  and container = {
    mutable custom: customWithState,
    mutable mountedTree
  }

  and instanceTree =
  | IString(string)
  | IBuiltin(NativeInterface.element, list(instanceTree)) : instanceTree
  | ICustom(customWithState, instanceTree, list(effect))

  and mountedTree =
  | MString(string, NativeInterface.nativeNode)
  | MBuiltin(NativeInterface.element, NativeInterface.nativeNode, list(mountedTree)): mountedTree
  | MCustom(container);

  type reconcilerFunction('data) = ('data, 'data, mountedTree, element) => mountedTree;

  let getNativeNode: mountedTree => NativeInterface.nativeNode;
  let inflateTree: instanceTree => mountedTree;
  let instantiateTree: element => instanceTree;

  let mount: (element, NativeInterface.nativeNode) => unit;
  module Maker:
    {
      let makeComponent:
        ('identity, hooksContainer('hooks, 'reconcileData) => element) => custom;
    };

  module Hooks: {
    type effect('a);
    let useReconciler:
      ('a, reconcilerFunction('a),
      hooksContainer((option('b), 'a), 'a),
      (unit, hooksContainer('b, 'a)) =>
      ('c, hooksContainer('d, 'e))) =>
      ('c, hooksContainer((option('d), 'a), 'e));
    let useRef:
      ('a, hooksContainer((option('b), ref('a)), 'c),
      (ref('a), hooksContainer('b, 'c)) =>
      ('d, hooksContainer('e, 'f))) =>
      ('d, hooksContainer((option('e), ref('a)), 'f));
    let useState:
      ('a, hooksContainer((option('next), ref('a)), 'z),
      (('a, 'a => unit), hooksContainer('next, 'z)) =>
      ('res, hooksContainer('next, 'z))) =>
      ('res, hooksContainer((option('next), ref('a)), 'z));
    let useReducer:
      ('a, ('a, 'b) => 'a,
      hooksContainer((option('c), ref('a)), 'd),
      (('a, 'b => unit), hooksContainer('c, 'd)) =>
      ('e, hooksContainer('c, 'd))) =>
      ('e, hooksContainer((option('c), ref('a)), 'd));
    let useEffect:
      ((unit, unit) => unit, 'args,
      hooksContainer((option('next), effect('args)), 'z),
      (unit, hooksContainer('next, 'z)) =>
      ('a, hooksContainer('b, 'c))) =>
      ('a, hooksContainer((option('b), effect('args)), 'c));
    let useMemo:
      (unit => 'a, 'b, hooksContainer((option('c), ('a, 'b)), 'd),
      ('a, hooksContainer('c, 'd)) =>
      ('e, hooksContainer('f, 'g))) =>
      ('e, hooksContainer((option('f), ('a, 'b)), 'g));
    let useCallback:
      ('a, 'b, hooksContainer((option('c), ('a, 'b)), 'd),
      ('a, hooksContainer('c, 'd)) =>
      ('e, hooksContainer('f, 'g))) =>
      ('e, hooksContainer((option('f), ('a, 'b)), 'g));
  }
}; */

module F = (NativeInterface: NativeInterface) => {

type effect = {
  cleanup: option(unit => unit),
  fn: (unit, unit) => unit,
  setCleanup: (unit => unit) => unit,
};

type pending('a) =
  | Create
  | Replace(NativeInterface.nativeNode)
  | Update('a, NativeInterface.nativeNode);

/* also need a "compareTo" (other custom) */
type custom = {
  init: unit => customWithState,
  clone: customWithState => [`Different | `Same | `Compatible(customWithState)],
}

and customContents('identity, 'hooks, 'reconcileData) = {
  identity: 'identity,
  render: hooksContainer('hooks, 'reconcileData) => element,
  hooks: ref('hooks),
  mutable invalidated: bool,
  mutable reconciler: option(
    (
'reconcileData, 'reconcileData, reconcilerFunction('reconcileData)
    )
  ),
  mutable onChange: unit => unit,
}

and hooksContainer('hooks, 'reconcileData) = {
  invalidate: unit => unit,
  setReconciler: ('reconcileData, 'reconcileData, reconcilerFunction('reconcileData)) => unit,
  triggerEffect:
    (
      ~cleanup: option(unit => unit),
      ~fn: (unit, unit) => unit,
      ~setCleanup: (unit => unit) => unit
    ) =>
    unit,
  current: ref('hooks),
}

and reconcilerFunction('data) = ('data, 'data, mountedTree, element) => pendingTree
and customWithState = WithState(customContents('identity, 'hooks, 'reconcileData)) : customWithState

and element = 
| Builtin(NativeInterface.element, list(element), option(Layout.style), option(Layout.measureType))
| Custom(custom /* already contains its props & children */)
| Null


and instanceTree = 
| IBuiltin(NativeInterface.element, list(instanceTree), Layout.node)
| ICustom(customWithState, instanceTree, list(effect))
| INull(Layout.node)


and container = {
  mutable custom: customWithState,
  mutable mountedTree: mountedChild,
}

and mountedChild = Pending(pendingTree) | Mounted(mountedTree)

and pendingTree =
| PBuiltin(NativeInterface.element, pending(NativeInterface.element), list(pendingTree), Layout.node)
| PCustom(container, list(effect))
| PNull(pending(unit), Layout.node)

and mountedTree = 
| MBuiltin(NativeInterface.element, NativeInterface.nativeNode, list(mountedTree), Layout.node)
| MCustom(container)
| MNull(NativeInterface.nativeNode, Layout.node)


;

let rec makePending = (inst: instanceTree) => switch inst {
| IBuiltin(el, children, layout) => PBuiltin(el, Create, children->List.map(makePending), layout)
| ICustom(custom, child, effects) => PCustom({custom, mountedTree: Pending(makePending(child))}, effects)
| INull(layout) => PNull(Create, layout)
};

let rec pendingReplace = (mounted: NativeInterface.nativeNode, inst: instanceTree) => switch inst {
| IBuiltin(el, children, layout) => PBuiltin(el, Replace(mounted), children->List.map(makePending), layout)
| ICustom(custom, child, effects) => PCustom({custom, mountedTree: Pending(pendingReplace(mounted, child))}, effects)
| INull(layout) => PNull(Replace(mounted), layout)
};

/* let string = (~layout=?, ~font=?, x) => String(x, layout, font); */

module Maker = {
  let makeComponent = (identity: 'identity, render: hooksContainer('hooks, 'reconcile) => element) => {
    {
      init: () => {
        WithState({
          identity,
          invalidated: false,
          onChange: () => (),
          reconciler: None,
          hooks: ref(None),
          render,
        })
      },
      clone: (WithState(contents)) => {
        /* If the identity is strictly equal, then we know that the types must be the same. */
        if (Obj.magic(contents.identity) === identity) {
          let contents: customContents('props, 'hooks, 'reconcileData) = Obj.magic(contents);
          if (contents.render === render) {
            `Same
          } else {
            `Compatible(WithState({
              ...contents,
              render
            }))
          }
        } else {
          `Different
        }
      }
    }
  };
};

let runRender = (WithState(component)) => {
  let effects = ref([]);
  let hooks = {
    invalidate: () => {
      component.invalidated = true;
      component.onChange();
      /* TODO actually trigger a rerender here */
    },
    setReconciler: (oldData, data, reconcile) => component.reconciler = Some((oldData, data, reconcile)),
    triggerEffect: (~cleanup, ~fn, ~setCleanup) => {
      effects.contents = [{cleanup, fn, setCleanup}, ...effects.contents];
    },
    current: component.hooks,
  };
  component.invalidated = false;
  let tree = component.render(hooks);
  (tree, effects^);
};

let rec getNativeNode = tree =>
  switch (tree) {
  | MNull(node, _)
  | MBuiltin(_, node, _, _) => Some(node)
  | MCustom({mountedTree}) => getNativePending(mountedTree)
  }
and getNativePending = tree =>
  switch (tree) {
  | Pending(tree) =>
    switch (tree) {
    | PNull(Create, _) => None
    | PNull(Replace(node) | Update(_, node), _) => Some(node)
    | PCustom({mountedTree}, _) => getNativePending(mountedTree)
    | PBuiltin(_, Create, _, _) => None
    | PBuiltin(_, Replace(node) | Update(_, node), _, _) => Some(node)
    }
  | Mounted(m) => getNativeNode(m)
  };

/* let getNative */

/*
Phases of the algorithm:

- first, there is an element tree. This doesn't have instantiated custom components
- second, that element tree is instantated into an "instanceTree"
- (TODO new step) the instance tree is given a layout by flex
  - maybe the instance tree has layout nodes on it? I think that would make sense.
- third, that instance tree is mounted to the DOM.

 */

let rec getInstanceLayout = element => switch element {
  | INull(layout)
  | IBuiltin(_, _, layout) => layout
  | ICustom(_, el, _) => getInstanceLayout(el)
};

let rec getMountedLayout = element => switch element {
  | MNull(_, layout)
  | MBuiltin(_, _, _, layout) => layout
  | MCustom({mountedTree: Mounted(mounted)}) => getMountedLayout(mounted)
  | MCustom({mountedTree: Pending(pending)}) => getPendingLayout(pending)
}
and getPendingLayout = element => switch element {
  | PNull(_, layout)
  | PBuiltin(_, _, _, layout) => layout
  | PCustom({mountedTree: Mounted(mounted)}, _) => getMountedLayout(mounted)
  | PCustom({mountedTree: Pending(pending)}, _) => getPendingLayout(pending)
};

let rec instantiateTree: element => instanceTree = el => switch el {
  | Null => INull(Layout.createNode([||], Layout.style()))

  | Builtin(nativeElement, children, layout, measure) =>
    let ichildren = children->List.map(instantiateTree);
    let childLayouts = ichildren->List.map(getInstanceLayout)->List.toArray;
    let style = switch layout {
      | None => Layout.style()
      | Some(s) => s
    };
    IBuiltin(nativeElement, ichildren, switch measure {
      | None => Layout.createNode(childLayouts, style)
      | Some(m) => Layout.createNodeWithMeasure(childLayouts, style, m)
    })

  | Custom(custom) =>
    /* How does it trigger a reconcile on setState? */
    let custom = custom.init();
    let (tree, effects) = custom->runRender;
    ICustom(custom, instantiateTree(tree), effects)
};

let runEffect = ({cleanup, setCleanup, fn}) => {
  switch (cleanup) {
  | Some(m) => m()
  | None => ()
  };
  setCleanup(fn());
};


type root = {
  mutable layout: Layout.node,
  mutable node: option(NativeInterface.nativeNode),
  mutable invalidatedElements: list(container),
  mutable waiting: bool,
};


/* let rec inflateTree: (container => unit, instanceTree) => mountedTree = (enqueue, el) => switch el {
  | INull(layout) => MNull(NativeInterface.createNullNode(), layout)

  | IBuiltin(nativeElement, children, layout) =>
    let node = NativeInterface.inflate(nativeElement, layout);
    let children = children->List.map(inflateTree(enqueue));
    children->List.map(getNativeNode)->List.forEach(childNode => {
      switch childNode {
        | None => failwith("Inflating a tree that's still pending")
        | Some(childNode) => NativeInterface.appendChild(node, childNode)
      }
    });
    MBuiltin(nativeElement, node, children, layout);

  | ICustom(WithState(contents) as custom, instanceTree, effects) =>
    let mountedTree = inflateTree(enqueue, instanceTree)
    let container = {custom, mountedTree: Mounted(mountedTree)};
    contents.onChange = () => enqueue(container);
    effects->List.forEach(runEffect);
    MCustom(container)
}; */

type mountPoint = AppendChild(NativeInterface.nativeNode) | NextSibling(NativeInterface.nativeNode);

let mountTo = (point, node) => switch point {
  | AppendChild(prev) => NativeInterface.appendChild(prev, node)
  /* I don't currently allow inserts, so I dont need this? */
  | NextSibling(prev) => NativeInterface.appendAfter(prev, node)
};

let rec mountPending: (container => unit, mountPoint, pendingTree) => mountedTree = (enqueue, mount, el) => switch el {
  | PNull((Create | Replace(_)) as prev, layout) =>
    let rep = NativeInterface.createNullNode();
    /* TODO seems like there should be a better way to model the "mountPoit" vs "pending" thing */
    switch prev {
      | Create => mountTo(mount, rep)
      | Replace(prev) => NativeInterface.replaceWith(prev, rep);
      | _ => ()
    }
    MNull(rep, layout)

  | PNull(Update((), node), layout) =>
    MNull(node, layout)

  | PBuiltin(native, Update(prevNative, node), children, layout) =>
    NativeInterface.update(prevNative, node, native);
    MBuiltin(native, node, children->List.map(mountPending(enqueue, AppendChild(node))), layout)

  | PBuiltin(native, (Create | Replace(_)) as prev, children, layout) =>
    let node = NativeInterface.inflate(native, layout);
    let children = children->List.map(mountPending(enqueue, AppendChild(node)));
    /* children->List.map(getNativeNode)->List.forEach(childNode => {
      switch childNode {
        | None => failwith("Inflating a tree that's still pending")
        | Some(childNode) => NativeInterface.appendChild(node, childNode)
      }
    }); */
    switch (prev) {
      | Create =>
      Js.log2("Creating", node);
      mountTo(mount, node);
      | Replace(prev) =>
      Js.log3("Replacing", prev, node)
      NativeInterface.replaceWith(prev, node)
      | _ => ()
    };
    MBuiltin(native, node, children, layout);

  | PCustom({custom, mountedTree: Mounted(mountedTree)} as container, effects) =>
    if (effects != []) {
      print_endline("Warning: Unchagned node had effects")
    };
    MCustom(container)

  | PCustom({custom: WithState(contents) as custom, mountedTree: Pending(pendingTree)}, effects) =>
    let mountedTree = mountPending(enqueue, mount, pendingTree);
    let container = {custom, mountedTree: Mounted(mountedTree)};
    contents.onChange = () => enqueue(container);
    effects->List.forEach(runEffect);
    MCustom(container)
};

/* and listenForChanges = (WithState(contents) as component, container, enqueue) => {
  contents.onChange = () => {
    enqueue(container)
    /* let (newElement, effects) = component->runRender;
    container.mountedTree = switch (contents.reconciler) {
      | Some((oldData, newData, reconcile)) => reconcile(oldData, newData, container.mountedTree, newElement)
      | _ => reconcileTrees(container.mountedTree, newElement)
    };
    effects->List.forEach(runEffect); */
  }
}; */

let rec reconcileTrees: (container => unit, mountedTree, element) => pendingTree = (enqueue, prev, next) => switch (prev, next) {
  | (MBuiltin(aElement, node, aChildren, aLayout), Builtin(bElement, bChildren, bLayoutStyle, bMeasure)) =>
    /* TODO should be "canUpdate"... or maybe it should return... the things... ok I need a prev el or sth */
    if (NativeInterface.canUpdate(~mounted= aElement, ~mountPoint=node, ~newElement=bElement)) {
      aLayout.style = switch bLayoutStyle {
        | Some(s) => s
        | _ => Layout.style()
      };
      /* TODO assign the measure function */
      /* TODO flush layout changes */
      PBuiltin(bElement, Update(aElement, node), reconcileChildren(enqueue, node, aChildren, bChildren), aLayout);
    } else {
      let instances = instantiateTree(next);
      /* let instanceLayout = getInstanceLayout(instances);
      Layout.layout(instanceLayout);
      let tree = inflateTree(enqueue, instances);
      NativeInterface.replaceWith(getNativeNode(prev), getNativeNode(tree)); */
      /* unmount prev nodes */
      /* tree */
      pendingReplace(node, instances)
    }
  | (MCustom(a), Custom(b)) =>
    switch (b.clone(a.custom)) {
      | `Same => PCustom(a, [])
      | `Compatible(custom) =>
        let (newElement, effects) = custom->runRender;
        /* TODO custom reconciler */
        switch (a.mountedTree) {
          | Pending(_) => failwith("Reconciling a componenet that's still pending.")
          | Mounted(mountedTree) =>
            let tree = reconcileTrees(enqueue, mountedTree, newElement);
            /* 
            a.custom = custom;
            a.mountedTree = tree;
            effects->List.forEach(runEffect); */
            /* MCustom(a) */
            PCustom({custom, mountedTree: Pending(tree)}, effects)
        }
      | `Different =>
        /* Js.log3("different", a, b); */
        let instances = instantiateTree(next);
        /* let instanceLayout = getInstanceLayout(instances);
        Layout.layout(instanceLayout);
        let tree = inflateTree(enqueue, instances);
        /* unmount prev nodes */
        NativeInterface.replaceWith(getNativeNode(prev), getNativeNode(tree));
        tree */
        switch (getNativeNode(prev)) {
          | None =>
          print_endline("Warning! Prev custom component was pending");
          makePending(instances)
          | Some(node) => pendingReplace(node, instances)
        }
    }
  | _ =>
    let instances = instantiateTree(next);
    switch (getNativeNode(prev)) {
      | None =>
      print_endline("Warning! Prev custom component was pending");
      makePending(instances)
      | Some(node) => pendingReplace(node, instances)
    }
    /* let instances = instantiateTree(next);
    let instanceLayout = getInstanceLayout(instances);
    Layout.layout(instanceLayout);
    let tree = inflateTree(enqueue, instances);
    /* unmount prev nodes */
    NativeInterface.replaceWith(getNativeNode(prev), getNativeNode(tree));
    tree */
} and reconcileChildren = (enqueue, parentNode, aChildren, bChildren) => {
  switch (aChildren, bChildren) {
    | ([], []) => []
    | ([], _) =>
      let more = bChildren->List.map(child => makePending(instantiateTree(child)));
      /* let more = bChildren->List.map(child => inflateTree(enqueue, instantiateTree(child)));
      more->List.forEach(child => NativeInterface.appendChild(parentNode, getNativeNode(child))); */
      more
    | (more, []) => 
      /* TODO is this the right place for that? */
      more->List.keepMap(getNativeNode)->List.forEach(NativeInterface.removeChild(parentNode));
      []
    | ([one, ...aRest], [two, ...bRest]) =>
      [reconcileTrees(enqueue, one, two), ...reconcileChildren(enqueue, parentNode, aRest, bRest)]
  }
};

let rec enqueue = (root, custom) => {
  root.invalidatedElements = [custom, ...root.invalidatedElements];
  if (!root.waiting) {
    root.waiting = true;
    NativeInterface.setImmediate(() => {
      root.waiting = false;
      let elements = root.invalidatedElements;
      root.invalidatedElements = [];
      let toUpdate = elements->List.keepMap(({custom: WithState(contents) as component} as container) => {
        if (contents.invalidated)  {
          let (newElement, effects) = component->runRender;
          switch (container.mountedTree) {
            | Pending(_) =>
            print_endline("Updating a pending tree...")
            None
            | Mounted(mountedTree) => 
              Some((container, switch (contents.reconciler) {
                | Some((oldData, newData, reconcile)) => reconcile(oldData, newData, mountedTree, newElement)
                | _ => reconcileTrees(enqueue(root), mountedTree, newElement)
              }, effects))
          }
        } else {
          None
        }
      });
      /* TODO if something absolutely positioned, only need to do it from there */
      Layout.layout(root.layout);
      /* [%bs.debugger]; */
      toUpdate->List.forEach(((container, pending, effects)) => {
        effects->List.forEach(runEffect);
        let current = switch (getNativePending(container.mountedTree)) {
          | Some(mounted) => mounted
          | None => failwith("Current is already pending")
        };
        container.mountedTree = Mounted(mountPending(enqueue(root), AppendChild(current), pending))
      })
    })
  };
};

/*
TODO what if thre's a setState right after/during the first render?
 */

let mount = (el, node) => {
  let instances = instantiateTree(el);
  let instanceLayout = getInstanceLayout(instances);
  Layout.layout(instanceLayout);

  let root = {
    layout: instanceLayout,
    node: None,
    invalidatedElements: [],
    waiting: false
  };


  let tree = mountPending(enqueue(root), AppendChild(node), makePending(instances));
  switch (getNativeNode(tree)) {
    | None => failwith("Still pending?")
    | Some(childNode) =>
      root.node = Some(childNode);
      node->NativeInterface.appendChild(childNode)
  }
};

/*

On a component, I need to be able to:

- presumably instantiate with props
- update the props w/ new props. or return a new version that has the new props
  but only if they have the same underlying type
- render() a new tree
lifecycle methods or something

 */

  module Hooks = {

    let useReconciler = (data, fn, hooks) => {
      let next = switch (hooks.current^) {
        | None => ref(None)
        | Some((r, next)) =>
          hooks.setReconciler(r, data, fn);
          next
      };
      hooks.current := Some((data, next));
      ((), hooks)
    };

    let useRef = (initial, hooks) => {
      switch (hooks.current^) {
        | None =>
          let r = ref(initial);
          let next = ref(None);
          hooks.current := Some((r, next));
          (r, {...hooks, current: next})
        | Some((r, next)) => (r, {...hooks, current: next})
      };
    };

    let useState =
        (
          initial,
          hooks
        ) => {
      let (state, next) =
        switch (hooks.current^) {
        | None =>
          let next = ref(None);
          hooks.current := Some((initial, next));
          (initial, next);
        | Some((state, next)) => 
          (state, next)
        };
      (
        (
          state,
          v => {
            hooks.current := Some((v, next));
            hooks.invalidate();
          },
        ),
        {...hooks, current: next},
      )
    };

    /* [@hook]
    let useReducer = (reducer, initial) => {
      let%hook (state, setState) = useState(initial);
      (state, action => setState(reducer(state, action)))
    }; */

    let useReducer = (initial, reducer, hooks) => {
      let ((state, setState), hooks) = useState(initial, hooks);
      ((state, action => setState(reducer(state, action))), hooks)
    };

    type effect('args) = {
      args: 'args,
      cleanup: ref(option(unit => unit)),
      fn: unit => (unit => unit),
    };

    let newEffect = (fn, args) => {fn, args, cleanup: ref(None)};

    let useEffect = (fn, args, hooks) => {
      let (effect, next) = switch (hooks.current^) {
        | None =>
          let effect = newEffect(fn, args);
          hooks.triggerEffect(~cleanup=effect.cleanup.contents, ~fn, ~setCleanup=v => {
            effect.cleanup.contents = Some(v)
          });
          (effect, ref(None))
        | Some((effect, next)) =>
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

          (effect, next)
      };
      hooks.current := Some((effect, next));
      ((), {...hooks, current: next})
    };

    let useMemo = (fn, args, hooks) => {
      let (value, next) =
        switch (hooks.current^) {
        | None => (fn(), ref(None))
        | Some(((value, prevArgs), next)) =>
          let value = prevArgs == args ? value : fn();
          (value, next);
        };
      hooks.current := Some(((value, args), next));
      (value, {...hooks, current: next})
    };

    let useCallback = (fn, args, hooks) => {
      useMemo(() => fn, args, hooks)
    };
  }

};