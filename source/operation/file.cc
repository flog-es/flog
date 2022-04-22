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

#include "operation.h"

void print(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  auto isolate = args.GetIsolate();
  v8::String::Utf8Value value(isolate, args[0]);
  flog::string::print("%s", *value);
}

namespace flog {
namespace operation {
  int file(char* path, int argc, char* argv[]) {
    v8::Isolate* isolate = Application::GetEngine()->GetIsolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "log", v8::FunctionTemplate::New(isolate, print));
    v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);
    v8::Context::Scope context_scope(context);
    v8::Local<v8::String> specifier = String::New(isolate, path);
    Module* module = Module::load(context, specifier, v8::Local<v8::Module>());
    module->SetArguments(argc, argv);
    if (module != nullptr) {
      if (module->Instantiate().IsNothing()) {
        return -1;
      };
      module->Evaluate();
      Module::print();
    }

    return 0;
  }
}
}
