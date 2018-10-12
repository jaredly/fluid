// Generated by BUCKLESCRIPT VERSION 4.0.6, PLEASE EDIT WITH CARE


function niceConfig(dampingRatio, frequencyResponse) {
  var stiffness = frequencyResponse * frequencyResponse / (1 - 2 * dampingRatio);
  var damping = dampingRatio * 2 * Math.sqrt(Math.abs(stiffness));
  console.log(stiffness, damping);
  return /* record */[
          /* damping */damping,
          /* stiffness */stiffness,
          /* restDisplacementThreshold */0.0001,
          /* restVelocityThreshold */0.0001
        ];
}

function dampingFromStiffness(ratio, stiffness) {
  return ratio * 2 * Math.sqrt(stiffness);
}

function init($staropt$star, config) {
  var initialVelocity = $staropt$star !== undefined ? $staropt$star : 0;
  return /* record */[
          /* config */config,
          /* velocity */initialVelocity,
          /* currentValue */1,
          /* currentTime */0
        ];
}

function isAtRest(param) {
  var match = param[/* config */0];
  if (Math.abs(param[/* currentValue */2]) < match[/* restDisplacementThreshold */2]) {
    return Math.abs(param[/* velocity */1]) < match[/* restVelocityThreshold */3];
  } else {
    return false;
  }
}

function advance(timeDelta, state) {
  var currentValue = state[/* currentValue */2];
  var velocity = state[/* velocity */1];
  var match = state[/* config */0];
  var timeDelta$1 = timeDelta / 1000;
  var acc = -match[/* stiffness */1] * currentValue + -match[/* damping */0] * velocity;
  var velocity$1 = velocity + acc * timeDelta$1;
  var currentValue$1 = currentValue + velocity$1 * timeDelta$1;
  return /* record */[
          /* config */state[/* config */0],
          /* velocity */velocity$1,
          /* currentValue */currentValue$1,
          /* currentTime */state[/* currentTime */3]
        ];
}

export {
  niceConfig ,
  dampingFromStiffness ,
  init ,
  isAtRest ,
  advance ,
  
}
/* No side effect */
