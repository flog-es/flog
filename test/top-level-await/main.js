import static_import from  "./static-import.js";

const success = () => log("\x1b[32m.\x1b[0m");
const failure = () => log("\x1b[31mx\x1b[0m");
const assert = result => result ? success() : failure();

assert(static_import === "static-import");

for (let i = 1; i < 3; i++) {
  const imported = await import(`./dynamic-import-${i}.js`);
  assert(imported.default === `dynamic-import-${i}`);
}
log("\n");
