// Generated by BUCKLESCRIPT VERSION 4.0.7000, PLEASE EDIT WITH CARE
'use strict';

var Block = require("bsb-native/lib/js/block.js");
var FluidMaker = require("../FluidMaker.bs.js");
var Pervasives = require("bsb-native/lib/js/pervasives.js");
var Js_primitive = require("bsb-native/lib/js/js_primitive.js");

function $$setImmediate(fn) {
  setTimeout(fn, 0);
  return /* () */0;
}

var setDomProps = function (node,props){
    Object.keys(props).forEach(key => {
      if (key === 'checked' || key === 'value' || key === 'textContent') {
        node[key] = props[key]
      } else if (typeof props[key] === 'function') {
        node[key] = props[key]
      } else if (key === 'style' && typeof props[key] === 'object' && props[key]) {
        Object.keys(props[key]).forEach(st => {
          node.style[st] = props[key][st]
        })
      } else {
        node.setAttribute(key, props[key])
      }
    })
  };

function string_of_float(f) {
  if ((f | 0) === f) {
    return Pervasives.string_of_float(f) + "0";
  } else {
    return Pervasives.string_of_float(f);
  }
}

function createElement(typ, nativeProps, layout) {
  console.log("Creating", typ, nativeProps);
  var node = document.createElement(typ);
  setDomProps(node, nativeProps);
  node.style.position = "absolute";
  node.style.left = string_of_float(layout[/* layout */2][/* left */0]) + "px";
  node.style.top = string_of_float(layout[/* layout */2][/* top */1]) + "px";
  node.style.width = string_of_float(layout[/* layout */2][/* width */4]) + "px";
  node.style.height = string_of_float(layout[/* layout */2][/* height */5]) + "px";
  return node;
}

var defaultFont = /* record */[
  /* fontName */"system-ui",
  /* fontSize */16
];

var measureWithCanvas = (
  function() {
    var canvas = document.createElement('canvas');
    var context = canvas.getContext('2d');
    var cache = {};
    document.body.appendChild(canvas);
    return function (text, font) {
      const key = `${text}:${font[0]}:${font[1]}`;
      if (cache[key]) {
        return cache[key]
      }
      context.font = font[1] + 'px ' + font[0]
      const dims = context.measureText(text);
      cache[key] = [dims.width, font[1] * 1.2]
      return cache[key]
    }
  }()
  );

function measureText(text, font, _node, width, widthMode, height, heightMode) {
  var font$1 = font !== undefined ? font : defaultFont;
  return measureWithCanvas(text, font$1);
}

function createNullNode(param) {
  return document.createTextNode("");
}

function setTextContent(el, text, font) {
  var font$1 = font !== undefined ? font : defaultFont;
  el.textContent = text;
  var style = el.style;
  style.fontFamily = font$1[/* fontName */0];
  style.fontSize = string_of_float(font$1[/* fontSize */1]) + "px";
  return /* () */0;
}

function createTextNode(text, layout, font) {
  var el = createElement("span", { }, layout);
  setTextContent(el, text, font);
  return el;
}

function updateNativeProps(node, _oldProps, newProps) {
  return setDomProps(node, newProps);
}

function canUpdate(mounted, mountPoint, newElement) {
  return mounted[/* tag */0] === newElement[/* tag */0];
}

function updateLayout(mounted, mountPoint, layout) {
  mountPoint.style.position = "absolute";
  mountPoint.style.left = string_of_float(layout[/* layout */2][/* left */0]) + "px";
  mountPoint.style.top = string_of_float(layout[/* layout */2][/* top */1]) + "px";
  mountPoint.style.width = string_of_float(layout[/* layout */2][/* width */4]) + "px";
  mountPoint.style.height = string_of_float(layout[/* layout */2][/* height */5]) + "px";
  return /* () */0;
}

