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

#ifndef FLOG_DATABASE_H_
#define FLOG_DATABASE_H_
#include "string.h"
#include "engine.h"
#include "file.h"
#include <pwd.h>
#include <unistd.h>
#include <git2.h>
#include <set>

namespace flog {
class Database {
public:
  static Database* New();
  void Dispose();
  bool Has(char* module_name);
  char* GetPath(const char* module);
private:
  Database();
  ~Database();
  void load();
  char* home;
  std::set<char*> modules;
};
}
#endif
