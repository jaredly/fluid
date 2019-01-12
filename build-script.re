open Bsb_internals;


let ( +/ ) = Filename.concat;

gcc(~flags=["-g"], "lib" +/ "fluid_shared.o", ["src" +/ "native" +/ "fluid_shared.m"]);
gcc(~flags=["-g"], "lib" +/ "fluid_components.o", ["src" +/ "native" +/ "fluid_components.m"]);
gcc(~flags=["-g"], "lib" +/ "fluid.o", ["src" +/ "native" +/ "fluid.m"]);
/* gcc("lib" +/ "fluid.o", [
  "src" +/ "native" +/ "fluid_shared.m",
  "src" +/ "native" +/ "fluid_components.m",
  "src" +/ "native" +/ "fluid.m"
]); */