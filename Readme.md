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

