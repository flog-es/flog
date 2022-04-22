/* flog - JavaScript runtime distribution 
 * Copyright (C) 2022 EG <terrablue@primatejs.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "debug.h"
#include "module.h"
#include "file.h"
#include "string.h"
#include "String.h"
#include "resolver.h"
#include "database.h"
#include "application.h"

#include <dlfcn.h>

namespace flog {
// linked list of modules
std::set<Module*> Module::modules;

Module* Module::load(v8::Local<v8::Context> context,
                     v8::Local<v8::String>  specifier,
                     v8::Local<v8::Module>  referrer) {
  log("static Module::load\n");
  auto isolate = context->GetIsolate();
  v8::String::Utf8Value to_resolve(isolate, specifier);
  char* path = 0;
  if (Resolver::is_file(*to_resolve)) {
    path = Resolver::resolve(
      referrer.IsEmpty() ? NULL : get(referrer)->from()
    , * to_resolve);
  } else {
    path = * to_resolve;
  }

  Module* exists = by_path(path);
  return exists != nullptr
    ? exists
    : Resolver::resolve2(path)(path, context);
}

/*
 * Load a module from a JavaScript Object Notation (.json) file.
 */
Module* Module::json(char* path, v8::Local<v8::Context> context) {
  log("static Module::json\n");
  auto isolate = context->GetIsolate();
  v8::Local<v8::Module> local = v8::Module::CreateSyntheticModule(
    isolate,
    String::New(isolate, path),
    { String::New(isolate, "default") },
    [](v8::Local<v8::Context> _context, v8::Local<v8::Module> module) -> v8::MaybeLocal<v8::Value> {
      auto _isolate = _context->GetIsolate();
      // get the module's path to load the JSON file
      v8::Local<v8::String> json = File::read(_isolate, get(module)->path);
      v8::Local<v8::Value> value;
      if (!v8::JSON::Parse(_context, json).ToLocal(&value)) {
        printf("Error to local JSON!\n");
      }
      v8::Maybe<bool> result = module->SetSyntheticModuleExport(
        _isolate,
        String::New(_isolate, "default"),
        value
      );
      if (result.IsNothing()) {}
      return v8::MaybeLocal<v8::Value>(v8::True(_isolate));
    }
  );
  Module* module = new Module(context, local);
  module->path = path;

  return module;
}

v8::MaybeLocal<v8::Value> Module::SyntheticModuleEvaluationSteps(
    v8::Local<v8::Context> context,
    v8::Local<v8::Module> v8_module) {
  auto isolate = context->GetIsolate();
  Module* module = get(v8_module);
  for (size_t i = 0; i < module->export_names.size(); i++) {
    v8::Maybe<bool> result = v8_module->SetSyntheticModuleExport(
      isolate,
      String::New(isolate, module->export_names[i]),
      module->export_values[i].Get(isolate)
    );
    if (result.IsNothing()) {}
  }
  return v8::MaybeLocal<v8::Value>(v8::True(isolate));
}

void Module::Synthesize(std::vector<v8::Local<v8::String>> _export_names) {
  this->SetModule(v8::Module::CreateSyntheticModule(
    this->isolate,
    String::New(isolate, this->path),
    _export_names,
    SyntheticModuleEvaluationSteps));
}

/*
 * Load a module from a shared object (.so) file.
 */
Module* Module::so(char* path, v8::Local<v8::Context> context) {
  log("Module::so %s\n", path);
  auto isolate = context->GetIsolate();
  void* lib = dlopen(path, RTLD_LAZY);
  if (lib == 0) {
    string::print("error: %s\n", dlerror());
  } else {
    Exports export_names = (Exports) dlsym(lib, "export_names");
    Exports export_functions = (Exports) dlsym(lib, "export_functions");
    if (export_names == 0 || export_functions == 0) {
      string::print("must define the 'export_names' and 'export_functions' symbols");
      dlclose(lib);
    } else {
      Module* module = new Module(context);
      std::vector<const char*> v_export_names = export_names();
      std::vector<const char*> v_export_functions = export_functions();
      std::vector<v8::Local<v8::String>> s_export_names;
      for (size_t i = 0; i < v_export_names.size(); i++) {
        const char* export_name = v_export_names[i];
        const char* export_symbol_name = v_export_functions[i];
        s_export_names.push_back(String::New(isolate, export_name));
        Export _export = (Export) dlsym(lib, export_symbol_name);
        v8::Local<v8::Value> value = _export(context);
        module->SetExport(export_name, value);
      }
      module->path = path;
      module->Synthesize(s_export_names);
      //dlclose(lib);
      return module;
    }
  }

  return nullptr;
}

/*
 * Load a module from the library. If the module exists in the library, it will
 * resolve to an actual file under modules/<module>/<version>/main.js
 
 * If the module hasn't been synced yet to the local project, flog will sync
 * the current version in the database to the modules directory.
 */
Module* Module::library(char* name, v8::Local<v8::Context> context) {
  if (!Application::GetDatabase()->Has(name)) {
    char* copy = string::copy(name);
    Application::OnDispose([copy]() {
      string::print("'%s' doesn't exist in library\n", copy);
      free(copy);
    });
  }
  return js(Application::GetDatabase()->GetPath(name), context);
}

