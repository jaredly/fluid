// Generated by BUCKLESCRIPT VERSION 4.0.6, PLEASE EDIT WITH CARE

import * as Block from "bs-platform/lib/es6/block.js";
import * as Curry from "bs-platform/lib/es6/curry.js";
import * as Fluid from "./Fluid.js";
import * as Spring from "./Spring.js";
import * as Animate from "./Animate.js";
import * as Pervasives from "bs-platform/lib/es6/pervasives.js";
import * as Js_primitive from "bs-platform/lib/es6/js_primitive.js";
import * as Caml_builtin_exceptions from "bs-platform/lib/es6/caml_builtin_exceptions.js";

var Style = /* module */[];

function zoom(node) {
  return Animate.spring(1, 1000, undefined, (function (amount) {
                node.style.transform = "translateX(" + (Pervasives.string_of_float(amount * 100) + "px)");
                return /* () */0;
              }), (function () {
                node.parentNode.removeChild(node);
                return /* () */0;
              }));
}

var fadeOut = (function(node) {
  const box = node.getBoundingClientRect();
  node.style.position = 'absolute';
  node.style.pointerEvents = 'none';
  node.style.top = box.top + 'px';
  node.style.left = box.left + 'px';
  const max = 30;
  let timer = max;
  const loop = () => {
    timer -= 1;
    node.style.opacity = timer / max
    if (timer > 0) {
      requestAnimationFrame(loop)
    } else {
      node.parentNode.removeChild(node);
    }
  };
  requestAnimationFrame(loop)
});

var fadeIn = (function(node) {
  const max = 30;
  let timer = max;
  node.style.opacity = 0;
  const loop = () => {
    timer -= 1;
    node.style.opacity = 1 - timer / max
    if (timer > 0) {
      requestAnimationFrame(loop)
    }
  };
  requestAnimationFrame(loop)
});

function props(on, off, _) {
  return /* record */[
          /* on */on,
          /* off */off
        ];
}

function maker_001() {
  return false;
}

var maker_003 = /* reconcileTrees */(function (oldState, newState, mountedTree, newTree) {
    var exit = 0;
    if (oldState) {
      if (newState) {
        return mountedTree;
      } else {
        exit = 1;
      }
    } else if (newState) {
      exit = 1;
    } else {
      return mountedTree;
    }
    if (exit === 1) {
      var domNode = Fluid.getDomNode(mountedTree);
      var newTree$1 = Fluid.inflateTree(Fluid.instantiateTree(newTree));
      var newDomNode = Fluid.getDomNode(newTree$1);
      zoom(domNode);
      Curry._1(fadeIn, newDomNode);
      domNode.parentNode.insertBefore(newDomNode, domNode);
      return newTree$1;
    }
    
  });

function maker_004(param, state, setState) {
  if (state) {
    return Curry._1(param[/* on */0], (function () {
                  return Curry._1(setState, false);
                }));
  } else {
    return Curry._1(param[/* off */1], (function () {
                  return Curry._1(setState, true);
                }));
  }
}

var maker = /* record */[
  /* name */"Toggle",
  maker_001,
  /* newStateForProps */undefined,
  maker_003,
  maker_004
];

function make(props) {
  return Fluid.Maker[/* makeComponent */0](maker, props);
}

var Toggle = /* module */[
  /* props */props,
  /* maker */maker,
  /* make */make
];

function props$1(value, toString, _) {
  return /* tuple */[
          value,
          toString
        ];
}

function maker_001$1() {
  return "Folks";
}

function maker_004$1(param, state, setState) {
  return /* Builtin */Block.__(1, [
            "div",
            { },
            /* :: */[
              /* Builtin */Block.__(1, [
                  "div",
                  {
                    onclick: (function () {
                        return Curry._1(setState, state + "1");
                      })
                  },
                  /* :: */[
                    /* String */Block.__(0, ["Awesome " + (Curry._1(param[1], param[0]) + (" " + state))]),
                    /* [] */0
                  ]
                ]),
              /* [] */0
            ]
          ]);
}

var maker$1 = /* record */[
  /* name */"Awesome",
  maker_001$1,
  /* newStateForProps */undefined,
  /* reconcileTrees */undefined,
  maker_004$1
];

function make$1(props) {
  return Fluid.Maker[/* makeComponent */0](maker$1, props);
}

var Awesome = /* module */[
  /* props */props$1,
  /* maker */maker$1,
  /* make */make$1
];

function props$2(text, style, onClick, _) {
  return /* tuple */[
          text,
          style,
          onClick
        ];
}

