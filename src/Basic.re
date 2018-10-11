
type domProps('a) = Js.t({..} as 'a);
type opaqueProps;
type domNode;

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
  clone: customWithState => option(customWithState),
}
and customContents('props, 'state) = {
  identity: contentsSubset('props, 'state),
  props: 'props,
  state: 'state,
  render: ('props, 'state) => el,
  onChange: ('state => unit) => unit,
}
and customWithState = WithState(customContents('props, 'state)) : customWithState

and el =
| String(string)
| Builtin(string, domProps('a), list(el)): el
| Custom(custom /* already contains its props & children */)

and instanceTree =
| PString(string)
| PBuiltin(string, domProps('a), list(instanceTree)): instanceTree
| PCustom(customWithState, instanceTree)

and container = {
  mutable custom: customWithState,
  mutable mountedTree
}

and mountedTree =
/* | Pending(pendingTree) */
| TString(string, domNode)
| TBuiltin(string, domProps('a), domNode, list(mountedTree)): mountedTree
/* maybe all of the customs have their state? assume that it's created for every create */
| TCustom(container)

and contentsSubset('props, 'state) = {
  initialState: 'props => 'state,
  newStateForProps: ('props, 'state) => 'state,
  render: ('props, 'state, 'state => unit) => el,
};

module Maker = {
  let makeComponent = (maker: contentsSubset('props, 'state), props: 'props) => {
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
          Some(WithState({
            ...contents,
            props
          }))
        } else {
          None
        }
      }
    }
  };

  let component = (~render) => makeComponent({
    initialState: _props => (),
    newStateForProps: (_, state) => state,
    render: (props, (), _setState) => render(props)
  })

  let statefulComponent = (~initialState, ~newStateForProps=?, ~render) => makeComponent({
    initialState,
    newStateForProps: switch newStateForProps {
      | Some(s) => s
      | None => (_, state) => state
    },
    render,
  });

};

let render = (WithState({render, props, state})) => render(props, state);
let onChange = (WithState({onChange} as contents), handler) => onChange(state => handler(WithState({...contents, state})));

let rec getDomNode = tree => switch tree {
  | TString(_, node)
  | TBuiltin(_, _, node, _) => node
  | TCustom({mountedTree}) => getDomNode(mountedTree)
};

/*
Phases of the algorithm:

- first, there is an element tree. This doesn't have instantiated custom components
- second, that element tree is instantated into an "instanceTree"
- third, that instance tree is mounted to the DOM.

 */

let rec instantiateTree: el => instanceTree = el => switch el {
  | String(contents) => PString(contents)
  | Builtin(string, domProps, children) => PBuiltin(string, domProps, children->List.map(instantiateTree))
  | Custom(custom) =>
    /* How does it trigger a reconcile on setState? */
    let custom = custom.init();
    PCustom(custom, instantiateTree(custom->render))
};

let rec inflateTree: instanceTree => mountedTree = el => switch el {
  | PString(contents) => TString(contents, createTextNode(contents))
  | PBuiltin(string, domProps, children) =>
    let node = createElement(string, domProps);
    let children = children->List.map(inflateTree);
    children->List.forEach(child => appendChild(node, getDomNode(child)));
    TBuiltin(string, domProps, node, children);
  | PCustom(custom, instanceTree) =>
    let mountedTree = inflateTree(instanceTree)
    let container = {custom, mountedTree};
    custom->onChange(custom => {
      container.custom = custom;
      container.mountedTree = reconcileTrees(container.mountedTree, custom->render);
    })
    TCustom(container)
}

and reconcileTrees: (mountedTree, el) => mountedTree = (prev, next) => switch (prev, next) {
  | (TString(a, node), String(b)) =>
    if (a == b) {
      prev
    } else {
      setTextContent(node, b);
      TString(b, node)
    }
  | (TBuiltin(a, aProps, node, aChildren), Builtin(b, bProps, bChildren)) when a == b =>
    updateDomProps(node, aProps, bProps);
    TBuiltin(b, bProps, node, reconcileChildren(node, aChildren, bChildren));
  | (TCustom(a), Custom(b)) =>
    switch (b.clone(a.custom)) {
      | Some(custom) =>
        let tree = reconcileTrees(a.mountedTree, custom->render);
        a.custom = custom;
        a.mountedTree = tree;
        TCustom(a)
      | None =>
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