function update(mounted, mountPoint, newElement, layout) {
  if (mounted[/* tag */0] === newElement[/* tag */0] && mounted[/* props */1] !== newElement[/* props */1]) {
    mountPoint.style.position = "absolute";
    mountPoint.style.left = string_of_float(layout[/* layout */2][/* left */0]) + "px";
    mountPoint.style.top = string_of_float(layout[/* layout */2][/* top */1]) + "px";
    mountPoint.style.width = string_of_float(layout[/* layout */2][/* width */4]) + "px";
    mountPoint.style.height = string_of_float(layout[/* layout */2][/* height */5]) + "px";
    return setDomProps(mountPoint, newElement[/* props */1]);
  } else {
    return 0;
  }
}

function inflate(param, layout) {
  return createElement(param[/* tag */0], param[/* props */1], layout);
}

var NativeInterface = /* module */[
  /* setImmediate */$$setImmediate,
  /* setDomProps */setDomProps,
  /* string_of_float */string_of_float,
  /* createElement */createElement,
  /* defaultFont */defaultFont,
  /* measureWithCanvas */measureWithCanvas,
  /* measureText */measureText,
  /* createNullNode */createNullNode,
  /* setTextContent */setTextContent,
  /* createTextNode */createTextNode,
  /* updateNativeProps */updateNativeProps,
  /* canUpdate */canUpdate,
  /* updateLayout */updateLayout,
  /* update */update,
  /* inflate */inflate
];

var include = FluidMaker.F([
      $$setImmediate,
      canUpdate,
      update,
      updateLayout,
      inflate,
      measureText,
      createNullNode,
      (function (prim, prim$1) {
          prim.appendChild(prim$1);
          return /* () */0;
        }),
      (function (prim, prim$1) {
          prim.appendAfter(prim$1);
          return /* () */0;
        }),
      (function (prim, prim$1) {
          prim.removeChild(prim$1);
          return /* () */0;
        }),
      (function (prim, prim$1) {
          prim.replaceWith(prim$1);
          return /* () */0;
        })
    ]);

function div(id, $staropt$star, layout, _type, width, height, onclick, style, param) {
  var children = $staropt$star !== undefined ? $staropt$star : /* [] */0;
  var tmp = { };
  if (id !== undefined) {
    tmp.id = Js_primitive.valFromOption(id);
  }
  if (_type !== undefined) {
    tmp.type = Js_primitive.valFromOption(_type);
  }
  if (width !== undefined) {
    tmp.width = Js_primitive.valFromOption(width);
  }
  if (height !== undefined) {
    tmp.height = Js_primitive.valFromOption(height);
  }
  if (onclick !== undefined) {
    tmp.onclick = Js_primitive.valFromOption(onclick);
  }
  if (style !== undefined) {
    tmp.style = Js_primitive.valFromOption(style);
  }
  return /* Builtin */Block.__(0, [
            /* record */[
              /* tag */"div",
              /* props */tmp
            ],
            children,
            layout,
            undefined
          ]);
}

function button(id, children, layout, _type, width, height, onclick, style, param) {
  var tmp = { };
  if (id !== undefined) {
    tmp.id = Js_primitive.valFromOption(id);
  }
  if (_type !== undefined) {
    tmp.type = Js_primitive.valFromOption(_type);
  }
  if (width !== undefined) {
    tmp.width = Js_primitive.valFromOption(width);
  }
  if (height !== undefined) {
    tmp.height = Js_primitive.valFromOption(height);
  }
  if (onclick !== undefined) {
    tmp.onclick = Js_primitive.valFromOption(onclick);
  }
  if (style !== undefined) {
    tmp.style = Js_primitive.valFromOption(style);
  }
  return /* Builtin */Block.__(0, [
            /* record */[
              /* tag */"button",
              /* props */tmp
            ],
            children,
            layout,
            undefined
          ]);
}

function img(src, layout, param) {
  return /* Builtin */Block.__(0, [
            /* record */[
              /* tag */"img",
              /* props */{
                src: src
              }
            ],
            /* [] */0,
            layout,
            undefined
          ]);
}

