# Flog

Flog is a **JavaScript** runtime distribution. Written on top of the V8 engine
in C++, it has a minimal core.

**It is currently in its initial prototyping and feedback-gathering stage. While
there is already initial working code in this repository, it still mostly has a
*RFC/declaration of intent* character.**

## Design goals

* Minimal core as a thin wrapper around V8
* Executeable and module manager in one
* Globally namespaced, officially supported standard library
* Third-party, scoped module area
* Hot code replacement in development mode
* Sandboxing of applications (directory-level scoping)

## Contributing

Please create a new issue for any comment / suggestion you have. Happy for
any feedback!

## Why another JavaScript runtime? There is already Node.js and Deno?

Unfortunately both Node and Deno have their inherent shortcomings.

### Modules and security

Node's package manager, NPM, manages dependencies using a `package.json` file.
Node itself comes with a monolithic core containing many modules that aren't
pivotal for running JavaScript in itself, like `fs`. Without going through the
official documentation or checking the NPM website, it is not clear or
transparent which packages are part of core (thus 'officially' supported) and
which aren't.

In addition, this model is inherently insecure: any security issue within *any*
of the core modules in Node immediately becomes a concern for everyone: the only
way to patch it is to release a new version of Node.

NPM itself is also hardly protected against supply chain attacks. Any module
owner can publish a new version of the module and smuggle malicious code into
it. If this module is depended upon and the version specified
[allows for automatic updates][npm-auto-updates], NPM will install the affected
version. This has been [recently exemplified][synk1] [several times][synk2]. In
addition, as NPM doesn't distinguish between scoped and non-scoped modules in
terms of its security, [typosquatting attacks are possible][devtech].

Deno has no package manager and loads everything from the Internet. 'Pinning'
dependencies means hardcoding version numbers within JavaScript files. This can
lead to inconsistencies in the same module across files. In addition, real-time
inspection and modification of loaded modules is difficult: Deno caches
downloaded files, but in a converted, hard-to-edit format. Since modules can be
loaded from any URL, there is no single source of truth. Deno's core also has a
monolithic character: the `Deno` namespace contains functions like
`Deno.readTextFile`. It is unclear to an outside observer why certain
functionalities should be in core and others not.

*Flog proposes a built-in module manager as part of the `flog` executeable that
resolves and fetches dependencies from a central repository, like NPM. Unlike
NPM, we propose a clear-cut distinction between officially maintained standard
library modules in the global namespace and third-party, namespaced modules.* 

An officially maintained module, for example `filesystem` or `console`, would be
globally scoped:

```js
import {File} from "filesystem";
import console from "console";

console.log(await File.read("hello-world.txt"));
```

A third-party module would always be namespace-scoped:

```js
import {app} from "primate/core";

await app.run();
```

We further propose that dependencies and other metadata be recorded in a
`module.json` file in a project's root directory. Global (standard library)
dependencies need not be explicitly stated before their first use.

```json
{
  "name": "my-namespace/my-app",
  "version": "0.3.0",
  "description": "My own Primate app",
  "depends": {
    "primate/core": "0.3"
  }
}
```

While being similar to the `package.json` format, we suggest a slightly
different format and a different name (`module.json`) to make the difference
between NPM's and Flog's modules explicit.

We propose that updates to standard library modules, if not breaking (minor
version or patch updates), be automatic. In contrast, *all* updates to
third-party modules should be explicit, though if not breaking (minor version
or patch updates), offered as an option during runtime.

```
$ flog app.js
:: Synchronizing module database...
:: Checking for standard library module updates... 3 found 
(1/3) upgrading streams (1.0.1 -> 1.0.2)
(2/3) upgrading fs (1.0.0 -> 1.1.0)
(3/3) upgrading console (0.9.6 -> 0.9.7)
:: Checking for third-party module updates... 2 found
(1/2) upgrade primate/core (1.9.5 -> 1.9.6)
[c] continue [i] inspect changes [s] skip
c ... done
(2/2) upgrade primate/mongodb-store (0.9.3 -> 0.10.0)
[c] continue [i] inspect changes [s] skip
s ... skipping
$ 
```

We propose, like Node, to store all used modules, standard library and
third-party alike, within a special `modules` directory in the project's root
directory. The modules themselves will be git checkouts from git repositories
located at `$HOME/.flog/modules`. Checkouts would always be versioned, allowing
for different versions of the same module within the same project.

A `modules` directory in a given project could look thus.

```
filesystem/1.0.0/
filesystem/2.1.3/
primate/core/1.9.6/
primate/mongodb-store/0.9.3/
```

