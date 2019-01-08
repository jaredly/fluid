// Generated by BUCKLESCRIPT VERSION 4.0.7000, PLEASE EDIT WITH CARE
'use strict';

var Block = require("bsb-native/lib/js/block.js");
var Curry = require("bsb-native/lib/js/curry.js");
var Layout = require("./Layout.bs.js");
var Caml_obj = require("bsb-native/lib/js/caml_obj.js");
var Belt_List = require("bsb-native/lib/js/belt_List.js");

function F(NativeInterface) {
  var string = function (layout, font, x) {
    return /* String */Block.__(0, [
              x,
              layout,
              font
            ]);
  };
  var makeComponent = function (identity, render) {
    return /* record */[
            /* init */(function (param) {
                return /* WithState */[/* record */[
                          /* identity */identity,
                          /* render */render,
                          /* hooks : record */[/* contents */undefined],
                          /* invalidated */false,
                          /* reconciler */undefined,
                          /* onChange */(function (param) {
                              return /* () */0;
                            })
                        ]];
              }),
            /* clone */(function (param) {
                var contents = param[0];
                if (contents[/* identity */0] === identity) {
                  if (contents[/* render */1] === render) {
                    return /* Same */925282182;
                  } else {
                    return /* `Compatible */[
                            785637236,
                            /* WithState */[/* record */[
                                /* identity */contents[/* identity */0],
                                /* render */render,
                                /* hooks */contents[/* hooks */2],
                                /* invalidated */contents[/* invalidated */3],
                                /* reconciler */contents[/* reconciler */4],
                                /* onChange */contents[/* onChange */5]
                              ]]
                          ];
                  }
                } else {
                  return /* Different */-890978471;
                }
              })
          ];
  };
  var Maker = /* module */[/* makeComponent */makeComponent];
  var runRender = function (param) {
    var component = param[0];
    var effects = /* record */[/* contents : [] */0];
    var hooks_000 = function (param) {
      component[/* invalidated */3] = true;
      return Curry._1(component[/* onChange */5], /* () */0);
    };
    var hooks_001 = function (oldData, data, reconcile) {
      component[/* reconciler */4] = /* tuple */[
        oldData,
        data,
        reconcile
      ];
      return /* () */0;
    };
    var hooks_002 = function (cleanup, fn, setCleanup) {
      effects[/* contents */0] = /* :: */[
        /* record */[
          /* cleanup */cleanup,
          /* fn */fn,
          /* setCleanup */setCleanup
        ],
        effects[/* contents */0]
      ];
      return /* () */0;
    };
    var hooks_003 = /* current */component[/* hooks */2];
    var hooks = /* record */[
      hooks_000,
      hooks_001,
      hooks_002,
      hooks_003
    ];
    component[/* invalidated */3] = false;
    var tree = Curry._1(component[/* render */1], hooks);
    return /* tuple */[
            tree,
            effects[0]
          ];
  };
  var getNativeNode = function (_tree) {
    while(true) {
      var tree = _tree;
      switch (tree.tag | 0) {
        case 0 : 
        case 1 : 
            return tree[1];
        case 2 : 
            _tree = tree[0][/* mountedTree */1];
            continue ;
        
      }
    };
  };
  var getInstanceLayout = function (_element) {
    while(true) {
      var element = _element;
      switch (element.tag | 0) {
        case 0 : 
            return element[1];
        case 1 : 
            return element[2];
        case 2 : 
            _element = element[1];
            continue ;
        
      }
    };
  };
  var getMountedLayout = function (_element) {
    while(true) {
      var element = _element;
      switch (element.tag | 0) {
        case 0 : 
            return element[2];
        case 1 : 
            return element[3];
        case 2 : 
            _element = element[0][/* mountedTree */1];
            continue ;
        
      }
    };
  };
  var instantiateTree = function (el) {
    switch (el.tag | 0) {
      case 0 : 
          var font = el[2];
          var layout = el[1];
          var contents = el[0];
          return /* IString */Block.__(0, [
                    contents,
                    Layout.createNodeWithMeasure(/* array */[], layout !== undefined ? layout : Layout.style(undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, /* () */0), Curry._2(NativeInterface[/* measureText */2], contents, font)),
                    font
                  ]);
      case 1 : 
          var measure = el[3];
          var layout$1 = el[2];
          var ichildren = Belt_List.map(el[1], instantiateTree);
          var childLayouts = Belt_List.toArray(Belt_List.map(ichildren, getInstanceLayout));
          var style = layout$1 !== undefined ? layout$1 : Layout.style(undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, /* () */0);
          return /* IBuiltin */Block.__(1, [
                    el[0],
                    ichildren,
                    measure !== undefined ? Layout.createNodeWithMeasure(childLayouts, style, measure) : Layout.createNode(childLayouts, style)
                  ]);
      case 2 : 
          var custom = Curry._1(el[0][/* init */0], /* () */0);
          var match = runRender(custom);
          return /* ICustom */Block.__(2, [
                    custom,
                    instantiateTree(match[0]),
                    match[1]
                  ]);
      
    }
  };
  var runEffect = function (param) {
    var cleanup = param[/* cleanup */0];
    if (cleanup !== undefined) {
      Curry._1(cleanup, /* () */0);
    }
    return Curry._1(param[/* setCleanup */2], Curry._1(param[/* fn */1], /* () */0));
  };
  var inflateTree = function (el) {
    switch (el.tag | 0) {
      case 0 : 
          var font = el[2];
          var layout = el[1];
          var contents = el[0];
          return /* MString */Block.__(0, [
                    contents,
                    Curry._3(NativeInterface[/* createTextNode */3], contents, layout, font),
                    layout,
                    font
                  ]);
      case 1 : 
          var layout$1 = el[2];
          var nativeElement = el[0];
          var node = Curry._2(NativeInterface[/* inflate */1], nativeElement, layout$1);
          var children = Belt_List.map(el[1], inflateTree);
          Belt_List.forEach(children, (function (child) {
                  return Curry._2(NativeInterface[/* appendChild */5], node, getNativeNode(child));
                }));
          return /* MBuiltin */Block.__(1, [
                    nativeElement,
                    node,
                    children,
                    layout$1
                  ]);
      case 2 : 
          var custom = el[0];
          var mountedTree = inflateTree(el[1]);
          var container = /* record */[
            /* custom */custom,
            /* mountedTree */mountedTree
          ];
          listenForChanges(custom, container);
          Belt_List.forEach(el[2], runEffect);
          return /* MCustom */Block.__(2, [container]);
      
    }
  };
  var listenForChanges = function (component, container) {
    var contents = component[0];
    contents[/* onChange */5] = (function (param) {
        var match = runRender(component);
        var newElement = match[0];
        var match$1 = contents[/* reconciler */4];
        var tmp;
        if (match$1 !== undefined) {
          var match$2 = match$1;
          tmp = Curry._4(match$2[2], match$2[0], match$2[1], container[/* mountedTree */1], newElement);
        } else {
          tmp = reconcileTrees(container[/* mountedTree */1], newElement);
        }
        container[/* mountedTree */1] = tmp;
        return Belt_List.forEach(match[1], runEffect);
      });
    return /* () */0;
  };
  var reconcileTrees = function (prev, next) {
    var exit = 0;
    switch (prev.tag | 0) {
      case 0 : 
          var font = prev[3];
          var layoutNode = prev[2];
          var node = prev[1];
          var a = prev[0];
          switch (next.tag | 0) {
            case 0 : 
                var bfont = next[2];
                var blayout = next[1];
                var b = next[0];
                if (a === b && Caml_obj.caml_equal(font, bfont)) {
                  layoutNode[/* style */1] = blayout !== undefined ? blayout : Layout.style(undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, /* () */0);
                  return /* MString */Block.__(0, [
                            a,
                            node,
                            layoutNode,
                            font
                          ]);
                } else {
                  Curry._3(NativeInterface[/* setTextContent */4], node, b, bfont);
                  Curry._1(Layout.Layout[/* LayoutSupport */0][/* markDirty */48], layoutNode);
                  return /* MString */Block.__(0, [
                            b,
                            node,
                            layoutNode,
                            bfont
                          ]);
                }
            case 1 : 
            case 2 : 
                exit = 1;
                break;
            
          }
          break;
      case 1 : 
          var aLayout = prev[3];
          var node$1 = prev[1];
          switch (next.tag | 0) {
            case 1 : 
                var bLayoutStyle = next[2];
                var bElement = next[0];
                if (Curry._3(NativeInterface[/* maybeUpdate */0], prev[0], node$1, bElement)) {
                  aLayout[/* style */1] = bLayoutStyle !== undefined ? bLayoutStyle : Layout.style(undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined, /* () */0);
                  return /* MBuiltin */Block.__(1, [
                            bElement,
                            node$1,
                            reconcileChildren(node$1, prev[2], next[1]),
                            aLayout
                          ]);
                } else {
                  var instances = instantiateTree(next);
                  var instanceLayout = getInstanceLayout(instances);
                  Layout.layout(instanceLayout);
                  var tree = inflateTree(instances);
                  Curry._2(NativeInterface[/* replaceWith */7], getNativeNode(prev), getNativeNode(tree));
                  return tree;
                }
            case 0 : 
            case 2 : 
                exit = 1;
                break;
            
          }
          break;
      case 2 : 
          var a$1 = prev[0];
          switch (next.tag | 0) {
            case 0 : 
            case 1 : 
                exit = 1;
                break;
            case 2 : 
                var match = Curry._1(next[0][/* clone */1], a$1[/* custom */0]);
                if (typeof match === "number") {
                  if (match >= 925282182) {
                    return /* MCustom */Block.__(2, [a$1]);
                  } else {
                    var instances$1 = instantiateTree(next);
                    var instanceLayout$1 = getInstanceLayout(instances$1);
                    Layout.layout(instanceLayout$1);
                    var tree$1 = inflateTree(instances$1);
                    Curry._2(NativeInterface[/* replaceWith */7], getNativeNode(prev), getNativeNode(tree$1));
                    return tree$1;
                  }
                } else {
                  var custom = match[1];
                  var match$1 = runRender(custom);
                  var tree$2 = reconcileTrees(a$1[/* mountedTree */1], match$1[0]);
                  a$1[/* custom */0] = custom;
                  a$1[/* mountedTree */1] = tree$2;
                  Belt_List.forEach(match$1[1], runEffect);
                  return /* MCustom */Block.__(2, [a$1]);
                }
            
          }
          break;
      
    }
    if (exit === 1) {
      var instances$2 = instantiateTree(next);
      var instanceLayout$2 = getInstanceLayout(instances$2);
      Layout.layout(instanceLayout$2);
      var tree$3 = inflateTree(instances$2);
      Curry._2(NativeInterface[/* replaceWith */7], getNativeNode(prev), getNativeNode(tree$3));
      return tree$3;
    }
    
  };
  var reconcileChildren = function (parentNode, aChildren, bChildren) {
    if (aChildren) {
      if (bChildren) {
        return /* :: */[
                reconcileTrees(aChildren[0], bChildren[0]),
                reconcileChildren(parentNode, aChildren[1], bChildren[1])
              ];
      } else {
        Belt_List.forEach(aChildren, (function (child) {
                return Curry._2(NativeInterface[/* removeChild */6], parentNode, getNativeNode(child));
              }));
        return /* [] */0;
      }
    } else if (bChildren) {
      var more = Belt_List.map(bChildren, (function (child) {
              return inflateTree(instantiateTree(child));
            }));
      Belt_List.forEach(more, (function (child) {
              return Curry._2(NativeInterface[/* appendChild */5], parentNode, getNativeNode(child));
            }));
      return more;
    } else {
      return /* [] */0;
    }
  };
  var mount = function (el, node) {
    var instances = instantiateTree(el);
    var instanceLayout = getInstanceLayout(instances);
    Layout.layout(instanceLayout);
    var tree = inflateTree(instances);
    return Curry._2(NativeInterface[/* appendChild */5], node, getNativeNode(tree));
  };
  var useReconciler = function (data, fn, hooks) {
    var match = hooks[/* current */3][0];
    var next;
    if (match !== undefined) {
      var match$1 = match;
      Curry._3(hooks[/* setReconciler */1], match$1[0], data, fn);
      next = match$1[1];
    } else {
      next = /* record */[/* contents */undefined];
    }
    hooks[/* current */3][0] = /* tuple */[
      data,
      next
    ];
    return /* tuple */[
            /* () */0,
            hooks
          ];
  };
  var useRef = function (initial, hooks) {
    var match = hooks[/* current */3][0];
    if (match !== undefined) {
      var match$1 = match;
      return /* tuple */[
              match$1[0],
              /* record */[
                /* invalidate */hooks[/* invalidate */0],
                /* setReconciler */hooks[/* setReconciler */1],
                /* triggerEffect */hooks[/* triggerEffect */2],
                /* current */match$1[1]
              ]
            ];
    } else {
      var r = /* record */[/* contents */initial];
      var next = /* record */[/* contents */undefined];
      hooks[/* current */3][0] = /* tuple */[
        r,
        next
      ];
      return /* tuple */[
              r,
              /* record */[
                /* invalidate */hooks[/* invalidate */0],
                /* setReconciler */hooks[/* setReconciler */1],
                /* triggerEffect */hooks[/* triggerEffect */2],
                /* current */next
              ]
            ];
    }
  };
  var useState = function (initial, hooks) {
    var match = hooks[/* current */3][0];
    var match$1;
    if (match !== undefined) {
      var match$2 = match;
      match$1 = /* tuple */[
        match$2[0],
        match$2[1]
      ];
    } else {
      var next = /* record */[/* contents */undefined];
      hooks[/* current */3][0] = /* tuple */[
        initial,
        next
      ];
      match$1 = /* tuple */[
        initial,
        next
      ];
    }
    var next$1 = match$1[1];
    return /* tuple */[
            /* tuple */[
              match$1[0],
              (function (v) {
                  hooks[/* current */3][0] = /* tuple */[
                    v,
                    next$1
                  ];
                  return Curry._1(hooks[/* invalidate */0], /* () */0);
                })
            ],
            /* record */[
              /* invalidate */hooks[/* invalidate */0],
              /* setReconciler */hooks[/* setReconciler */1],
              /* triggerEffect */hooks[/* triggerEffect */2],
              /* current */next$1
            ]
          ];
  };
  var useReducer = function (initial, reducer, hooks) {
    var match = useState(initial, hooks);
    var match$1 = match[0];
    var setState = match$1[1];
    var state = match$1[0];
    return /* tuple */[
            /* tuple */[
              state,
              (function (action) {
                  return Curry._1(setState, Curry._2(reducer, state, action));
                })
            ],
            match[1]
          ];
  };
  var newEffect = function (fn, args) {
    return /* record */[
            /* args */args,
            /* cleanup : record */[/* contents */undefined],
            /* fn */fn
          ];
  };
  var useEffect = function (fn, args, hooks) {
    var match = hooks[/* current */3][0];
    var match$1;
    if (match !== undefined) {
      var match$2 = match;
      var effect = match$2[0];
      var effect$1 = Caml_obj.caml_notequal(effect[/* args */0], args) ? (Curry._3(hooks[/* triggerEffect */2], effect[/* cleanup */1][/* contents */0], fn, (function (v) {
                  effect[/* cleanup */1][/* contents */0] = v;
                  return /* () */0;
                })), /* record */[
            /* args */args,
            /* cleanup */effect[/* cleanup */1],
            /* fn */fn
          ]) : effect;
      match$1 = /* tuple */[
        effect$1,
        match$2[1]
      ];
    } else {
      var effect$2 = newEffect(fn, args);
      Curry._3(hooks[/* triggerEffect */2], effect$2[/* cleanup */1][/* contents */0], fn, (function (v) {
              effect$2[/* cleanup */1][/* contents */0] = v;
              return /* () */0;
            }));
      match$1 = /* tuple */[
        effect$2,
        /* record */[/* contents */undefined]
      ];
    }
    var next = match$1[1];
    hooks[/* current */3][0] = /* tuple */[
      match$1[0],
      next
    ];
    return /* tuple */[
            /* () */0,
            /* record */[
              /* invalidate */hooks[/* invalidate */0],
              /* setReconciler */hooks[/* setReconciler */1],
              /* triggerEffect */hooks[/* triggerEffect */2],
              /* current */next
            ]
          ];
  };
  var useMemo = function (fn, args, hooks) {
    var match = hooks[/* current */3][0];
    var match$1;
    if (match !== undefined) {
      var match$2 = match;
      var match$3 = match$2[0];
      var match$4 = Caml_obj.caml_equal(match$3[1], args);
      var value = match$4 ? match$3[0] : Curry._1(fn, /* () */0);
      match$1 = /* tuple */[
        value,
        match$2[1]
      ];
    } else {
      match$1 = /* tuple */[
        Curry._1(fn, /* () */0),
        /* record */[/* contents */undefined]
      ];
    }
    var next = match$1[1];
    var value$1 = match$1[0];
    hooks[/* current */3][0] = /* tuple */[
      /* tuple */[
        value$1,
        args
      ],
      next
    ];
    return /* tuple */[
            value$1,
            /* record */[
              /* invalidate */hooks[/* invalidate */0],
              /* setReconciler */hooks[/* setReconciler */1],
              /* triggerEffect */hooks[/* triggerEffect */2],
              /* current */next
            ]
          ];
  };
  var useCallback = function (fn, args, hooks) {
    return useMemo((function (param) {
                  return fn;
                }), args, hooks);
  };
  var Hooks = /* module */[
    /* useReconciler */useReconciler,
    /* useRef */useRef,
    /* useState */useState,
    /* useReducer */useReducer,
    /* newEffect */newEffect,
    /* useEffect */useEffect,
    /* useMemo */useMemo,
    /* useCallback */useCallback
  ];
  return /* module */[
          /* string */string,
          /* Maker */Maker,
          /* runRender */runRender,
          /* getNativeNode */getNativeNode,
          /* getInstanceLayout */getInstanceLayout,
          /* getMountedLayout */getMountedLayout,
          /* instantiateTree */instantiateTree,
          /* runEffect */runEffect,
          /* inflateTree */inflateTree,
          /* listenForChanges */listenForChanges,
          /* reconcileTrees */reconcileTrees,
          /* reconcileChildren */reconcileChildren,
          /* mount */mount,
          /* Hooks */Hooks
        ];
}

exports.F = F;
/* Layout Not a pure module */
