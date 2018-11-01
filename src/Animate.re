
[@bs.val] external requestAnimationFrame: (unit => unit) => unit = "";

let spring = (~dampingRatio, ~frequencyResponseMs, ~initialVelocity=0., update, finish) => {
  /* let config =
    Spring.niceConfig(
      ~dampingRatio=0.8,
      ~frequencyResponse=0.5,
    ); */
  let stiffness = 1000.;
  let stiffness = 500.;
  /* let stiffness = 200.; */
  let config = {
    Spring.damping: Spring.perfectDamp(stiffness),
    stiffness: stiffness,
    restDisplacementThreshold: 0.001,
    restVelocityThreshold: 0.001,
  };
  let state = ref(Spring.init(~initialVelocity, config));
  let lastTime = ref(Js.Date.now());
  let startTime = Js.Date.now();
  let rec loop = () => {
    update(state^.currentValue);
    let now = Js.Date.now();
    let timeDelta = now -. lastTime^;
    state := Spring.advance(timeDelta, state^);
    lastTime := now;
    if (Spring.isAtRest(state^)) {
      /* Js.log(Js.Date.now() -. startTime); */
      finish()
    } else {
      requestAnimationFrame(loop)
    }
  };
  loop();
};