function input(id, _type, width, height, onchange, oninput, style, param) {
  var tmp = { };
  if (id !== undefined) {
    tmp.id = Js_primitive.valFromOption(id);
  }
  if (_type !== undefined) {
    tmp.type = Js_primitive.valFromOption(_type);
  }
  if (onchange !== undefined) {
    tmp.onchange = Js_primitive.valFromOption(onchange);
  }
  if (oninput !== undefined) {
    tmp.oninput = Js_primitive.valFromOption(oninput);
  }
  if (width !== undefined) {
    tmp.width = Js_primitive.valFromOption(width);
  }
  if (height !== undefined) {
    tmp.height = Js_primitive.valFromOption(height);
  }
  if (style !== undefined) {
    tmp.style = Js_primitive.valFromOption(style);
  }
  return /* Builtin */Block.__(0, [
            /* record */[
              /* tag */"input",
              /* props */tmp
            ],
            /* [] */0,
            undefined,
            undefined
          ]);
}

function string(layout, font, x) {
  var match = font !== undefined ? font : defaultFont;
  return /* Builtin */Block.__(0, [
            /* record */[
              /* tag */"span",
              /* props */{
                textContent: x,
                style: {
                  fontFamily: match[/* fontName */0],
                  fontSize: string_of_float(match[/* fontSize */1]) + "px"
                }
              }
            ],
            /* [] */0,
            layout,
            (function (param, param$1, param$2, param$3, param$4) {
                return measureText(x, font, param, param$1, param$2, param$3, param$4);
              })
          ]);
}

function text(layout, font, contents, param) {
  return string(layout, font, contents);
}

var Native = /* module */[
  /* div */div,
  /* button */button,
  /* img */img,
  /* input */input,
  /* string */string,
  /* text */text
];

var Fluid_001 = /* NoReason */include[0];

var Fluid_002 = /* LoadingImage */include[1];

var Fluid_003 = /* SuspendException */include[2];

var Fluid_004 = /* StillSuspended */include[3];

var Fluid_005 = /* makePending */include[4];

var Fluid_006 = /* pendingReplace */include[5];

var Fluid_007 = /* Maker */include[6];

var Fluid_008 = /* mapResult */include[7];

var Fluid_009 = /* bindResult */include[8];

var Fluid_010 = /* runRender */include[9];

var Fluid_011 = /* getNativeNode */include[10];

var Fluid_012 = /* getNativePending */include[11];

var Fluid_013 = /* getInstanceLayout */include[12];

var Fluid_014 = /* getMountedLayout */include[13];

var Fluid_015 = /* getPendingLayout */include[14];

var Fluid_016 = /* updateLayout */include[15];

var Fluid_017 = /* instantiateTree */include[16];

var Fluid_018 = /* runEffect */include[17];

var Fluid_019 = /* mountTo */include[18];

var Fluid_020 = /* mountPending */include[19];

var Fluid_021 = /* reconcileTrees */include[20];

var Fluid_022 = /* reconcileChildren */include[21];

var Fluid_023 = /* enqueue */include[22];

var Fluid_024 = /* mount */include[23];

var Fluid_025 = /* preMount */include[24];

var Fluid_026 = /* noReason */include[25];

var Fluid_027 = /* Cache */include[26];

var Fluid_028 = /* Hooks */include[27];

var Fluid = /* module */[
  /* NativeInterface */0,
  Fluid_001,
  Fluid_002,
  Fluid_003,
  Fluid_004,
  Fluid_005,
  Fluid_006,
  Fluid_007,
  Fluid_008,
  Fluid_009,
  Fluid_010,
  Fluid_011,
  Fluid_012,
  Fluid_013,
  Fluid_014,
  Fluid_015,
  Fluid_016,
  Fluid_017,
  Fluid_018,
  Fluid_019,
  Fluid_020,
  Fluid_021,
  Fluid_022,
  Fluid_023,
  Fluid_024,
  Fluid_025,
  Fluid_026,
  Fluid_027,
  Fluid_028,
  /* Native */Native,
  /* string */string
];

exports.NativeInterface = NativeInterface;
exports.Fluid = Fluid;
/* measureWithCanvas Not a pure module */
