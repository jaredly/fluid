
function h(type, props, ...children) {
  return { type, props, children };
}

function createElement(node) {
  if (typeof node === 'string') {
    return document.createTextNode(node);
  }
  if (typeof node.type === 'string') {
    const $el = document.createElement(node.type);
    Object.keys(node.props).forEach(attr => {
      if (attr.startsWith('on')) {
        $el[attr] = node.props[attr]
      } else {
        $el.setAttribute(attr, node.props[attr])
      }
    });
    node.children.map(createElement).forEach(child => $el.appendChild(child));
    return $el;
  } else {
    const inst = node.type(node.props, node.children)
    if (inst.render) {
      let state = inst.getInitialState ? inst.getInitialState() : null;
      let node;
      const setState = (newState) => {
        node.replaceWith(inst.render(node.props, state, setState))
      }
      node = createElement(inst.render(node.props, state, setState))
    } else {
      return createElement(inst)

    }
  }
}

function changed(node1, node2) {
  return typeof node1 !== typeof node2 ||
         typeof node1 === 'string' && node1 !== node2 ||
         node1.type !== node2.type
}

function updateElement($parent, newNode, oldNode, index = 0) {
  if (!oldNode) {
    $parent.appendChild(
      createElement(newNode)
    );
  } else if (!newNode) {
    $parent.removeChild(
      $parent.childNodes[index]
    );
  } else if (changed(newNode, oldNode)) {
    $parent.replaceChild(
      createElement(newNode),
      $parent.childNodes[index]
    );
  } else if (newNode.type) {
    const newLength = newNode.children.length;
    const oldLength = oldNode.children.length;
    for (let i = 0; i < newLength || i < oldLength; i++) {
      updateElement(
        $parent.childNodes[index],
        newNode.children[i],
        oldNode.children[i],
        i
      );
    }
  }
}

const mount = (el, domNode) => {
  
};


const $root = document.getElementById('root');
const $reload = document.getElementById('reload');

updateElement($root, a);
$reload.addEventListener('click', () => {
  updateElement($root, b, a);
});
