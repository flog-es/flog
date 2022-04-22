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

#include "application.h"
#include <stdarg.h>

namespace flog {
Application* Application::application = nullptr;

void Application::Initialize() {
  if (application == nullptr) {
    application = new Application();
  };
}

void Application::Dispose() {
  if (application != nullptr) {
    delete application;
  }
}

void Application::OnDispose(std::function<void()> callback) {
  application->ondispose_cb = callback;
}

Engine* Application::GetEngine() {
  return application->engine;
}

Database* Application::GetDatabase() {
  return application->database;
}

Application::Application() {
  this->platform = v8::platform::NewDefaultPlatform().release();
  v8::V8::InitializePlatform(this->platform);
  v8::V8::Initialize();

  /*class Task : public v8::JobTask {
   public:
    explicit Task(std::atomic_size_t* num_tasks_to_run)
        : num_tasks_to_run(num_tasks_to_run) {}
    void Run(v8::JobDelegate* delegate) override { 
      string::print("running task number: %d", num_tasks_to_run);
      --(*num_tasks_to_run); 
    }
    size_t GetMaxConcurrency(size_t worker_count) const override {
      return *num_tasks_to_run;
    }
    std::atomic_size_t* num_tasks_to_run;
  };
  //this->platform->PostJob(v8::TaskPriority::kBestEffort);
  std::atomic_size_t num_tasks_to_run(4);
  auto handle = this->platform->PostJob(v8::TaskPriority::kUserVisible,
    std::make_unique<Task>(&num_tasks_to_run));*/

  // start ES engine
  this->engine = Engine::New();
  // load module database
  this->database = Database::New();
}

Application::~Application() {
  // close module database
  this->database->Dispose();
  // shutdown ES engine
  this->engine->Dispose();

  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();

  delete this->platform;

  if (application->ondispose_cb != 0) {
    application->ondispose_cb();
  }
}
}
