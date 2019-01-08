
let measureText = (w, h, _, _, _, _, _) => {Layout.LayoutTypes.width: w, height: h};

let a = Layout.createNodeWithMeasure(
  [||],
  Layout.style(),
  measureText(30., 50.)
);

let b = Layout.createNode(
  [|a|],
  Layout.style()
);

let c = Layout.createNodeWithMeasure(
  [||],
  Layout.style(),
  measureText(100., 30.)
);


let d = Layout.createNode(
  [|b, c|],
  Layout.style()
);

Layout.layout(d);

Printf.printf("%f, %f\n", d.layout.width, d.layout.height);
Printf.printf("%f, %f, %f x %f\n", c.layout.top, c.layout.left, c.layout.width, c.layout.height);
