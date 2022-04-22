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
#include "string.h"
#include <stdio.h>

void line(char const*, char const*);
void line(char const* operation, char const* explanation) {
  flog::string::print("  flog %-20s %s\n", operation, explanation);
}

namespace flog {
namespace operation {
  int help() {
    const char* version = "0.0.1";
    string::print("flog %s\n", version);
    string::print("v8 %s (dynamic)\n\n", v8::V8::GetVersion());
    string::print("usage:  flog <file>.js\n");
    string::print("        flog <operation> [...]\n\n");
    string::print("local operations (current directory):\n");
    line("sync <module>", "add/update <module> as dependency into module.json");
    line("list", "show synced modules"),
    string::print("\nglobal operations:\n");
    line("info <module>", "show info about <module>");
    line("with <module> [...]", "execute <module> with ... as params");
    return 0;
  }
}
}
