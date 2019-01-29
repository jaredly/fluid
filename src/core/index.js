
console.log('hello')

/*
I need to generate the full tree.
And then be able to traverse it probably?

Current minimal setup:

- updateNode(domNode, newNode, oldNode, index)
- if not there, createElement (just does the dom create element)
- else if same type, update props & recurse children
- else replace old one with new one, creating new dom node

With lifecycle methods
> didMount
> didUpdate
> willReceiveProps (return a new state maybe)
> willUnmount

as I'm going through, I need to be able to collect the ones that will be removed and start the animation, and then go through and commit the result.

So I'll need a "realized tree".

{
  el: {type, props, children},
  inst,
  domNode, // might be absent, if not set up yet probably
  state,
  children
}


(and I'll probably want a "keys map")

And then I'll be able to go in and make changes.
probably.


 */

function h(type, props, ...children) {
  return { type, props, children };
}

const createTree = (el, keyMap) => {
  if (!el) return
  if (typeof el === 'string') {
    return {el, domNode: null}
  } else if (typeof el.type === 'string') {
    const node = {
      el,
      domNode: null,
      children: el.children.map(child => createTree(child, keyMap))
    };
    if (el.props.key) {
      keyMap[el.props.key] = node
    }
    return node
  } else {
    const inst = el.type(el.props, el.children)
    const state = inst.getInitialState ? inst.getInitialState() : null;
    const node = {el, state, inst, domNode: null, tree: null};
    if (el.props.key) {
      keyMap[el.props.key] = node
    }
    const setState = (newState) => {
      const oldState = node.state;
      node.state = newState;
      const childEl = inst.render(el.props, el.children, state, setState)
      const oldTree = node.tree
      // shouldComponentUpdate here
      node.tree = createTree(childEl, keyMap);
      reconcileTree(oldTree, node.tree)
      inst.didUpdate(oldState, newState)
    };
    const childEl = inst.render(el.props, el.children, state, setState)
    node.tree = createTree(childEl, keyMap);
    return node
  }
};

const reconcileTree = (oldTree, newTree) => {
  if (oldTree.el === newTree.el) {
    newTree.domNode = oldTree.domNode
    return
  }
};

const inflateTree = (tree) => {
  if (!tree) return
  if (typeof tree.el === 'string') {
    const node = document.createTextNode(tree.el)
    tree.domNode = node
  } else if (typeof tree.el.type === 'string') {
    const node = document.createElement(tree.el.type);
    tree.domNode = node;
    tree.children.forEach(inflateTree);
  } else {
    inflateTree(tree.tree)
    tree.domNode = tree.tree.domNode
    tree.inst.didMount(tree.el.props, tree.state)
  }
};

const mount = (el, domNode) => {
  const keyMap = {}
  const tree = createTree(el, keyMap);
  inflateTree(tree)
  return tree.domNode
};