
type domProps('a) = Js.t({..} as 'a);
type opaqueProps;
type domNode;

[@bs.obj] external domProps: (
  ~id:string=?,
  ~_type:string=?,
  ~onchange:Dom.event => unit=?,
  ~oninput:Dom.event => unit=?,
  ~width:int=?,
  ~height:int=?,
  ~onclick:Dom.event => unit=?,
  ~style:string=?,
  unit
  ) => domProps({..}) = "";

external opaqueProps: domProps('a) => opaqueProps = "%identity";
[@bs.scope "document"][@bs.val] external createTextNode: string => domNode = "";
[@bs.set] external setTextContent: (domNode, string) => unit = "textContent";
[@bs.scope "document"][@bs.val] external _createElement: string => domNode = "createElement";
let setDomProps: (domNode, opaqueProps) => unit = [%bs.raw (node, props) => {|
  Object.keys(props).forEach(key => {
    if (key === 'checked' || key === 'value') {
      node[key] = props[key]
    } else if (typeof props[key] === 'function') {
      node[key] = props[key]
    } else {
      node.setAttribute(key, props[key])
    }
  })
|}];
let createElement = (typ, domProps) => {
  let node = _createElement(typ);
  setDomProps(node, opaqueProps(domProps));
  /* TODO set props */
  node;
};
[@bs.get] external parentNode: domNode => domNode = "";
[@bs.send] external appendChild: (domNode, domNode) => unit = "";
[@bs.send] external insertBefore: (domNode, domNode, ~reference: domNode) => unit = "";
[@bs.send] external removeChild: (domNode, domNode) => unit = "";
let updateDomProps = (node, _oldProps, newProps) => {
  setDomProps(node, opaqueProps(newProps));
};
[@bs.send] external replaceWith: (domNode, domNode) => unit = "";

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
  render: context('hooks, 'reconcileData) => element,
  mutable hooks: option('hooks),
  mutable invalidated: bool,
  mutable reconciler: option(
    (
'reconcileData, option(('reconcileData, reconcilerFunction('reconcileData)))
    )
  ),
  mutable onChange: unit => unit,
}

and hooksContainer('hooks, 'reconcileData) = {
  invalidate: unit => unit,
  setReconciler: ('reconcileData, reconcilerFunction('reconcileData)) => unit,
  triggerEffect:
    (
      ~cleanup: option(unit => unit),
      ~fn: (unit, unit) => unit,
      ~setCleanup: (unit => unit) => unit
    ) =>
    unit,
  current: option('hooks),
}

and reconcilerFunction('data) = ('data, 'data, mountedTree, element) => mountedTree
and customWithState = WithState(customContents('identity, 'hooks, 'reconcileData)) : customWithState

and element =
| String(string)
| Builtin(string, domProps('a), list(element)): element
| Custom(custom /* already contains its props & children */)

and instanceTree =
| IString(string)
| IBuiltin(string, domProps('a), list(instanceTree)): instanceTree
| ICustom(customWithState, instanceTree, list(effect))

and container = {
  mutable custom: customWithState,
  mutable mountedTree
}

and mountedTree =
| MString(string, domNode)
| MBuiltin(string, domProps('a), domNode, list(mountedTree)): mountedTree
| MCustom(container)

and context('initial, 'reconcile) = {
  hooks: hooksContainer('initial, 'reconcile),
  finish: hooksContainer('initial, 'reconcile) => unit
}

;

/* let defaultConfig = {
  name: "Unnamed",
  initialState: () => (),
  queuedEffects: [],
  newStateForProps: None,
  reconcileTrees: None,
  render: ((), (), _setState) => String("Hello")
}; */

module Maker = {
  let makeComponent = (identity: 'identity, render: context('state, 'reconcile) => element) => {
    {
      init: () => {
        WithState({
          identity,
          invalidated: false,
          onChange: () => (),
          reconciler: None,
          hooks: None,
          render,
        })
      },
      clone: (WithState(contents)) => {
        /* If the `identity` is strictly equal, then we know that the types must be the same. */
        if (Obj.magic(contents.identity) === identity) {
          let contents: customContents('props, 'state, 'reconcileData) = Obj.magic(contents);
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

  /* let component = (~name, ~reconcileTrees=?, ~render, ()) => makeComponent({
    name,
    initialState: _props => (),
    newStateForProps: None,
    reconcileTrees,
    render: (props, (), _setState) => render(props)
  });

  let statefulComponent = (~name, ~initialState, ~reconcileTrees=?, ~newStateForProps=?, ~render, ()) => makeComponent({
    name,
    initialState,
    newStateForProps,
    reconcileTrees,
    render,
  });

  let rec recursiveComponent = (inner, props) => inner((recursiveComponent(inner), props)); */

};

let render = (WithState(component)) => {
  let effects = ref([]);
  let context = {
    hooks: {
      invalidate: () => {
        component.invalidated = true;
        component.onChange();
        /* TODO actually trigger a rerender here */
      },
      setReconciler: (data, reconcile) => component.reconciler = switch (component.reconciler) {
        | None => Some((data, None))
        | Some((old, None))
        | Some((_, Some((old, _)))) => Some((old, Some((data, reconcile))))
      },
      triggerEffect: (~cleanup, ~fn, ~setCleanup) => {
        effects.contents = [{cleanup, fn, setCleanup}, ...effects.contents];
      },
      current: component.hooks,
    },
    finish: v => component.hooks = v.current
  };
  component.invalidated = false;
  let tree = component.render(context);
  (tree, effects^);
};

let rec getDomNode = tree => switch tree {
  | MString(_, node)
  | MBuiltin(_, _, node, _) => node
  | MCustom({mountedTree}) => getDomNode(mountedTree)
};

/*
Phases of the algorithm:

- first, there is an element tree. This doesn't have instantiated custom components
- second, that element tree is instantated into an "instanceTree"
- third, that instance tree is mounted to the DOM.

 */

let rec instantiateTree: element => instanceTree = el => switch el {
  | String(contents) => IString(contents)
  | Builtin(string, domProps, children) => IBuiltin(string, domProps, children->List.map(instantiateTree))
  | Custom(custom) =>
    /* How does it trigger a reconcile on setState? */
    let custom = custom.init();
    let (tree, effects) = custom->render;
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
  | IString(contents) => MString(contents, createTextNode(contents))
  | IBuiltin(string, domProps, children) =>
    let node = createElement(string, domProps);
    let children = children->List.map(inflateTree);
    children->List.forEach(child => appendChild(node, getDomNode(child)));
    MBuiltin(string, domProps, node, children);
  | ICustom(custom, instanceTree, effects) =>
    let mountedTree = inflateTree(instanceTree)
    let container = {custom, mountedTree};
    custom->listenForChanges(container);
    effects->List.forEach(runEffect);
    MCustom(container)
}

and listenForChanges = (WithState(contents) as component, container) => {
  contents.onChange = () => {
    let (newElement, effects) = component->render;
    container.mountedTree = switch (contents.reconciler) {
      | Some((oldData, Some((newData, reconcile)))) => reconcile(oldData, newData, container.mountedTree, newElement)
      | _ => reconcileTrees(container.mountedTree, newElement)
    };
    effects->List.forEach(runEffect);
  }
}

and reconcileTrees: (mountedTree, element) => mountedTree = (prev, next) => switch (prev, next) {
  | (MString(a, node), String(b)) =>
    if (a == b) {
      prev
    } else {
      setTextContent(node, b);
      MString(b, node)
    }
  | (MBuiltin(a, aProps, node, aChildren), Builtin(b, bProps, bChildren)) when a == b =>
    updateDomProps(node, aProps, bProps);
    MBuiltin(b, bProps, node, reconcileChildren(node, aChildren, bChildren));
  | (MCustom(a), Custom(b)) =>
    switch (b.clone(a.custom)) {
      | `Same => MCustom(a)
      | `Compatible(custom) =>
        let (newElement, effects) = custom->render;
        let tree = reconcileTrees(a.mountedTree, newElement);
        a.custom = custom;
        a.mountedTree = tree;
        effects->List.forEach(runEffect);
        MCustom(a)
      | `Different =>
        /* Js.log3("different", a, b); */
        let tree = inflateTree(instantiateTree(next));
        /* unmount prev nodes */
        replaceWith(getDomNode(prev), getDomNode(tree));
        tree
    }
  | _ =>
    let tree = inflateTree(instantiateTree(next));
    /* unmount prev nodes */
    replaceWith(getDomNode(prev), getDomNode(tree));
    tree
} and reconcileChildren = (parentNode, aChildren, bChildren) => {
  switch (aChildren, bChildren) {
    | ([], []) => []
    | ([], _) =>
      let more = bChildren->List.map(child => inflateTree(instantiateTree(child)));
      more->List.forEach(child => appendChild(parentNode, getDomNode(child)));
      more
    | (more, []) => 
      more->List.forEach(child => removeChild(parentNode, getDomNode(child)));
      []
    | ([one, ...aRest], [two, ...bRest]) =>
      [reconcileTrees(one, two), ...reconcileChildren(parentNode, aRest, bRest)]
  }
};

let mount = (el, node) => {
  let tree = inflateTree(instantiateTree(el));
  node->appendChild(getDomNode(tree))
};

/*

On a component, I need to be able to:

- presumably instantiate with props
- update the props w/ new props. or return a new version that has the new props
  but only if they have the same underlying type
- render() a new tree
lifecycle methods or something

 */
