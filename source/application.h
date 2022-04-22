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

#ifndef FLOG_APPLICATION_H_
#define FLOG_APPLICATION_H_
#include <stdlib.h>
#include <functional>
#include "engine.h"
#include "database.h"

namespace flog {
class Application {
public:
  static void Initialize();
  static void Dispose();
  static Engine* GetEngine();
  static Database* GetDatabase();
  static void OnDispose(std::function<void()> callback);
private:
  std::function<void()> ondispose_cb;
  static Application* application;
  Engine* engine;
  Database* database;
  v8::Platform* platform;
  Application();
  ~Application();
};
}
#endif
