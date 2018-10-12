
type domProps('a) = Js.t({..} as 'a);
type opaqueProps;
type domNode;

[@bs.obj] external domProps: (
  ~id:string=?,
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
[@bs.send] external appendChild: (domNode, domNode) => unit = "";
[@bs.send] external removeChild: (domNode, domNode) => unit = "";
let updateDomProps = (node, _oldProps, newProps) => {
  setDomProps(node, opaqueProps(newProps));
};
[@bs.send] external replaceWith: (domNode, domNode) => unit = "";

/* also need a "compareTo" (other custom) */
type custom = {
  init: unit => customWithState,
  clone: customWithState => [`Different | `Same | `Compatible(customWithState)],
}
and customContents('props, 'state) = {
  identity: customConfig('props, 'state),
  props: 'props,
  state: 'state,
  render: ('props, 'state) => element,
  onChange: ('state => unit) => unit,
}
and customWithState = WithState(customContents('props, 'state)) : customWithState

and element =
| String(string)
| Builtin(string, domProps('a), list(element)): element
| Custom(custom /* already contains its props & children */)

and instanceTree =
| IString(string)
| IBuiltin(string, domProps('a), list(instanceTree)): instanceTree
| ICustom(customWithState, instanceTree)

and container = {
  mutable custom: customWithState,
  mutable mountedTree
}

and mountedTree =
| MString(string, domNode)
| MBuiltin(string, domProps('a), domNode, list(mountedTree)): mountedTree
| MCustom(container)

and customConfig('props, 'state) = {
  initialState: 'props => 'state,
  newStateForProps: option(('props, 'state) => 'state),
  render: ('props, 'state, 'state => unit) => element,
};

let defaultConfig = {
  initialState: () => (),
  newStateForProps: None,
  render: ((), (), _setState) => String("Hello")
};

module Maker = {
  let makeComponent = (maker: customConfig('props, 'state), props: 'props) => {
    {
      init: () => {
        let onChange = ref(_state => Js.log("SetState before render ignored"));
        Js.log2("Mount!", maker);
        WithState({
          identity: maker,
          onChange: handler => onChange := handler,
          props,
          state: maker.initialState(props),
          render: (props, state) => maker.render(props, state, state => onChange^(state)),
        })
      },
      clone: (WithState({identity} as contents)) => {
        /* If the `identity` is strictly equal, then we know that the types must be the same. */
        if (Obj.magic(identity) === maker) {
          let contents: customContents('props, 'state) = Obj.magic(contents);
          if (contents.props === props) {
            `Same
          } else {
            `Compatible(WithState({
              ...contents,
              state: switch (maker.newStateForProps) {
                | None => contents.state
                | Some(fn) => fn(props, contents.state)
              },
              props
            }))
          }
        } else {
          `Different
        }
      }
    }
  };

  let component = (~render) => makeComponent({
    initialState: _props => (),
    newStateForProps: None,
    render: (props, (), _setState) => render(props)
  });

  let statefulComponent = (~initialState, ~newStateForProps=?, ~render) => makeComponent({
    initialState,
    newStateForProps,
    render,
  });

  let rec recursiveComponent = (inner, props) => inner((recursiveComponent(inner), props));

};

let render = (WithState({render, props, state})) => render(props, state);
let onChange = (WithState({onChange} as contents), handler) => onChange(state => handler(WithState({...contents, state})));

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
    ICustom(custom, instantiateTree(custom->render))
};

let rec inflateTree: instanceTree => mountedTree = el => switch el {
  | IString(contents) => MString(contents, createTextNode(contents))
  | IBuiltin(string, domProps, children) =>
    let node = createElement(string, domProps);
    let children = children->List.map(inflateTree);
    children->List.forEach(child => appendChild(node, getDomNode(child)));
    MBuiltin(string, domProps, node, children);
  | ICustom(custom, instanceTree) =>
    let mountedTree = inflateTree(instanceTree)
    let container = {custom, mountedTree};
    custom->onChange(custom => {
      container.custom = custom;
      container.mountedTree = reconcileTrees(container.mountedTree, custom->render);
    })
    MCustom(container)
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
        let tree = reconcileTrees(a.mountedTree, custom->render);
        a.custom = custom;
        a.mountedTree = tree;
        MCustom(a)
      | `Different =>
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
