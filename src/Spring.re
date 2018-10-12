/*
damping ratio
frequency response

let project = (initialVelocity, decelrationRate) => {
  (initialVelocity / 1000.) * decelerationRate / (1. -. decelerationRate)
}


critical_damping = 2*sqrt(k * m)

actual_damping / critial_damping == the ratio

natural frequency = sqrt(k/m)

 */

type config = {
  damping: float,
  stiffness: float,
  restDisplacementThreshold: float,
  restVelocityThreshold: float,
};

let niceConfig = (~dampingRatio, ~frequencyResponse) => {
  let stiffness = frequencyResponse *. frequencyResponse /. (1. -. 2. *. dampingRatio);
  let damping = (dampingRatio *. 2. *. sqrt(abs_float(stiffness)));
  Js.log2(stiffness, damping);
  {stiffness, damping, restDisplacementThreshold: 0.0001, restVelocityThreshold: 0.0001}
};

let dampingFromStiffness = (ratio, stiffness) => ratio *. 2. *. sqrt(stiffness);

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
  let acc = -. stiffness *. currentValue +. -. damping *. velocity

  let velocity = velocity +. acc *. timeDelta;
  let currentValue = currentValue +. velocity *. timeDelta;

  {...state, velocity, currentValue}
};
