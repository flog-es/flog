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

#include "engine.h"
#include "module.h"
#include "string.h"
#include "String.h"
#include "application.h"

namespace flog {
  Engine* Engine::New() {
    return new Engine();
  }

  void Engine::Dispose() {
    delete this;
  }

  void Engine::MetaCallback(
      v8::Local<v8::Context> context,
      v8::Local<v8::Module> module,
      v8::Local<v8::Object> meta) {
    auto isolate = context->GetIsolate();
    meta->Set(context,
      String::New(isolate, "url"),
      String::New(isolate, Module::get(module)->path)
    ).Check();
    /*std::vector<char*> args = _module->GetArguments();
    if (args.size() > 0) {
      size_t arguments_size = args.size();
      v8::Local<v8::Array> array = v8::Array::New(isolate, arguments_size);
      for (size_t i = 0; i < arguments_size; i++) {
        string::print(args[i]);
        array->Set(context, 0, String::New(isolate, args[i])).Check();
      }
      meta->Set(context, String::New(isolate, "arguments"), array).Check();
    }*/
  }

  v8::MaybeLocal<v8::Promise> Engine::AsyncImportCallback(
      v8::Local<v8::Context> context,
      v8::Local<v8::ScriptOrModule> referrer,
      v8::Local<v8::String> specifier,
      v8::Local<v8::FixedArray> import_assertions) {
    v8::Local<v8::String> path = referrer->GetResourceName().As<v8::String>();
    v8::String::Utf8Value _path(context->GetIsolate(), path);
    v8::Local<v8::Module> mod = flog::Module::by_path(*_path)->GetModule();
    // Promise resolver: that way promise for dynamic import can be rejected
    // or full-filed
    v8::Local<v8::Promise::Resolver> resolver =
        v8::Promise::Resolver::New(context).ToLocalChecked();
    v8::MaybeLocal<v8::Promise> promise(resolver->GetPromise());

    // Loading module (with checking)
    v8::String::Utf8Value name(context->GetIsolate(), specifier);
    flog::Module* module = flog::Module::load(context, specifier, mod);
    if (module != nullptr) {
      if (module->Instantiate().IsNothing()) {
        return v8::MaybeLocal<v8::Promise>();
      }
      v8::Local<v8::Value> return_value = module->Evaluate(true);

      if (!return_value.IsEmpty()) {
        // Resolving (fulfilling) promise with module global namespace
        resolver->Resolve(context, return_value).ToChecked();
        return promise;
      }
    }

    return v8::MaybeLocal<v8::Promise>();
  }

  Engine::Engine() {
    // Create a new Isolate and make it the current one.
    this->allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = this->allocator;
    this->isolate = v8::Isolate::New(create_params);

    // Binding dynamic import() callback
    this->isolate->SetHostImportModuleDynamicallyCallback(AsyncImportCallback);

    // Binding meta object
    this->isolate->SetHostInitializeImportMetaObjectCallback(MetaCallback);
  }

  Engine::~Engine() {
    this->isolate->TerminateExecution();
    this->isolate->Dispose();
    delete this->allocator;
  }

  v8::Isolate* Engine::GetIsolate() {
    return this->isolate;
  }
}
