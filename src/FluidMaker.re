open Belt;

module type NativeInterface = {
  type element;
  type nativeNode;
  type font;


  /* If it returns true, then the elements were of the same type and the mounted node was updated.
  If false, they weren't of the same type and no update happened.
   */
  let maybeUpdate: (~mounted: element, ~mountPoint: nativeNode, ~newElement: element) => bool;

  let inflate: (element, Layout.node) => nativeNode;

  let measureText: (string, option(font)) => Layout.measureType;

  let createTextNode: (string, Layout.node, option(font)) => nativeNode;
  let setTextContent: (nativeNode, string, option(font)) => unit;
  let appendChild: (nativeNode, nativeNode) => unit;
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

and reconcilerFunction('data) = ('data, 'data, mountedTree, element) => mountedTree
and customWithState = WithState(customContents('identity, 'hooks, 'reconcileData)) : customWithState

and element =
| String(string, option(Layout.style), option(NativeInterface.font))
| Builtin(NativeInterface.element, list(element), option(Layout.style), option(Layout.measureType)): element
| Custom(custom /* already contains its props & children */)

and instanceTree =
| IString(string, Layout.node, option(NativeInterface.font))
| IBuiltin(NativeInterface.element, list(instanceTree), Layout.node)
| ICustom(customWithState, instanceTree, list(effect))

and container = {
  mutable custom: customWithState,
  mutable mountedTree
}

and mountedTree =
| MString(string, NativeInterface.nativeNode, Layout.node, option(NativeInterface.font))
| MBuiltin(NativeInterface.element, NativeInterface.nativeNode, list(mountedTree), Layout.node): mountedTree
| MCustom(container)

;

let string = (~layout=?, ~font=?, x) => String(x, layout, font);

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
        /* If the `identity` is strictly equal, then we know that the types must be the same. */
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

let rec getNativeNode = tree => switch tree {
  | MString(_, node, _, _)
  | MBuiltin(_, node, _, _) => node
  | MCustom({mountedTree}) => getNativeNode(mountedTree)
};

/*
Phases of the algorithm:

- first, there is an element tree. This doesn't have instantiated custom components
- second, that element tree is instantated into an "instanceTree"
- (TODO new step) the instance tree is given a layout by flex
  - maybe the instance tree has layout nodes on it? I think that would make sense.
- third, that instance tree is mounted to the DOM.

 */

let rec getInstanceLayout = element => switch element {
  | IString(_, layout, _)
  | IBuiltin(_, _, layout) => layout
  | ICustom(_, el, _) => getInstanceLayout(el)
};

let rec getMountedLayout = element => switch element {
  | MString(_, _, layout, _)
  | MBuiltin(_, _, _, layout) => layout
  | MCustom({mountedTree}) => getMountedLayout(mountedTree)
};

let rec instantiateTree: element => instanceTree = el => switch el {
  | String(contents, layout, font) => 
  IString(
    contents,
    Layout.createNodeWithMeasure(
      [||],
      switch layout { | None => Layout.style() | Some(l) => l },
      NativeInterface.measureText(contents, font),
    ),
    font
  );

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

let rec inflateTree: instanceTree => mountedTree = el => switch el {
  | IString(contents, layout, font) => 
    /* TODO set layout properties here... or something */
    MString(contents, NativeInterface.createTextNode(contents, layout, font), layout, font)

  | IBuiltin(nativeElement, children, layout) =>
    let node = NativeInterface.inflate(nativeElement, layout);
    let children = children->List.map(inflateTree);
    children->List.forEach(child => NativeInterface.appendChild(node, getNativeNode(child)));
    MBuiltin(nativeElement, node, children, layout);

  | ICustom(custom, instanceTree, effects) =>
    let mountedTree = inflateTree(instanceTree)
    let container = {custom, mountedTree};
    custom->listenForChanges(container);
    effects->List.forEach(runEffect);
    MCustom(container)
}

and listenForChanges = (WithState(contents) as component, container) => {
  contents.onChange = () => {
    let (newElement, effects) = component->runRender;
    container.mountedTree = switch (contents.reconciler) {
      | Some((oldData, newData, reconcile)) => reconcile(oldData, newData, container.mountedTree, newElement)
      | _ => reconcileTrees(container.mountedTree, newElement)
    };
    effects->List.forEach(runEffect);
  }
}

and reconcileTrees: (mountedTree, element) => mountedTree = (prev, next) => switch (prev, next) {
  | (MString(a, node, layoutNode, font), String(b, blayout, bfont)) =>
    if (a == b && font == bfont) {
      /* TODO mark a change if layout != blayout */
      layoutNode.style = switch blayout { | None => Layout.style() | Some(l) => l };
      MString(a, node, layoutNode, font)
    } else {
      NativeInterface.setTextContent(node, b, bfont);
      Layout.LayoutSupport.markDirty(layoutNode);
      MString(b, node, layoutNode, bfont)
    }
  | (MBuiltin(aElement, node, aChildren, aLayout), Builtin(bElement, bChildren, bLayoutStyle, bMeasure)) =>
    if (NativeInterface.maybeUpdate(~mounted= aElement, ~mountPoint=node, ~newElement=bElement)) {
      aLayout.style = switch bLayoutStyle {
        | Some(s) => s
        | _ => Layout.style()
      };
      /* TODO assign the measure function */
      /* TODO flush layout changes */
      MBuiltin(bElement, node, reconcileChildren(node, aChildren, bChildren), aLayout);
    } else {
      let instances = instantiateTree(next);
      let instanceLayout = getInstanceLayout(instances);
      Layout.layout(instanceLayout);
      let tree = inflateTree(instances);
      /* unmount prev nodes */
      NativeInterface.replaceWith(getNativeNode(prev), getNativeNode(tree));
      tree
    }
  | (MCustom(a), Custom(b)) =>
    switch (b.clone(a.custom)) {
      | `Same => MCustom(a)
      | `Compatible(custom) =>
        let (newElement, effects) = custom->runRender;
        let tree = reconcileTrees(a.mountedTree, newElement);
        a.custom = custom;
        a.mountedTree = tree;
        effects->List.forEach(runEffect);
        MCustom(a)
      | `Different =>
        /* Js.log3("different", a, b); */
        let instances = instantiateTree(next);
        let instanceLayout = getInstanceLayout(instances);
        Layout.layout(instanceLayout);
        let tree = inflateTree(instances);
        /* unmount prev nodes */
        NativeInterface.replaceWith(getNativeNode(prev), getNativeNode(tree));
        tree
    }
  | _ =>
    let instances = instantiateTree(next);
    let instanceLayout = getInstanceLayout(instances);
    Layout.layout(instanceLayout);
    let tree = inflateTree(instances);
    /* unmount prev nodes */
    NativeInterface.replaceWith(getNativeNode(prev), getNativeNode(tree));
    tree
} and reconcileChildren = (parentNode, aChildren, bChildren) => {
  switch (aChildren, bChildren) {
    | ([], []) => []
    | ([], _) =>
      let more = bChildren->List.map(child => inflateTree(instantiateTree(child)));
      more->List.forEach(child => NativeInterface.appendChild(parentNode, getNativeNode(child)));
      more
    | (more, []) => 
      more->List.forEach(child => NativeInterface.removeChild(parentNode, getNativeNode(child)));
      []
    | ([one, ...aRest], [two, ...bRest]) =>
      [reconcileTrees(one, two), ...reconcileChildren(parentNode, aRest, bRest)]
  }
};

let mount = (el, node) => {
  let instances = instantiateTree(el);
  let instanceLayout = getInstanceLayout(instances);
  Layout.layout(instanceLayout);
  let tree = inflateTree(instances);
  node->NativeInterface.appendChild(getNativeNode(tree))
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
    /* let useHook = hooks => switch (hooks.current^) {
      | None =>
    } */

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