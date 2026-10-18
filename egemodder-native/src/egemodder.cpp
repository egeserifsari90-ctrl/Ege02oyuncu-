// Super Bear Adventure 13.0.8 IL2CPP-native experimental component.
// Not injected into the game. NOT yet a mod APK.
#include <dlfcn.h>
#include <cmath>
namespace {
using DomainGet = void*(*)();
using ThreadAttach = void*(*)(void*);
using AssemblyOpen = void*(*)(void*,const char*);
using AssemblyImage = void*(*)(void*);
using ClassFromName = void*(*)(void*,const char*,const char*);
using ClassMethod = void*(*)(void*,const char*,int);
using Invoke = void*(*)(const void*,void*,void**,void**);
template<typename T> T sym(void* lib,const char* n) {
  return reinterpret_cast<T>(dlsym(lib,n));
}
}
// Apply game-wide simulation speed on Unity's MAIN thread after startup.
// This is not running/walking speed. No flying, noclip, or jump hooks exist yet.
extern "C" int egemodder_apply_game_time_scale(float factor) {
  if (!std::isfinite(factor) || factor<0.25f || factor>3.0f) return -1;
#ifdef RTLD_NOLOAD
  void* lib = dlopen("libil2cpp.so",RTLD_NOW|RTLD_NOLOAD);
#else
  void* lib = nullptr;
#endif
  if (!lib) return -2;
  auto get_domain = sym<DomainGet>(lib,"il2cpp_domain_get");
  auto attach = sym<ThreadAttach>(lib,"il2cpp_thread_attach");
  auto open_assembly = sym<AssemblyOpen>(lib,"il2cpp_domain_assembly_open");
  auto get_image = sym<AssemblyImage>(lib,"il2cpp_assembly_get_image");
  auto find_class = sym<ClassFromName>(lib,"il2cpp_class_from_name");
  auto find_method = sym<ClassMethod>(lib,"il2cpp_class_get_method_from_name");
  auto invoke = sym<Invoke>(lib,"il2cpp_runtime_invoke");
  if (!get_domain || !attach || !open_assembly || !get_image ||
      !find_class || !find_method || !invoke) {dlclose(lib);return -3;}
  void* domain=get_domain();
  if (!domain || !attach(domain)) {dlclose(lib);return -4;}
  void* assembly=open_assembly(domain,"UnityEngine.CoreModule");
  if (!assembly) assembly=open_assembly(domain,"UnityEngine.CoreModule.dll");
  if (!assembly) {dlclose(lib);return -5;}
  void* image=get_image(assembly);
  void* klass=image?find_class(image,"UnityEngine","Time"):nullptr;
  const void* method=klass?find_method(klass,"set_timeScale",1):nullptr;
  if (!method) {dlclose(lib);return -6;}
  void* args[]={&factor};
  void* exc=nullptr;
  invoke(method,nullptr,args,&exc);
  dlclose(lib);
  return exc?-7:0;
}
