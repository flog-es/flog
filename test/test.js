import test2 from "../test2.js";

log("testw");
log(test2());

//log(import.meta.url);

import("./test4.js").then(module => {
  log("test4");
  log(module.default);
  log("test6");
});
