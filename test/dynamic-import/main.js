const module = (await import("import.js")).default;

const success = () => log("\x1b[32m.\x1b[0m");
const failure = () => log("\x1b[31mx\x1b[0m");
const assert = result => result ? success() : failure();

assert(module === 1);
