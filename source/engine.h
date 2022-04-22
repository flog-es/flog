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

#ifndef FLOG_ENGINE_H_
#define FLOG_ENGINE_H_
#include <libplatform/libplatform.h>
#include <v8.h>

namespace flog {
class Engine {
public:
  static Engine* New();
  void Dispose();
  v8::Isolate* GetIsolate();
private:
  static void MetaCallback(
    v8::Local<v8::Context> context,
    v8::Local<v8::Module> module,
    v8::Local<v8::Object> meta);
  static v8::MaybeLocal<v8::Promise> AsyncImportCallback(
    v8::Local<v8::Context> context,
    v8::Local<v8::ScriptOrModule> referrer,
    v8::Local<v8::String> specifier,
    v8::Local<v8::FixedArray> import_assertions);
  Engine();
  ~Engine();
  v8::Isolate* isolate;
  v8::ArrayBuffer::Allocator* allocator;
};
}
#endif
