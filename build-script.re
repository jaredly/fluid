open Bsb_internals;


let ( +/ ) = Filename.concat;

gcc("lib" +/ "fluid.o", ["src" +/ "native" +/ "fluid.m"]);