/*
 * Load a module from a JavaScript (.js) file.
 */
Module* Module::js(char* path, v8::Local<v8::Context> context) {
  log("static Module::js\n");
  auto isolate = context->GetIsolate();

  v8::Local<v8::String> code = File::read(isolate, path);
  v8::ScriptOrigin origin(
      isolate,
      String::New(isolate, path),
      0,
      0,
      false,
      -1,
      v8::Local<v8::Value>(),
      false,
      false,
      true,
      v8::Local<v8::PrimitiveArray>());

  // Compiling module from source (code + origin)
  v8::Context::Scope context_scope(context);
  v8::ScriptCompiler::Source source(code, origin);
  // allocate memory
  //Module* module = (Module*) malloc(sizeof(Module));
  // this is the currently last module
  // Check
  v8::Local<v8::Module> local;
  if (!v8::ScriptCompiler::CompileModule(isolate, &source).ToLocal(&local)) {
    Application::OnDispose([]() {
      string::print("Error compiling module!\n");
    });
    return nullptr;
  }
  /*int requests_length = local->GetModuleRequestsLength();
  printf("\nrequests length: %d\n", requests_length);
  for (int i = 0; i < requests_length; i++) {
    v8::Local<v8::String> specifier = local->GetModuleRequest(i);
    v8::String::Utf8Value specifier_utf8(isolate, specifier);
    printf("request %d: %s\n", i, *specifier_utf8);
  }*/

  Module* module = new Module(context, local);
  module->path = path;

  return module;
}

Module::Module(v8::Local<v8::Context> context) {
  log("Module::Module\n");
  this->isolate = context->GetIsolate();
  this->context_ = v8::Global<v8::Context>(this->isolate, context);
  modules.insert(this);
  log("modules size: %ld\n", modules.size());
}

Module::Module(v8::Local<v8::Context> context,
               v8::Local<v8::Module>  module) {
  log("Module::Module\n");
  this->isolate = context->GetIsolate();
  this->context_ = v8::Global<v8::Context>(this->isolate, context);
  this->module_ = v8::Global<v8::Module>(this->isolate, module);
  modules.insert(this);
  log("modules size: %ld\n", modules.size());
}

v8::Local<v8::Value> Module::Evaluate(bool return_namespace) {
  log("Module::evaluate %s\n", this->path);
  // Executing module with return value
  v8::Local<v8::Value> return_value;
  v8::Local<v8::Module> module = this->GetModule();
  if (!module->Evaluate(this->GetContext()).ToLocal(&return_value)) {
    Application::OnDispose([]() {
      string::print("Error evaluating module!\n");
    });
  }

  if (module->GetStatus() == v8::Module::Status::kErrored) {
    v8::Local<v8::String> exception = module->GetException().As<v8::String>();
    v8::String::Utf8Value pex(isolate, exception);

    char* slice = string::slice(*pex, 0, pex.length());
    Application::OnDispose([slice]() -> void {
      string::print("%s\n", slice);
      free(slice);
    });
  }

  return return_namespace ? module->GetModuleNamespace() : return_value;
}

char* Module::from() {
  return Resolver::directory(this->path);
}

void Module::SetExport(const char* name, v8::Local<v8::Value> value) {
  v8::Global<v8::Value> _value = v8::Global<v8::Value>(this->isolate, value);
  this->export_names.push_back(name);
  this->export_values.push_back(std::move(_value));
}

void Module::SetArguments(int count, char* _arguments[]) {
  for (int i = 0; i < count; i++) {
    this->arguments.push_back(_arguments[i]);
  }
}

std::vector<char*> Module::GetArguments() {
  return this->arguments;
};

void Module::print() {
  log("static Module::print\n");
  std::set<Module*>::iterator iterator = modules.begin();
  for (; iterator != modules.end(); iterator++) {
    Module* module = *iterator;
    log("  id: %d, path: %s\n", module->GetModule()->GetIdentityHash(), module->path);
  }
  log("\n");
}

Module* Module::by_path(const char* path) {
  log("static Module::by_path\n");
  for (auto it = modules.begin(); it != modules.end(); it++) {
    if (string::equals((*it)->path, path)) {
      return *it;
    }
  }

  return nullptr;
}

Module* Module::get(v8::Local<v8::Module> module) {
  log("static Module::get\n");
  for (auto it = modules.begin(); it != modules.end(); it++) {
    if ((*it)->GetModule()->GetIdentityHash() == module->GetIdentityHash()) {
      return *it;
    }
  }
  return nullptr;
}

v8::Maybe<bool> Module::Instantiate() {
  auto result = this->GetModule()->InstantiateModule(this->GetContext(),
    [](v8::Local<v8::Context> context,
       v8::Local<v8::String> specifier,
       v8::Local<v8::FixedArray> import_assertions,
       v8::Local<v8::Module> referrer) -> v8::MaybeLocal<v8::Module> {
    Module* module = load(context, specifier, referrer);
    return module != nullptr ? module->GetModule() : v8::MaybeLocal<v8::Module>();
  });
  if (result.IsNothing()) {
    string::print("Can't instantiate module %s.\n", this->path);
  }
  return result;
}
}