var make$2 = Fluid.Maker[/* component */1]("Button", undefined, (function (param) {
        var onClick = param[2];
        return /* Builtin */Block.__(1, [
                  "button",
                  {
                    onclick: (function () {
                        return Curry._1(onClick, /* () */0);
                      }),
                    style: param[1]
                  },
                  /* :: */[
                    /* String */Block.__(0, [param[0]]),
                    /* [] */0
                  ]
                ]);
      }), /* () */0);

var Button = /* module */[
  /* props */props$2,
  /* make */make$2
];

var canvas = Fluid.createElement("canvas", { });

document.body.appendChild(canvas);

var visualize = (
  function (state, advance, isAtRest) {
    canvas.width = 500
    canvas.height = 500
    const ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, 500, 500);
    ctx.strokeStyle = 'black'
    ctx.lineWidth = 3;
    ctx.beginPath();
    ctx.moveTo(0, 350);
    ctx.stroke()
    for (var i=0; i<500; i++) {
      if (isAtRest(state)) {
        break
      }
      state = advance(1, state);
      ctx.lineTo(i, 250 + state[2] * 100)
    }
    ctx.stroke();
  }
);

function props_000(onClick) {
  return /* Builtin */Block.__(1, [
            "div",
            { },
            /* :: */[
              /* String */Block.__(0, ["Click this to"]),
              /* :: */[
                /* Custom */Block.__(2, [Curry._1(make$2, /* tuple */[
                          "Turn Off",
                          "background-color: #88ff88",
                          onClick
                        ])]),
                /* [] */0
              ]
            ]
          ]);
}

function props_001(onClick) {
  return /* Builtin */Block.__(1, [
            "div",
            { },
            /* :: */[
              /* Custom */Block.__(2, [Curry._1(make$2, /* tuple */[
                        "Turn On",
                        "background-color: #ffacf0",
                        onClick
                      ])]),
              /* :: */[
                /* String */Block.__(0, ["if you want"]),
                /* [] */0
              ]
            ]
          ]);
}

var props$3 = /* record */[
  props_000,
  props_001
];

function props_001$1(prim) {
  return String(prim);
}

var props$4 = /* tuple */[
  5,
  props_001$1
];

function props_001$2(x) {
  return x;
}

var props$5 = /* tuple */[
  "Hi",
  props_001$2
];

var first_001 = {
  id: "awesome"
};

var first_002 = /* :: */[
  /* String */Block.__(0, ["Hello"]),
  /* :: */[
    /* Builtin */Block.__(1, [
        "input",
        {
          type: "range",
          oninput: (function (evt) {
              var v = evt.target.value;
              var stiffness = 10 * (v + 1);
              var config_000 = /* damping */Spring.dampingFromStiffness(1, stiffness);
              var config = /* record */[
                config_000,
                /* stiffness */stiffness,
                /* restDisplacementThreshold */0.001,
                /* restVelocityThreshold */0.001
              ];
              var state = Spring.init(0, config);
              return Curry._3(visualize, state, Spring.advance, Spring.isAtRest);
            })
        },
        /* [] */0
      ]),
    /* :: */[
      /* Builtin */Block.__(1, [
          "div",
          {
            id: "here"
          },
          /* :: */[
            /* Builtin */Block.__(1, [
                "div",
                { },
                /* :: */[
                  /* String */Block.__(0, ["What"]),
                  /* [] */0
                ]
              ]),
            /* [] */0
          ]
        ]),
      /* :: */[
        /* Custom */Block.__(2, [Fluid.Maker[/* makeComponent */0](maker, props$3)]),
        /* :: */[
          /* Custom */Block.__(2, [Fluid.Maker[/* makeComponent */0](maker$1, props$4)]),
          /* :: */[
            /* Custom */Block.__(2, [Fluid.Maker[/* makeComponent */0](maker$1, props$5)]),
            /* :: */[
              /* Builtin */Block.__(1, [
                  "div",
                  {
                    id: "Inner"
                  },
                  /* :: */[
                    /* String */Block.__(0, ["world"]),
                    /* [] */0
                  ]
                ]),
              /* [] */0
            ]
          ]
        ]
      ]
    ]
  ]
];

var first = /* Builtin */Block.__(1, [
    "div",
    first_001,
    first_002
  ]);

var match = document.getElementById("root");

if (match !== undefined) {
  Fluid.mount(first, Js_primitive.valFromOption(match));
} else {
  throw [
        Caml_builtin_exceptions.assert_failure,
        /* tuple */[
          "App.re",
          189,
          12
        ]
      ];
}

export {
  Style ,
  zoom ,
  fadeOut ,
  fadeIn ,
  Toggle ,
  Awesome ,
  Button ,
  canvas ,
  visualize ,
  first ,
  
}
/* fadeOut Not a pure module */
