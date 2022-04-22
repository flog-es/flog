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

#include "database.h"

namespace flog {
  Database* Database::New() {
    return new Database();
  }

  void Database::Dispose() {
    delete this;
  }

  Database::Database() {
    const char* homedir = getpwuid(getuid())->pw_dir;
    this->home = string::glue(homedir, ".flog");
    File::new_directory(this->home);
    this->load();
  }

  Database::~Database() {
    //git_libgit2_shutdown();
    for (auto it = this->modules.begin(); it != this->modules.end(); it++) {
      free(*it);
    }
    this->modules.clear();
    free(this->home);
  }

  bool Database::Has(char* module_name) {
    for (auto it = this->modules.begin(); it != this->modules.end(); it++) {
      if (string::equals(*it, module_name)) {
        return true;
      }
    }
    return false;
  }

  char* Database::GetPath(const char* name) {
    return string::format("%s/modules/%s/main.js", this->home, name);
  }

  void Database::load() {
    char* path = string::glue(this->home, "master.db");
    char* file = File::read(path);
    free(path);
    for (int i = 0, count = 0; file[i] != 0; i++) {
      if (file[i] == '\n') {
        // found new module
        char* buffer = string::slice(file, i-count, i);
        this->modules.insert(buffer);
        count = -1;
      }
      count++;
    }
    free(file);

  }

  /*void Database::init() {
    v8::Isolate* isolate = Engine::get()->isolate();
    string::print("init database: %s\n", this->home);

    //git_libgit2_init();
    //git_repository* repository = 0;
    char* path = string::glue(this->home, "database/master.json");
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr);
    v8::Context::Scope context_scope(context);
    v8::Local<v8::String> json = File::read(isolate, path);
    v8::Local<v8::Value> json_value;
    if (!v8::JSON::Parse(context, json).ToLocal(&json_value)) {
      string::print("Error to local JSON!\n");
      exit(EXIT_FAILURE);
    }

    if (json_value->IsArray()) {
      v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(json_value);
      //string::print("size %d\n", object->Length());
      for (size_t i = 0; i < array->Length(); i++) {
        v8::MaybeLocal<v8::Value> entry = array->Get(context, i);
        v8::Local<v8::Value> lentry;
        if (!entry.ToLocal(&lentry)) {

        }
        v8::String::Utf8Value name(isolate, lentry);
        char* buffer = string::slice(*name, 0, name.length());
        string::print("%s\n", buffer);
        this->modules.insert(buffer);
//         Local<Value> value = object->Get(key);
         // do stuff with key / value
      }
      string::print("SIZE %d\n", this->modules.size());
    } else {
      string::print("NOT OBJECT, master.json is corrupt");
    }
    //string::print("path: %s\n", path);
    free(path);
    //string::print("contents: %s\n", *json);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    
    if (git_repository_open(&repository, path) == 0) {
      string::print("opened");
    } else {
      string::print("didn't open!");
    };
    //git_repository_free(repository);
  }*/
}
