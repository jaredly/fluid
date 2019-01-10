# Fluid - a framework with first-class animations & transitions

## Thoughts

Ok, so the problem with imperative is it's hard to keep track of what's going on, and hard to know that you're doing it right. What if you're trying to transition from one state to another and the node you expect to exist doesn't?

Sooo

We'll still have the render() function which is the source of truth, the declarative way things are.
It will still be easy to understand what state will result in what UI.

BUT

you can declare a custom `reconcile()` function which will show the *imperative* steps for getting from one render tree to another.

Importantly, because imperative is hard, we'll check to make sure that your imperative steps get us to the place that the render function indicates.

`reconcile` will take as arguments:
- oldState
- oldMountedTree
- newState
- newElementTree

IFF you make a mistake, and the results of your imperative steps don't line up to the render() tree, then we show a warning, and do the old-fashioned reconcile so that render() is always the source of truth.

What counts as matching up? Mostly what you'd expect, with some important additions:
- you can have nodes that are "exiting" that are still around (in the dom) but not gone yet, as long as they're attached to an animation that will remove them from the DOM
- you can have a node that's not in the position the render() tree says it is, as long as it's layout is *animating toward* that position. -this-means- that fluid is very much aware of layout and animations as first-class things.
  - so "entering" is just a special kind of "is animating towards"

hrmmmm so I imagine what we really want is the layout algorithm's results for the oldMountedTree and the newElementTree.
Can the layout algorithm's recalculation be incremental & aided by imperative instructions from `reconcileLayout` as well?



hrmm how does layout fit in?
maybe I do a ... in-memory reconcile ... I'm not sure what representation yoga wants.
And how to apply the layout to DOM nodes.

Ok yeah here goes.
-> ermmm do a
...

ok so here are the things I want to make sure are doable:
- "exiting" nodes gracefully - e.g. not just removing them, fading out or sth
- moving a node from one place in the layout ~tree to another (instead of removing one place & creating a new one in the other)
- umm maybe that's it. oh "entering gracefully"

So first there's a "layout reconciliation"
where I take the previous fully layout-ed (and thereby instantiated) tree
and the new element tree
and give imperative ways to update one to match the other based on the state transition.

And then! we calculate the new layout.

And then! there's a "layout application" where you get to decide how to apply the new layout things to the old layout tree.

And then! we actually put that into effect in the DOM.

#(reconcileTree)
(oldMountedTree, oldState, newState, newElement)
->
(layoutTreeChanges) (including maybe "exit node animations" or some such)

oldMountedTree + layoutTreeChanges => newTreeReadyForLayout

newTreeReadyForLayout => (yoga) => newTreeWithLayout

#(applyNewLayout)
(oldMountedTree, oldState, newState, newTreeWithLayout)
->
(layoutApplications)

Where layoutApplications might include
"set the transform of this element to be an animation from the current position to the new position"


And then we take
oldMountedTree + layoutTreeChanges + layoutApplication

and hopefully come up with a newMountedTree that matches the newTreeWithLayout



## New question with hero transitions:

- if you have two components that think they own a node, who wins?
- how do you negotiate between the two?

orrr do you have explicitly that they don't both own it. One has to relinquish ownership.

For example:

<PhotoListView>
<PhotoDetailView>

You're viewing photos, you tap on one, and then the photo list view's
state says "viewing this photo", and the reconciler passes off the
selected photo cell to the <PhotoDetailView> to zoom with.

Reconciling back will bring the thing back, I think?

could get complicated.


# Thinking about native support

I've moved all dom related things into a `NativeInterface` file
Open question: What does the JSX do?
for native components.

annnd maybe this is the point where I should abstract all the things
into `View` and `Text` and `Button` etc.

but again, for the times when you need a native component.

```html
<div onClick={x} />
```

or in cocoa land

```html
<ns-view onClick={x} />
```

for html we get around by just being unsafe, and knowing that the dom is incredibly forgiving of bad values.
cocoa will not be so forgiving.

Therefore, maybe having functions to call is the best way to do it.

So `<div onClick={x} />` becomes `Fluid.Builtin(Native.div(~onClick=x, ()), [])`

and then `Native.div` can create the `NativeElement` that it wants.


Another consideration. Currently I'm pretending that all native elements have the same type.
Again, the dom is forgiving, and you can add a `TextNode` in the same way you would a `div`.

although looks like in AppKit I can use an `NSTextField`

anddd maybe those can be added with abandon? Will see how far I get


####

<view>
  <Hello />
  <view flex=1 />
</view>

and Hello is

let toggle = useState(false)

<view>
  {str(toggle ? "Hello folks" : "Hello")}
  {toggle ? str("Yeah") : null}
  <ExpensiveChild />
</view>


# element

Builtin(View, [
  Custom(hello())
  Builtin(View, [], style(flex=1))
  Custom(expensiveChild())
])

# instance

(note how a Custom component's children get instantiated immediately after `render()` is called)

IBuiltin(View, [
  ICustom(helloWithState: {render, hooks, invalidated, reconciler, onChange}, 
    IBuiltin(View, [
      IBuiltin(String("Hello"), layout),
      INull(layout)
    ], layout)
  )
], layout)

# mounted

MBuiltin(View, [
  MCustom({custom: helloWithState{render, hooks, invalidated, reconciler, onChange}, mountedTree:
    MBuiltin(View, [
      MBuiltin(String("Hello folks"), layout, nativeNode),
      MNull(nativeNode)
    ])
    st
  })
], layout, nativeNode)

# SetState!! rerender is needed
currently I apply the change immediately, so the hooks state is now different
(would there be a reason to wait & not change immediately? Maybe an inconsistent view on state? idk. prolly not)

- the change is applied to the hook. in the state case, that's updating the ref value.
- the component container is enqueued to be re-rendered, and `invalidated` is set to true
- component->runRender returns an element tree:
  Builtin(View, [
    Builtin(String("Hello folks")),
    Builtin(String("Yeah"))
  ])

So, in the "no custom shenanigans" case (ignoring keys for the moment), we do:

reconcileTrees(
  MBuiltin(View, [
    MBuiltin(String("Hello folks"), layout, nativeNode),
    MNull(nativeNode)
  ])
  ,
  Builtin(View, [
    Builtin(String("Hello folks")),
    Builtin(String("Yeah"))
  ])
)


Ok, here's how we do.

a mountedTree Custom can have a pendingTree or a mountedTree
buuut only when it's pending time... at other times it cant

because what if I'm trying to reconcile and its actually not mounted

hrmmm but I don't think I can have it both ways. I'll try this.




### keys too I guess

BUT lets also consider the case where there are keys involved

  Builtin(View, [
    Builtin(String("Yeah))
    Builtin(~key="Hello", String("Hello folks")),
  ])

And so the order is different, but the node for "hello" should be re-used
