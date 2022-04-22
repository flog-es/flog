#include <stdio.h>
#include <v8.h>

void log(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  v8::String::Utf8Value value(args.GetIsolate(), args[0]);
  printf("%s\n", *value);
}

extern "C" v8::Local<v8::Value> console(v8::Local<v8::Context> context) {
  auto isolate = context->GetIsolate();
  v8::Local<v8::Object> object = v8::Object::New(isolate);
  v8::Local<v8::String> name =
    v8::String::NewFromUtf8(isolate, "log").ToLocalChecked();

  v8::EscapableHandleScope scope(isolate);
  v8::Local<v8::FunctionTemplate> funct = v8::FunctionTemplate::New(isolate, log); 
  v8::Local<v8::Function> func;
  if (!funct->GetFunction(context).ToLocal(&func)) {
    return v8::Local<v8::Value>();
  }

  object->Set(context, name, func).Check();
  return scope.Escape(object);
}

extern "C" std::vector<const char*> export_names() {
  return {"default"};
}

extern "C" std::vector<const char*> export_functions() {
  return {"console"};
}
