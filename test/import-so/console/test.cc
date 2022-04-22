/*#include <v8.h>

void log(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  auto isolate = args.GetIsolate();
  v8::String::Utf8Value value(isolate, args[0]);
  printf("%s", *value);
}*/

#include "stdio.h"

void fun() {
  printf("I am in fun5");
}

/*namespace flog {
  class Console : public Module {
    public:
      void exports() {
        auto isolate = this->isolate();
        this->SetExport("log", v8::FunctionTemplate::New(isolate, log));
      }
  };

  __attribute__((constructor)) static void register_console() {
    //node_module_register(&_module);
  }
  __attribute__((destructor)) static void deregister_console() {
    printf("deregistering");
  }
}*/
