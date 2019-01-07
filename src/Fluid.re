
module type NativeInterface = (Config: { type nativePropsFn; }) => {
  type element;
  type nativeNode;

  /* If it returns true, then the elements were of the same type and the mounted node was updated.
  If false, they weren't of the same type and no update happened.
   */
  let maybeUpdate: (~mounted: element, ~mountPoint: nativeNode, ~newElement: element) => bool;

  let inflate: element => nativeNode;

  let nativeProps: Config.nativePropsFn;
  let createTextNode: string => nativeNode;
  let setTextContent: (nativeNode, string) => unit;
};

module NativeInterface = {
  type nativeProps;

  type nativeNode;


  [@bs.obj] external nativeProps: (
    ~id:string=?,
    ~_type:string=?,
    ~onchange:Dom.event => unit=?,
    ~oninput:Dom.event => unit=?,
    ~width:int=?,
    ~height:int=?,
    ~onclick:Dom.event => unit=?,
    ~style:string=?,
    unit
    ) => nativeProps = "";

  /* external opaqueProps: domProps('a) => nativeProps = "%identity"; */
  [@bs.scope "document"][@bs.val] external _createElement: string => nativeNode = "createElement";
  let setDomProps: (nativeNode, nativeProps) => unit = [%bs.raw (node, props) => {|
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
  let createElement = (typ, nativeProps) => {
    let node = _createElement(typ);
    setDomProps(node, nativeProps);
    node;
  };

  [@bs.scope "document"][@bs.val] external createTextNode: string => nativeNode = "";
  [@bs.set] external setTextContent: (nativeNode, string) => unit = "textContent";
  [@bs.get] external parentNode: nativeNode => nativeNode = "";
  [@bs.send] external appendChild: (nativeNode, nativeNode) => unit = "";
  [@bs.send] external insertBefore: (nativeNode, nativeNode, ~reference: nativeNode) => unit = "";
  [@bs.send] external removeChild: (nativeNode, nativeNode) => unit = "";
  let updateNativeProps = (node, _oldProps, newProps) => {
    setDomProps(node, newProps);
  };
  [@bs.send] external replaceWith: (nativeNode, nativeNode) => unit = "";


  type element = {tag: string, props: nativeProps};
  let maybeUpdate = (~mounted, ~mountPoint, ~newElement) => {
    if (mounted.tag == newElement.tag) {
      updateNativeProps(mountPoint, mounted.props, newElement.props);
      true
    } else {
      false
    }
  };
  let inflate = ({tag, props}) => createElement(tag, props);


}

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
  current: option('hooks),
}

and reconcilerFunction('data) = ('data, 'data, mountedTree, element) => mountedTree
and customWithState = WithState(customContents('identity, 'hooks, 'reconcileData)) : customWithState

and element =
| String(string)
| Builtin(string, NativeInterface.nativeProps, list(element)): element
| Custom(custom /* already contains its props & children */)

and instanceTree =
| IString(string)
| IBuiltin(string, NativeInterface.nativeProps, list(instanceTree)): instanceTree
| ICustom(customWithState, instanceTree, list(effect))

and container = {
  mutable custom: customWithState,
  mutable mountedTree
}

and mountedTree =
| MString(string, NativeInterface.nativeNode)
| MBuiltin(string, NativeInterface.nativeProps, NativeInterface.nativeNode, list(mountedTree)): mountedTree
| MCustom(container)

and context('initial, 'reconcile) = {
  hooks: hooksContainer('initial, 'reconcile),
  finish: hooksContainer('initial, 'reconcile) => unit
}

;

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
      setReconciler: (oldData, data, reconcile) => component.reconciler = Some((oldData, data, reconcile)),
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

let rec getNativeNode = tree => switch tree {
  | MString(_, node)
  | MBuiltin(_, _, node, _) => node
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

let rec instantiateTree: element => instanceTree = el => switch el {
  | String(contents) => IString(contents)
  | Builtin(string, nativeProps, children) => IBuiltin(string, nativeProps, children->List.map(instantiateTree))
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
  | IString(contents) => MString(contents, NativeInterface.createTextNode(contents))
  | IBuiltin(string, nativeProps, children) =>
    let node = NativeInterface.createElement(string, nativeProps);
    let children = children->List.map(inflateTree);
    children->List.forEach(child => NativeInterface.appendChild(node, getNativeNode(child)));
    MBuiltin(string, nativeProps, node, children);
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
      | Some((oldData, newData, reconcile)) => reconcile(oldData, newData, container.mountedTree, newElement)
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
      NativeInterface.setTextContent(node, b);
      MString(b, node)
    }
  | (MBuiltin(a, aProps, node, aChildren), Builtin(b, bProps, bChildren)) when a == b =>
    NativeInterface.updateNativeProps(node, aProps, bProps);
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
        NativeInterface.replaceWith(getNativeNode(prev), getNativeNode(tree));
        tree
    }
  | _ =>
    let tree = inflateTree(instantiateTree(next));
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
  let tree = inflateTree(instantiateTree(el));
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
