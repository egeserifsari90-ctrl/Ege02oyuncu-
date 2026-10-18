#include <cstring>
namespace {
int domain, thread, assembly, image, klass, method, exception;
float last_scale = -1.f;
bool should_raise = false;
int call_count = 0;
}
extern "C" void* il2cpp_domain_get() { return &domain; }
extern "C" void* il2cpp_thread_attach(void* d) { return d == &domain ? &thread : nullptr; }
extern "C" void* il2cpp_domain_assembly_open(void* d, const char* name) {
  if (d != &domain || !name) return nullptr;
  return (std::strcmp(name, "UnityEngine.CoreModule") == 0 ||
          std::strcmp(name, "UnityEngine.CoreModule.dll") == 0)
             ? &assembly : nullptr;
}
extern "C" void* il2cpp_assembly_get_image(void* a) {
  return a == &assembly ? &image : nullptr;
}
extern "C" void* il2cpp_class_from_name(void* i, const char* ns, const char* n) {
  return i == &image && std::strcmp(ns, "UnityEngine") == 0 &&
                 std::strcmp(n, "Time") == 0 ? &klass : nullptr;
}
extern "C" void* il2cpp_class_get_method_from_name(void* c, const char* n, int argc) {
  return c == &klass && std::strcmp(n, "set_timeScale") == 0 && argc == 1
             ? &method : nullptr;
}
extern "C" void* il2cpp_runtime_invoke(const void* m, void* obj, void** args, void** exc) {
  if (m != &method || obj || !args || !args[0]) {
    *exc = &exception; return nullptr;
  }
  last_scale = *static_cast<float*>(args[0]);
  ++call_count;
  *exc = should_raise ? &exception : nullptr;
  return nullptr;
}
extern "C" float fake_last_scale() { return last_scale; }
extern "C" int fake_call_count() { return call_count; }
extern "C" void fake_raise_exception(int value) { should_raise = value != 0; }
