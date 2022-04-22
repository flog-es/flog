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

#ifndef FLOG_OPERATION_OPERATION_H
#define FLOG_OPERATION_OPERATION_H

#include "../application.h"
#include "../string.h"
#include "../String.h"
#include "../module.h"
#include "../file.h"

#include <libplatform/libplatform.h>
#include <v8.h>

namespace flog {
namespace operation {
  int help();
  int args(int argc, char* argv[]);

  int args2(int argc, char* argv[]);
  int file(char* file, int argc, char* argv[]);
  int sync();
  int list();

  int args3(int argc, char* argv[]);
  int sync(char const module[]);
  int with(char const module[]);
  int info(char const module[]);
}
}
#endif
