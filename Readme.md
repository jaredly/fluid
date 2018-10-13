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


GIVEN:
- damping_ratio between 0 and 1
- frequency_response in ms
- x = current position
- v = current velocity

DERIVE:
- accelleration

assuming a mass of 1 I think

####

damping_ratio = b / (2*sqrt(k))

** b in terms of k **
b = damping_ratio * 2 * sqrt(k)

accelleration = -k * x + -b * v

#####

b = damping_factor * 2 * sqrt(k)

accelleration = -k * x + -(damping_ratio * 2 * sqrt(k)) * v

w = sqrt(k * (1 - 2 * damping_ratio))

w^2 = k * (1 - 2 * damping_ratio)

k = frequency_response ^ 2 / (1 - 2 * damping_ratio)

accelleration = -k * x + -(damping_ratio * 2 * sqrt(k)) * v





What's frequency repsonse?

For underdamped, it's:

w^2 = - (b^2 - 2k)/(2)

w = sqrt(k - b^2/(2))

in terms of damping ratio:


damping_factor = sqrt(2)/2 * damping_ratio

w = undamped_w * sqrt(1 - 2 * damping_factor * damping_factor)

undamped_w = sqrt(k)











// equations with mass

zeta = b / (2*sqrt(k * m))

1 = b / (2*sqrt(k * m))

** b in terms of k & m **
b = damping_ratio * 2 * sqrt(k * m)


accelleration = -k * x + -b * v




What's frequency repsonse?

For underdamped, it's:

w^2 = - (b^2 - 2km)/(2m^2)

w = sqrt(k/m - b^2/(2m^2))

in terms of damping ratio:

w = undamped_w * sqrt(1 - 2 * damping_ratio)

undamped_w = sqrt(k / m)




