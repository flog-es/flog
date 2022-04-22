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

#ifndef FLOG_MODULE_H_
#define FLOG_MODULE_H_
#include <v8.h>
#include <set>
#include <map>

typedef std::vector<const char*> (*Exports)();
typedef v8::Local<v8::Value> (*Export)(v8::Local<v8::Context> context);

// struct declarations, local to this file

namespace flog {
class Module {
public:
  static Module* load(v8::Local<v8::Context> context,
                      v8::Local<v8::String>  specifier,
                      v8::Local<v8::Module>  referrer);
  static Module* js(char* path, v8::Local<v8::Context> context);
  static Module* json(char* path, v8::Local<v8::Context> context);
  static Module* so(char* path, v8::Local<v8::Context> context);
  static Module* library(char* path, v8::Local<v8::Context> context);
  static Module* get(v8::Local<v8::Module> module);
  static Module* by_path(const char* path);
  static void print();
  Module(v8::Local<v8::Context> context);
  Module(v8::Local<v8::Context> context,
         v8::Local<v8::Module>  module);
  v8::Local<v8::Value> Evaluate(bool return_namespace = false);
  v8::Maybe<bool> Instantiate();
  void SetModule(v8::Local<v8::Module> module) {
    this->module_ = v8::Global<v8::Module>(this->isolate, module);
  }
  v8::Local<v8::Module> GetModule() { return this->module_.Get(this->isolate); }
  v8::Local<v8::Context> GetContext() { return this->context_.Get(this->isolate); }
  v8::Isolate* GetIsolate() { return this->isolate; }
  char* path;
  char* from();
  void SetExport(const char* name, v8::Local<v8::Value> value);
  void SetArguments(int number, char* arguments[]);
  std::vector<char*> GetArguments();
private:
  static std::set<Module*> modules;
  static v8::MaybeLocal<v8::Value> SyntheticModuleEvaluationSteps(
    v8::Local<v8::Context> context,
    v8::Local<v8::Module> module);
  v8::Isolate* isolate;
  v8::Global<v8::Module> module_;
  v8::Global<v8::Context> context_;
  std::vector<const char*> export_names;
  std::vector<v8::Global<v8::Value>> export_values;
  std::vector<char*> arguments;
  void Synthesize(std::vector<v8::Local<v8::String>> export_names);
};
}
#endif
