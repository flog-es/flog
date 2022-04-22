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
#include "../string.h"
#include "../application.h"
    
flog::Database* database;

namespace flog {
namespace operation {
  // permissible operations: <file>.js, sync, list
  int args2(int argc, char* argv[]) {
    if (argc != 2) {
      return -1;
    }

    char* operation = argv[1];

    if (string::ends_with(operation, ".js")) {
      return file(operation, argc, argv);
    }

    if (string::equals(operation, "sync")) {
      return sync();
    }

    if (string::equals(operation, "list")) {
      return list();
    }

    return 0;
  }

  // permissible operations: sync, info, with
  int args3(int argc, char* argv[]) {
    if (argc != 3) {
      return -1;
    }

    char* operation = argv[1];
    char* param = argv[2];

    if (string::equals(operation, "sync")) {
      return sync(param);
    }

    if (string::equals(operation, "info")) {
      return info(param);
    }

    if (string::equals(operation, "with")) {
      return with(param);
    }

    return 0;
  }

  int args(int argc, char* argv[]) {
    Application::Initialize();

    int result = argc == 2 ? args2(argc, argv) : args3(argc, argv);

    Application::Dispose();

    return result;
  }
}
}
