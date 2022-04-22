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

#ifndef FLOG_RESOLVER_H_
#define FLOG_RESOLVER_H_
#include <v8.h>
#include "module.h"

namespace flog {

typedef Module* (*loader)(char* path, v8::Local<v8::Context> context);

class Resolver {
public:
  static bool is_file(char const* path);
  static char* resolve(char const* from, char const* specifier);
  static loader resolve2(char const* path);
  static char* directory(const char path[]);
};
}
#endif
