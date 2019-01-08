/*
damping ratio
frequency response


TODO TODO TODO implement

https://gist.github.com/atnan/6473706eb041d499599dfffaac9fa10c

function convertDampingRatioResponseToStiffnessDamping(dampingRatio, response) {
  let mass = 1
  let angularFrequency = (2 * Math.PI) / response
  let stiffness = Math.pow(angularFrequency, 2) * mass
  let damping = dampingRatio * (2 * Math.sqrt(stiffness * mass))

  return { mass: mass, stiffness: stiffness, damping: damping }
}

function convertMassStiffnessDampingToDampingRatioResponse(mass, stiffness, damping) {
  let dampingRatio = damping / (2 * Math.sqrt(stiffness * mass))
  let angularFrequency = Math.sqrt(stiffness / mass)
  let response = (2 * Math.PI) / angularFrequency

  return { dampingRatio: dampingRatio, response: response }
}

 */

type config = {
  damping: float,
  stiffness: float,
  restDisplacementThreshold: float,
  restVelocityThreshold: float,
};

/**

w = rotations / second

freqResponse = ms / rotation
1 /. 

 */

let niceConfig = (~dampingRatio, ~frequencyResponse) => {
  let w = 1. /. frequencyResponse;
  let dampingFactor = sqrt(2.)/. 2. *. dampingRatio;
  let denom = (1. -. 2. *. dampingFactor *. dampingFactor);
  let stiffness = w *. w /. denom;
  let damping = dampingFactor *. 2. *. sqrt(stiffness);
  Js.log4(dampingFactor, denom, stiffness, damping);
  {stiffness, damping, restDisplacementThreshold: 0.0001, restVelocityThreshold: 0.0001}
};

/* let niceConfig = (~dampingRatio, ~frequencyResponse) => {
  let stiffness = frequencyResponse *. frequencyResponse /. (1. -. 2. *. dampingRatio);
  let damping = (dampingRatio *. 2. *. sqrt(abs_float(stiffness)));
  Js.log2(stiffness, damping);
  {stiffness, damping, restDisplacementThreshold: 0.0001, restVelocityThreshold: 0.0001}
}; */

/* let dampingFromStiffness = (ratio, stiffness) => ratio *. sqrt(2.) /. 2. *. 2. *. sqrt(stiffness); */
let perfectDamp = (stiffness) => sqrt(stiffness /. 2.);
/* let perfectDamp = (stiffness) => sqrt(stiffness *. 2.); */
/* let perfectDamp = (stiffness) => stiffness; */
let perfectDamp = (stiffness) => stiffness *. 4.;

type state = {
  config,
  velocity: float,
  currentValue: float,
  currentTime: float,
};

/*
This spring always springs from 1 to 0.
Which means you have to normalize your velocity as well.
*/

let init = (~initialVelocity=0., config) => {config, velocity: initialVelocity, currentValue: 1., currentTime: 0.};

let isAtRest = ({config: {restDisplacementThreshold, restVelocityThreshold}, velocity, currentValue}) => {
  abs_float(currentValue) < restDisplacementThreshold && abs_float(velocity) < restVelocityThreshold
};

let advance = (timeDelta, {config: {damping, stiffness}, currentValue, velocity} as state) => {
  let timeDelta = timeDelta /. 1000.;
  let acc = -. stiffness *. currentValue +. -. damping *. velocity *. timeDelta

  let velocity = velocity +. acc *. timeDelta;
  let currentValue = currentValue +. velocity *. timeDelta;

  {...state, velocity, currentValue}
};
