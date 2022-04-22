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
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

namespace flog {
namespace string {
  bool ends_with(char const search[], char const ending[]) {
    size_t search_length = length(search);
    size_t ending_length = length(ending);

    if (search_length < ending_length) {
      log("ends_with error: search_length smaller than ending_length");
      return 0;
    }

    for (size_t i = 0; i < ending_length; i++) {
      if (search[search_length-ending_length+i] != ending[i])  {
        return 0;
      }
    }

    return 1;
  }

  bool equals(char const left[], char const right[]) {
    if (left == 0 || right == 0) {
      return 0;
    }

    if (length(left) != length(right)) {
      return 0;
    }

    for (size_t i = 0; left[i] != 0; i++) {
      if (left[i] != right[i]) {
        return 0;
      }
    }

    return 1;
  }

  void print(char const format[], ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }

  size_t length(char const string[]) {
    size_t length = 0;
    while (string[length] != 0) {
      length++;
    }
    return length;
  }

  char* slice(char const string[], int start, int end) {
    size_t length = end - start;
    char* buffer = (char*) malloc(sizeof(char) * (length+1));
    buffer[length] = 0;
    for (int i = start; i < end; i++) {
      buffer[i - start] = string[i];
    }
    return buffer;
  }

  char* copy(const char* string) {
    size_t length = 0;
    while (string[length] != 0) {
      length++;
    }
    return slice(string, 0, length);
  }

  char* copy(const char* string, int length) {
    return slice(string, 0, length);
  }

  char* format(const char* format, ...) {
    char buffer[PATH_MAX];
    va_list args;
    va_start(args, format);
    int length = vsprintf(buffer, format, args);
    va_end(args);
    return copy(buffer, length);
  }

  int last(char const string[], char const character) {
    int position = -1;
    for (size_t i = 0; string[i] != 0; i++) {
      if (string[i] == character) {
        position = i;
      }
    }
    return position;
  }

  char* glue(char const left[], char const right[]) {
    size_t left_length = length(left);
    size_t right_length = length(right);
    size_t total = left_length + right_length + 1;
    char* buffer = (char*) malloc(sizeof(char) * (total+1));
    buffer[total] = 0;

    for (size_t i = 0; i < left_length; i++) {
      buffer[i] = left[i];
    }
    buffer[left_length] = '/';
    for (size_t i = 0; i < right_length; i++) {
      buffer[left_length+1+i] = right[i];
    }

    return buffer;
  };
}
}
