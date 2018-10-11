
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
let updateDomProps = (node, oldProps, newProps) => {
  setDomProps(node, opaqueProps(newProps));
};
[@bs.send] external replaceWith: (domNode, domNode) => unit = "";

/* also need a "compareTo" (other custom) */
type custom = {
  init: unit => customWithState,
  clone: customWithState => option(customWithState),
}
and customContents('id, 'props, 'state) = {
  identity: 'id,
  props: 'props,
  state: 'state,
  render: ('props, 'state) => el,
  onChange: ('state => unit) => unit,
}
and customWithState = WithState(customContents('id, 'props, 'state)) : customWithState

and el =
| String(string)
| Builtin(string, domProps('a), list(el)): el
| Custom(custom /* already contains its props & children */)

and pendingTree =
| PString(string)
| PBuiltin(string, domProps('a), list(pendingTree)): pendingTree
| PCustom(customWithState, pendingTree)

and container = {
  mutable custom: customWithState,
  mutable tree
}

and tree =
/* | Pending(pendingTree) */
| TString(string, domNode)
| TBuiltin(string, domProps('a), domNode, list(tree)): tree
/* maybe all of the customs have their state? assume that it's created for every create */
| TCustom(container);

module Maker = {
  type contentsSubset('props, 'state) = {
    initialState: 'state,
    render: ('props, 'state, 'state => unit) => el,
  };
  let makeComponent = (maker, props) => {
    {
      init: () => {
        let onChange = ref(_state => ());
        let subset = maker(props);
        WithState({
          identity: maker,
          onChange: handler => onChange := handler,
          props,
          state: subset.initialState,
          render: (props, state) => subset.render(props, state, onChange^),
        })
      },
      clone: (WithState({identity} as contents)) => {
        if (Obj.magic(identity) === maker) {
          Some(WithState({
            ...Obj.magic(contents),
            props
          }))
        } else {
          None
        }
      }
    }
  };

};

let render = (WithState({render, props, state})) => render(props, state);
let onChange = (WithState({onChange} as contents), handler) => onChange(state => handler(WithState({...contents, state})));

let rec getDomNode = tree => switch tree {
  | TString(_, node)
  | TBuiltin(_, _, node, _) => node
  | TCustom({tree}) => getDomNode(tree)
};

let rec createTree: el => pendingTree = el => switch el {
  | String(contents) => PString(contents)
  | Builtin(string, domProps, children) => PBuiltin(string, domProps, children->List.map(createTree))
  | Custom(custom) =>
    /* How does it trigger a reconcile on setState? */
    let custom = custom.init();
    PCustom(custom, createTree(custom->render))
};

let rec inflateTree: pendingTree => tree = el => switch el {
  | PString(contents) => TString(contents, createTextNode(contents))
  | PBuiltin(string, domProps, children) =>
    let node = createElement(string, domProps);
    let children = children->List.map(inflateTree);
    children->List.forEach(child => appendChild(node, getDomNode(child)));
    TBuiltin(string, domProps, node, children);
  | PCustom(custom, tree) =>
    let tree = inflateTree(tree)
    let container = {custom, tree};
    custom->onChange(custom => {
      container.custom = custom;
      container.tree = reconcileTrees(container.tree, custom->render);
    })
    TCustom(container)
}

and reconcileTrees: (tree, el) => tree = (prev, next) => switch (prev, next) {
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
        let tree = reconcileTrees(a.tree, custom->render);
        a.custom = custom;
        a.tree = tree;
        TCustom(a)
      | None =>
        let tree = inflateTree(createTree(next));
        /* unmount prev nodes */
        replaceWith(getDomNode(prev), getDomNode(tree));
        tree
    }
  | _ =>
    let tree = inflateTree(createTree(next));
    /* unmount prev nodes */
    replaceWith(getDomNode(prev), getDomNode(tree));
    tree
} and reconcileChildren = (parentNode, aChildren, bChildren) => {
  switch (aChildren, bChildren) {
    | ([], []) => []
    | ([], more) =>
      let more = bChildren->List.map(child => inflateTree(createTree(child)));
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
  let tree = inflateTree(createTree(el));
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
