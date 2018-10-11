
/* also need a "compareTo" (other custom) */
type custom = {
  init: unit => customWithState,
  clone: customWithState => option(customWithState),
}
and customWithState = {
  render: unit => el
}

and el =
| String(string)
| Builtin(string, domProps, list(el))
| Custom(custom /* already contains its props & children */)

and pendingTree =
| String(string)
| Builtin(string, domProps, list(pendingTree))
| Custom(custom, pendingTree)

and container = {
  mutable custom: customWithState,
  mutable tree
}

and tree =
/* | Pending(pendingTree) */
| String(string, domNode)
| Builtin(string, domProps, domNode, list(tree))
/* maybe all of the customs have their state? assume that it's created for every create */
| Custom(container);

let rec createTree: el => pendingTree = el => switch el {
  | String(contents) => String(contents)
  | Builtin(string, domProps, children) => Builtin(string, domProps, children->List.map(createTree))
  | Custom(custom) =>
    /* How does it trigger a reconcile on setState? */
    let custom = custom.init();
    Custom(custom, createTree(custom.render()))
};

let rec inflateTree: pendingTree => tree = el => switch el {
  | String(contents) => String(contents, createTextNode(contents))
  | Builtin(string, domProps, children) =>
    let node = createElement(string, domProps);
    let children = children->List.map(inflateTree);
    children->List.forEach(child => appendChild(node, getDomNode(child)));
  | Custom(custom, tree) =>
    let tree = inflateTree(tree)
    let container = {custom, tree};
    custom.onChange(custom => {
      container.custom = custom;
      container.tree = reconcileTrees(container.tree, custom.render());
    })
    Custom(container)
};

let reconcileTrees: (tree, el) => tree = (prev, next) => switch (prev, next) {
  | (String(a, node), String(b)) =>
    if (a == b) {
      prev
    } else {
      setTextContent(node, b);
      String(b, node)
    }
  | (Builtin(a, aProps, node, aChildren), Builtin(b, bProps, bChildren)) when a == b =>
    updateDomProps(node, aProps, bProps);
    Builtin(b, bProps, node, reconcileChildren(node, aChildren, bChildren));
  | (Custom(a), Custom(b)) =>
    switch (b.clone(a.custom)) {
      | Some(custom) =>
        let tree = reconcileTrees(a.tree, custom.render());
        a.custom = custom;
        a.tree = tree;
        Custom(a)
      | None =>
        let tree = inflateTree(next);
        /* unmount prev nodes */
        replaceWith(getDomNode(prev), getDomNode(tree));
    }
  | _ =>
    let tree = inflateTree(next);
    /* unmount prev nodes */
    replaceWith(getDomNode(prev), getDomNode(tree));
    tree
} and reconcileChildren = (aChildren, bChildren) => {
  assert(false)
};

/* type  */

/*

On a component, I need to be able to:

- presumably instantiate with props
- update the props w/ new props. or return a new version that has the new props
  but only if they have the same underlying type
- render() a new tree
lifecycle methods or something

 */
