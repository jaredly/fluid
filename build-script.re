open Bsb_internals;


let ( +/ ) = Filename.concat;

let sources = [
  "fluid_shared",
  "fluid_components",
  "fluid_window",
  "fluid_hotkeys",
  "fluid"
];

sources |> List.iter(name =>
gcc(~flags=["-g"], "lib" +/ name ++ ".o", ["src" +/ "native" +/ name ++ ".m"]) |> ignore
)

/* gcc("lib" +/ "fluid.o", [
  "src" +/ "native" +/ "fluid_shared.m",
  "src" +/ "native" +/ "fluid_components.m",
  "src" +/ "native" +/ "fluid.m"
]); */