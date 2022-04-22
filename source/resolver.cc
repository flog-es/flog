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
#include "resolver.h"
#include "string.h"

#include <unistd.h>
#include <limits.h>

namespace flog {
bool Resolver::is_file(const char* path) {
  return flog::string::ends_with(path, ".js")
    || flog::string::ends_with(path, ".json")
    || flog::string::ends_with(path, ".so");
}
char* Resolver::directory(const char* path) {
  int last = string::last(path, '/');
  return string::slice(path, 0, last == -1 ? 0 : last + 1);
}
char* Resolver::resolve(char const* from, char const* specifier) {
  // absolute path
  if (specifier[0] == '/') {
    char* buffer = string::copy(specifier);
    char* _realpath = realpath(buffer, NULL);
    free(buffer);
    return _realpath;
  }
  char* buffer = 0;
  if (from == NULL) {
    // no directory, start from cwd
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    log("  [from] = %s (cwd)\n", cwd);
    buffer = string::glue(cwd, specifier);
  } else {
    log("  [from] = %s\n", from);
    buffer = string::glue(from, specifier);
  }
  log("  [specifier] = %s\n", specifier);
  char* _realpath = realpath(buffer, NULL);
  log("  return = %s\n", _realpath);
  free(buffer);
  return _realpath;
}

loader Resolver::resolve2(char const* path) {
  if (string::ends_with(path, ".js")) {
    return Module::js;
  }
  if (string::ends_with(path, ".json")) {
    return Module::json;
  }
  if (string::ends_with(path, ".so")) {
    return Module::so;
  }

  return Module::library;
}
}
