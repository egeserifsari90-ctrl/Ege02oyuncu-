// Experimental Super Bear Adventure / Unity IL2CPP time-scale bridge.
// Does not load itself into the game; no gameplay ability hooks are included.
#include <cmath>
#include <dlfcn.h>

namespace {
using DomainGet = void* (*)();
using ThreadAttach = void* (*)(void*);
using AssemblyOpen = void* (*)(void*, const char*);
using AssemblyImage = void* (*)(void*);
using ClassFromName = void* (*)(void*, const char*, const char*);
using ClassMethod = void* (*)(void*, const char*, int);
using Invoke = void* (*)(const void*, void*, void**, void**);

template <typename T>
T symbol(void* library, const char* name) {
  return reinterpret_cast<T>(dlsym(library, name));
}

struct LibraryHandle {
  explicit LibraryHandle(void* value) : handle(value) {}
  LibraryHandle(const LibraryHandle&) = delete;
  LibraryHandle& operator=(const LibraryHandle&) = delete;
  ~LibraryHandle() { if (handle) dlclose(handle); }
  void* handle;
};
} // namespace

// This must be called on the Unity main thread after libil2cpp.so is loaded.
// Changes simulation time, not just the bear's walking/running speed.
// No flying, noclip, or unlimited-jump implementation is claimed here.
extern "C" int egemodder_apply_game_time_scale(float factor) {
  if (!std::isfinite(factor) || factor < 0.25f || factor > 3.0f) return -1;
#if defined(RTLD_NOLOAD)
  LibraryHandle lib(dlopen("libil2cpp.so", RTLD_NOW | RTLD_NOLOAD));
#else
  return -2;
#endif
  if (!lib.handle) return -2;
  auto get_domain = symbol<DomainGet>(lib.handle, "il2cpp_domain_get");
  auto attach = symbol<ThreadAttach>(lib.handle, "il2cpp_thread_attach");
  auto open_assembly = symbol<AssemblyOpen>(lib.handle, "il2cpp_domain_assembly_open");
  auto get_image = symbol<AssemblyImage>(lib.handle, "il2cpp_assembly_get_image");
  auto find_class = symbol<ClassFromName>(lib.handle, "il2cpp_class_from_name");
  auto find_method = symbol<ClassMethod>(lib.handle, "il2cpp_class_get_method_from_name");
  auto invoke = symbol<Invoke>(lib.handle, "il2cpp_runtime_invoke");
  if (!get_domain || !attach || !open_assembly || !get_image ||
      !find_class || !find_method || !invoke) return -3;
  void* domain = get_domain();
  if (!domain || !attach(domain)) return -4;
  void* assembly = open_assembly(domain, "UnityEngine.CoreModule");
  if (!assembly) assembly = open_assembly(domain, "UnityEngine.CoreModule.dll");
  if (!assembly) return -5;
  void* image = get_image(assembly);
  void* klass = image ? find_class(image, "UnityEngine", "Time") : nullptr;
  const void* method = klass ? find_method(klass, "set_timeScale", 1) : nullptr;
  if (!method) return -6;
  void* args[] = {&factor};
  void* exception = nullptr;
  invoke(method, nullptr, args, &exception);
  return exception ? -7 : 0;
}

extern "C" const char* egemodder_time_scale_status(int code) {
  switch (code) {
    case 0: return "ok: Unity Time.timeScale method invoked (not device-tested)";
    case -1: return "invalid scale: expected finite factor between 0.25 and 3.0";
    case -2: return "libil2cpp.so not loaded";
    case -3: return "IL2CPP API symbols missing";
    case -4: return "IL2CPP domain/thread unavailable";
    case -5: return "UnityEngine.CoreModule not found";
    case -6: return "UnityEngine.Time.set_timeScale not found";
    case -7: return "managed exception during call";
    default: return "unknown status";
  }
}
