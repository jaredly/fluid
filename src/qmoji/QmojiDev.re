let (/+) = Filename.concat;
let assetDir = Filename.dirname(Sys.argv[0]) /+ ".." /+ ".." /+ ".." /+ "assets";
Mojibar.run(assetDir);
