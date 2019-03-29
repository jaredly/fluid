open Belt;

module Layout = Layout;

module type NativeInterface = {
  type element;
  type nativeNode;
  type font;

  let setImmediate: (unit => unit) => unit;


  /* If it returns true, then the elements were of the same type and the mounted node was updated.
  If false, they weren't of the same type and no update happened.
   */
  let canUpdate: (~mounted: element, ~mountPoint: nativeNode, ~newElement: element) => bool;

  let update: (element, nativeNode, element, Layout.node) => unit;
  let updateLayout: (element, nativeNode, Layout.node) => unit;

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

type async('t) = ('t => unit) => unit;

type suspendReason = ..;

type suspendReason += NoReason;
type suspendReason += LoadingImage(string);


type suspendEvent = {
  reason: suspendReason,
  payload: async(unit)
};

exception SuspendException(suspendEvent);
exception StillSuspended;


type instantiateResult('contents) =
  | Good('contents)
  | Suspense(list(suspendEvent))
  | Bad(exn)


/* also need a "compareTo" (other custom) */
type custom = {
  init: unit => customWithState,
  clone: customWithState => [`Different | `Same | `Compatible(customWithState)],
}

and customContents('identity, 'hooks, 'reconcileData) = {
  identity: 'identity,
  render: hooksContainer('hooks, 'reconcileData) => element,
  hooks: ref('hooks),
  /* mutable suspense: option() */
  mutable invalidated: bool,
  mutable suspense: option(ref(list(suspendEvent))),
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
  setSuspense: unit => list(suspendReason),
  triggerEffect:
    (
      ~cleanup: option(unit => unit),
      ~fn: (unit, unit) => unit,
      ~setCleanup: (unit => unit) => unit
    ) =>
    unit,
  current: ref('hooks),
}

and reconcilerFunction('data) = ('data, 'data, mountedTree, element) => instantiateResult(pendingTree)
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
          onChange: () => {
            print_endline("Ignoring onChange, happened too early")
          },
          reconciler: None,
          suspense: None,
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

let mapResult = (r, fn) => switch r {
  | Good(r) => Good(fn(r))
  | Suspense(s) => Suspense(s)
  | Bad(exn) => Bad(exn)
};
let bindResult = (r, fn) => switch r {
  | Good(r) => fn(r)
  | Suspense(s) => Suspense(s)
  | Bad(exn) => Bad(exn)
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
    setSuspense: () => {
      switch (component.suspense) {
        | None => component.suspense = Some(ref([])); []
        | Some({contents: items}) => items->List.map(evt => evt.reason)
      }
      /* ->List.keepMap(event => filter(event.reason) ? Some(event.reason) : None) */
    },
    triggerEffect: (~cleanup, ~fn, ~setCleanup) => {
      effects.contents = [{cleanup, fn, setCleanup}, ...effects.contents];
    },
    current: component.hooks,
  };
  component.invalidated = false;
  try {
    let tree = component.render(hooks);
    Good((tree, effects^));
  } {
    | SuspendException(evt) => Suspense([evt])
    | exn =>
      print_endline("Error rending component: " ++ Printexc.to_string(exn));
      Bad(exn)
  }
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

let updateLayout = (layout: option(Layout.node), children, style, measure) => {
  switch layout {
    | None => 
    switch measure { 
      | None => Layout.createNode(children, style)
      | Some(m) => Layout.createNodeWithMeasure(children, style, m)
    }
    | Some(layout) =>
      layout.style = style;
      layout.children = children;
      layout.childrenCount = Array.length(children);
      layout.measure = measure;
      layout.layout = Layout.LayoutSupport.createLayout();
      Layout.Layout.invalidateCache(layout.layout);
      layout
  }
  /* TODO free current children? but maybe I can't because they're reused */
};

let rec instantiateTree = (~withLayout=?, el: element) => switch el {
  | Null => Good(INull(updateLayout(withLayout, [||], Layout.style(), None)))

  | Builtin(nativeElement, children, layout, measure) =>
    let ichildren = children->List.reduce(Good([]), (current, child) => {
      switch current {
        | Bad(exn) => Bad(exn)
        | Good(children) => instantiateTree(child)->mapResult(child => [child, ...children])
        | Suspense(s) => switch (instantiateTree(child)) {
          | Good(_) => Suspense(s)
          | Bad(exn) => Bad(exn)
          | Suspense(s2) => Suspense(s @ s2)
        }
      }
    });

    ichildren->mapResult(children => {
      /* Js.log2("instantiated children", children); */
        let children = children->List.reverse;
        let childLayouts = children->List.map(getInstanceLayout)->List.toArray;
        let style = switch layout {
          | None => Layout.style()
          | Some(s) => s
        };
        IBuiltin(nativeElement, children, updateLayout(withLayout, childLayouts, style, measure))
    })

  | Custom(custom) =>
    /* How does it trigger a reconcile on setState? */
    let custom = custom.init();
    switch (custom->runRender) {
      | Bad(exn) => Bad(exn)
      | Suspense(s) => Suspense(s)
      | Good((tree, effects)) =>
        switch (instantiateTree(tree)) {
          | Good(tree) => Good(ICustom(custom, tree, effects))
          | Bad(exn) => Bad(exn)
          | Suspense(s) =>
            let WithState({suspense} as inner) = custom;
            switch (suspense) {
              | None => Suspense(s)
              | Some(holder) =>
                holder.contents = s;
                switch (custom->runRender) {
                  | Bad(exn) => Bad(exn)
                  | Suspense(s) => Suspense(s)
                  | Good((tree, effects)) =>
                    switch (instantiateTree(tree)) {
                      | Good(tree) =>
                        s->List.forEach(item => {
                          item.payload(() => {
                            print_endline("Got a suspend result");
                            holder.contents = holder.contents->List.keep(k => k !== item);
                            /* Js.log(holder.contents); */
                            inner.invalidated = true;
                            inner.onChange();
                          })
                        });
                        Good(ICustom(custom, tree, effects))
                      | Bad(exn) => Bad(exn)
                      | Suspense(s) => Suspense(s)
                    }
                }
            }
        }
    }
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
  mutable node: option((mountedTree, NativeInterface.nativeNode)),
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
    NativeInterface.update(prevNative, node, native, layout);
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
      /* Js.log2("Creating", node); */
      mountTo(mount, node);
      | Replace(prev) =>
      /* Js.log3("Replacing", prev, node) */
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

let rec reconcileTrees: (container => unit, mountedTree, element) => instantiateResult(pendingTree) = (enqueue, prev, next) => switch (prev, next) {
  | (MBuiltin(aElement, node, aChildren, aLayout), Builtin(bElement, bChildren, bLayoutStyle, bMeasure)) =>
    /* TODO maybe it should return... the things... ok I need a prev el or sth */
    /* TODO re-enable updating */
    if (NativeInterface.canUpdate(~mounted= aElement, ~mountPoint=node, ~newElement=bElement)) {
      reconcileChildren(enqueue, node, aChildren, bChildren)->mapResult(children => {
        updateLayout(
          Some(aLayout),
          children->List.map(getPendingLayout)->List.toArray,
          switch (bLayoutStyle) {
          | Some(s) => s
          | _ => Layout.style()
          },
          bMeasure,
        )
        ->ignore;
        PBuiltin(bElement, Update(aElement, node), children, aLayout)
      })
    } else {
      let instances = instantiateTree(~withLayout=aLayout, next);
      instances->mapResult(instances => pendingReplace(node, instances))
    }
  | (MCustom(a), Custom(b)) =>
    switch (b.clone(a.custom)) {
      | `Same => Good(PCustom(a, []))
      | `Compatible(custom) =>
        custom->runRender->bindResult(((newElement, effects)) => {
          /* TODO custom reconciler */
          switch (a.mountedTree) {
            | Pending(_) => failwith("Reconciling a componenet that's still pending.")
            | Mounted(mountedTree) =>
              let tree = reconcileTrees(enqueue, mountedTree, newElement);
              tree->mapResult(tree => PCustom({custom, mountedTree: Pending(tree)}, effects))
          }
        })
      | `Different =>
        let instances = instantiateTree(next);
        instances->mapResult(instances => switch (getNativeNode(prev)) {
        | None =>
          print_endline("Warning! Prev custom component was pending");
          makePending(instances);
        | Some(node) => pendingReplace(node, instances)
        });
    }
  | _ =>
    let instances = instantiateTree(next);
    instances->mapResult(instances => switch (getNativeNode(prev)) {
    | None =>
      print_endline("Warning! Prev custom component was pending");
      makePending(instances);
    | Some(node) => pendingReplace(node, instances)
    });
} and reconcileChildren = (enqueue, parentNode, aChildren, bChildren) => {
  switch (aChildren, bChildren) {
    | ([], []) => Good([])
    | ([], _) =>
      let ichildren = bChildren->List.reduce(Good([]), (current, child) => {
        switch current {
          | Bad(exn) => Bad(exn)
          | Good(children) => instantiateTree(child)->mapResult(child => [child, ...children])
          | Suspense(s) => switch (instantiateTree(child)) {
            | Good(_) => Suspense(s)
            | Bad(exn) => Bad(exn)
            | Suspense(s2) => Suspense(s @ s2)
          }
        }
      });
      ichildren->mapResult(children => {
        /* Js.log2("Got extra children", children->List.toArray); */
        children->List.mapReverse(makePending);
      })
    | (more, []) => 
      /* TODO is this the right place for that? */
      more->List.keepMap(getNativeNode)->List.forEach(NativeInterface.removeChild(parentNode));
      Good([])
    | ([one, ...aRest], [two, ...bRest]) =>
      reconcileTrees(enqueue, one, two)
      ->bindResult(child =>
          reconcileChildren(enqueue, parentNode, aRest, bRest)
          ->mapResult(children => [child, ...children])
        );
  }
};

/*
Allow things like event handlers to do something like
Fluid.enqueueRenders(() => {
  onClick()
})
so that we can debounce all of them & then run them at the end.
so enqueueRenders(()) (or gatherRenders or something)

  enqueueRenders = fn => {
    root.enqueuing = true
    fn();
    flushQueue(root);
  }

  enqueu = (root, custom) => {
    if (!root.enqueueing) {
      // do the rerender
    } else {
      root.queue->add(custom)
    }
  }
 */

let rec enqueue = (~onUpdate, root, custom) => {
  root.invalidatedElements = [custom, ...root.invalidatedElements];
  if (!root.waiting) {
    root.waiting = true;
    NativeInterface.setImmediate(() => {
      root.waiting = false;
      let elements = root.invalidatedElements;
      root.invalidatedElements = [];
      let toUpdate = elements->List.keepMap(({custom: WithState(contents) as component} as container) => {
        if (contents.invalidated)  {
          switch (container.mountedTree) {
            | Pending(_) =>
              print_endline("Updating a pending tree...")
              None
            | Mounted(mountedTree) => 
              Some((container, component->runRender->bindResult(((newElement, effects)) => {
                let pending = switch (contents.reconciler) {
                  | Some((oldData, newData, reconcile)) => reconcile(oldData, newData, mountedTree, newElement)
                  | _ => reconcileTrees(enqueue(~onUpdate, root), mountedTree, newElement)
                };
                let pending = switch pending {
                  | Suspense(s) => switch (contents.suspense) {
                    | None => failwith("Top of the line")
                    | Some({contents: []} as holder) =>
                      holder.contents = s;
                      /* TODO dedup this logic with the instantiate stuff */
                      component->runRender->bindResult(((newElement, newEffects)) => {
                        let pending = switch (contents.reconciler) {
                          | Some((oldData, newData, reconcile)) => reconcile(oldData, newData, mountedTree, newElement)
                          | _ => reconcileTrees(enqueue(~onUpdate, root), mountedTree, newElement)
                        };
                        pending->mapResult(pending => (pending, newEffects))
                      })
                    | Some(_) =>
                      print_endline("Suspended component rendering things that suspended again");
                      Suspense(s)
                  }
                  | Good(pending) => Good((pending, []))
                  | Bad(exn) => Bad(exn)
                };
                /* Although, should I really be persisting the original effects? idk */
                pending->mapResult(((pending, moreEffects)) => (pending, effects @ moreEffects))
              })))
          }
        } else {
          None
        }
      });
      Layout.Layout.gCurrentGenerationCount := Layout.Layout.gCurrentGenerationCount^ + 1;
      Layout.Layout.invalidateAllCaches(root.layout);
      /* TODO if something absolutely positioned, only need to do it from there */
      /* TODO if I hook up layout node's `parent` attributes, I can walk up the tree
         instead of invalidating at the root every time. */
      Layout.layout(root.layout);
      /* [%bs.debugger]; */
      toUpdate->List.forEach(((container, result)) => {
        switch (result) {
          | Good((pending, effects)) =>
            effects->List.forEach(runEffect);
            let current = switch (getNativePending(container.mountedTree)) {
              | Some(mounted) => mounted
              | None => failwith("Current is already pending")
            };
            let newTree = mountPending(enqueue(~onUpdate, root), AppendChild(current), pending);
            container.mountedTree = Mounted(newTree);
            let rec crawl = el => switch el {
              | MNull(_) => ()
              | MBuiltin(el, mounted, children, layout) =>
                /* Js.log3("Updating layout now", mounted, layout.layout); */
                NativeInterface.updateLayout(el, mounted, layout);
                children->List.forEach(crawl)
              | MCustom({custom, mountedTree: Mounted(tree)}) => crawl(tree)
              | MCustom({custom, mountedTree: Pending(_)}) => ()
            };
            switch (root.node) {
              | None => ()
              | Some((tree, _)) => crawl(tree)
            }
          | Bad(exn) => raise(exn)
          | Suspense(s) =>
            /* TODO have a `onSuspense` on the container or component object to call here */
            failwith("Top of the line")
        }
      });
      onUpdate(root.layout.layout)
      /* STOPSHIP go down through the tree & update any nodes that have had their layout updated,
         that didn't just get rerendered. yknow. */
    })
  };
};

/*
TODO what if thre's a setState right after/during the first render?
 */

let mount = (el, node) => {
  let instances = switch (instantiateTree(el)) {
    | Bad(exn) => raise(exn)
    | Suspense(s) => failwith("useSuspense called, but no useSuspenseHandler in the tree")
    | Good(i) => i
  };
  let instanceLayout = getInstanceLayout(instances);
  Layout.layout(instanceLayout);

  let root = {
    layout: instanceLayout,
    node: None,
    invalidatedElements: [],
    waiting: false
  };


  let tree = mountPending(enqueue(~onUpdate=(_) => (), root), AppendChild(node), makePending(instances));
  switch (getNativeNode(tree)) {
    | None => failwith("Still pending?")
    | Some(childNode) =>
      root.node = Some((tree, childNode));
      node->NativeInterface.appendChild(childNode)
  };
};

let preMount = (el, onUpdate, makeNative) => {
  let instances = switch (instantiateTree(el)) {
    | Bad(exn) => raise(exn)
    | Suspense(s) => failwith("useSuspense called, no handler found")
    | Good(i) => i
  };
  let instanceLayout = getInstanceLayout(instances);
  Layout.layout(instanceLayout);
  let root = {
    layout: instanceLayout,
    node: None,
    invalidatedElements: [],
    waiting: false
  };

  let {Layout.LayoutTypes.width, height} = instanceLayout.layout;
  makeNative(~size=(width, height), node => {
    print_endline("Mounting now");
    let tree = mountPending(enqueue(~onUpdate, root), AppendChild(node), makePending(instances));
    print_endline("Mounted");
    switch (getNativeNode(tree)) {
      | None => failwith("Still pending?")
      | Some(childNode) =>
        root.node = Some((tree, childNode));
        print_endline("Add to the mwindow");
        node->NativeInterface.appendChild(childNode)
    };
  })
};

let noReason = (_) => NoReason;

module Cache = (Config: {type arg; type result; let reason: arg => suspendReason; let fetch: (arg) => async(result)}) => {
  let cache: Hashtbl.t(Config.arg, Config.result) = Hashtbl.create(10);
  let fetch = arg => switch (Hashtbl.find(cache, arg)) {
    | exception Not_found =>
      let suspendEvent = {
        reason: Config.reason(arg),
        payload: fin => {
          /* TODO dedup requests */
          Config.fetch(arg, value => {
            Hashtbl.replace(cache, arg, value);
            fin()
          })
        }
      }
      raise(SuspendException(suspendEvent));
    | value => value
  };
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

    type reconciler('data) = Reconciler('data);
    type suspenseHandler = SuspenseHandler;
    type exceptionHandler = ExceptionHandler;

    let useReconciler = (data, fn, hooks) => {
      let next = switch (hooks.current^) {
        | None => ref(None)
        | Some((Reconciler(r), next)) =>
          hooks.setReconciler(r, data, fn);
          next
      };
      hooks.current := Some((Reconciler(data), next));
      ((), hooks)
    };

    let useSuspenseHandler = ((), hooks) => {
      let next = switch (hooks.current^) {
        | None => ref(None)
        | Some((r, next)) =>
          next
      };
      hooks.current := Some((SuspenseHandler, next));
      (hooks.setSuspense(), hooks)
    };

    /* let useExceptionHandler = (handler, hooks) => {
      let next = switch (hooks.current^) {
        | None => ref(None)
        | Some((r, next)) =>
          next
      };
      hooks.current := Some((ExceptionHandler, next));
      (hooks.handleExceptions(handler), hooks)
    }; */

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

    let useReducer = (initial, reducer, hooks) => {
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
          action => {
            switch (hooks.current^) {
              | None => ()
              | Some((state, next)) =>
                hooks.current := Some((reducer(state, action), next));
            }
            hooks.invalidate();
          },
        ),
        {...hooks, current: next},
      )
    };

    /* let _runSuspense = (reason, value, args, next, hooks) => {
      hooks.current := Some((args, next))
      let suspendEvent = {
        reason,
        payload: fin => {
          value((), value => {
            if (!cancelled.contents) {
              fin();
            }
          })
        }
      }
      raise(SuspendException(suspendEvent));
    }; */

    /* let useSus = (~reason=NoReason, ) */

    /* TODO would be nice to have a `Loading | Refreshing(data) | Loaded(data)` thing in here.
       might be too complex for a first pass tho. */
    /* let useSuspense = (~reason=NoReason, value: unit => async('t), args, hooks) => {
      let (value, next) = switch (hooks.current^) {
        | None => _runSuspense(reason, value, args, ref(None), hooks)
        | Some(((prevArgs, status), next)) =>
          if (prevArgs != args) {
            _runSuspense(reason, value, args, ref(None), hooks)
            /* TODO TODO need to cancel previous */
          };
          switch (status^) {
            | None => raise(StillSuspended)
            | Some(value) => (value, next)
          }
      };
      (value, {...hooks, current: next})
    }; */

    /* [@hook]
    let useReducer = (reducer, initial) => {
      let%hook (state, setState) = useState(initial);
      (state, action => setState(reducer(state, action)))
    }; */

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