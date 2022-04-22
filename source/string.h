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

#ifndef INCLUDE_FLOG_STRING_H_
#define INCLUDE_FLOG_STRING_H_
#include <v8.h>

namespace flog {
namespace string {
  bool ends_with(char const search[], char const suffix[]);
  bool equals(char const left[], char const right[]);
  void print(char const format[], ...);
  size_t length(char const string[]);
  char* slice(char const string[], int start, int end);
  char* copy(const char* string);
  char* copy(const char* string, int length);
  char* format(const char* format, ...);
  int last(char const string[], char character);
  char* glue(char const left[], char const right[]);
}
}
#endif