This layout describes four dependencies within one project: `primate/core`,
relying on `filesystem` (major version 2), and `primate/mongodb-store`, relying
on `filesystem` (major version 1). In case `primate/mongodb-store`'s next
version is updated with support for `filesystem` in major version 2, the
directory would look different:

```
filesystem/2.1.3/
primate/core/1.9.6/
primate/mongodb-store/0.10.0/
```

In other words, there is always exactly *one* checkout of any major version of
standard library modules.

### Standard library

Node doesn't have a standard library. Most of the functions that would be
otherwise included in a standard library are either part of node core (like
`streams` or `fs`) or can be provided via NPM.

Deno officially has a standard library, though there's nothing particular about
it. It is no different from any other module that can be loaded from the web,
except it is officially offered on the Deno website and maintained by the Deno
team.

*Flog proposes an officially maintained, audited standard library on the global
namespace of modules. Every module in the library would be using semantic
versioning to express API breakage.*

As noted before, standard library modules would be exempt from being recorded in
`module.json` manually before use. Simply using them in any file would lead to
their inclusion in `module.json`.

```js
import {File} from "filesystem";
```

After running `flog` on a file containing the code above, a `module.json` file
will be created or updated if already exists, containing the standard library
dependency.

```js
{
  "depends": {
    "filesystem": "1",
  }
}
```

Standard library module versions would only be expressable in terms of the major
version ("0", "1" and so on). Minor version or patch updates to standard library
module would be installed automatically on every run of `flog` within the
project's directory. Major version upgrades would be offered only after the user
has inspected changes.

```
$ flog app.js
:: Synchronizing module database...
:: Checking for standard library module updates... 1 found 
(1/1) upgrade filesystem (1.0.1 -> 2.0.0)
!! Breaking changes update; inspect changes first
[i] inspect changes [s] skip 
```

In addition, all standard library modules will be guaranteed to either export
classes or singletons, as fits them. Examples:

* `filesystem` would export `Path`, `File` and `Directory`, to manipulate the
file system
* `streams` would export `ReadableStream`, `WriteableStream`, and
`TransformStream`, in accordance with the official [Streams API][streams-api]
* `web` would export `Server` and `Client`
* `console` would export a singleton `console`, as it doesn't really make sense
to create several console instances

The modules that export classes could have both static and instance methods, as
appropriate. For example, `Path.resolve` would resolve file system paths.
`File.read("filename")` could be short for `new File("filename").read()`. These
details will be decided on a per-module basis, but all standard library modules
would adhere to the class/singleton pattern, to be consistently approachable.

### Runtime security

Node doesn't have any runtime security; a script executed by a user can do
anything that the user can do. This is a major security concern, as malicious
actors (via supply chain attacks or otherwise) and read from and write to $HOME.

Deno's security concept, on the other hand, is *too* limiting. Unless the flags
`--allow-read` and/or `--allow-write` are used, no file operations are possible.
To use the network, `--allow-net` must be set. This renders Deno pretty useless:
in reality, most applications need access to the file system and to the web,
which would lead most developers to simple use
`--allow-read --allow-write --allow-net` in their applications, reaching the
same situation as with Node. In short, both approaches are bad.

*Flog proposes a per-project directory scoped sandbox approach, similiar to the
browsers' per-tab sandbox approach. An application will only have access to the
files within its root directory and subdirectories, symlinks not being
followed.*

Thus, on any application run, a script would be able to read and modify anything
that's contained within its project root directory (the first parent directory
of the script's directory containing `module.json`) and its subdirectories. This
could be done by copying all files in the root directory to a separate runtime
directory, or simply using the files in place. Especially if considering
possible downloads from the web, it might be more adviseable to use a runtime
directory that is destroyed on the application exiting.

Sometimes an application might need to access additional files or directories
that are outside of its root directory's scope. For example, the application
might need to read SSL key/certificate files that are located at `/etc/ssl`.

In this case, a special directive will be available within `module.json`.

```js
{
   "exposes": {
      "/etc/ssl/key.ssl": "r",
      "/etc/ssl/certificate.ssl": "r"
   }
}
```

This approach makes sure a developer needs to conciously expose certain files
that are outside of the project's scope. However, this needs to be done only
once by recording it in `module.json`.

[synk1]: https://snyk.io/blog/open-source-npm-packages-colors-faker/
[synk2]: https://snyk.io/blog/peacenotwar-malicious-npm-node-ipc-package-vulnerability/
[npm-auto-updates]: https://docs.npmjs.com/about-semantic-versioning#using-semantic-versioning-to-specify-update-types-your-package-can-accept
[devtech]: https://developer-tech.com/news/2022/mar/24/large-scale-supply-chain-attack-used-218-malicious-npm-packages/
[streams-api]: https://developer.mozilla.org/en-US/docs/Web/API/Streams_API
