// Egemodder Super Bear Adventure 13.0.8: game-specific IL2CPP hooks.
// WARNING: runtime behavior is not tested on a physical Android device.
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#ifdef __ANDROID__
#include <android/log.h>
#include <jni.h>
extern "C" int DobbyHook(void*, void*, void**);
#define EGLOG(...) __android_log_print(ANDROID_LOG_INFO,"EGEMODDER",__VA_ARGS__)
#else
#define EGLOG(...) ((void)0)
#endif

namespace {
std::atomic<bool> jump_enabled{false};
std::atomic<bool> hover_enabled{false};
std::atomic<bool> noclip_enabled{false};
std::atomic<float> time_scale{1.f};
std::atomic<bool> hooked{false};
std::atomic<bool> started{false};

using Jump = bool (*)(void*, const void*);
using Gravity = float (*)(void*, const void*);
using Collision = bool (*)(void*, void*, const void*);
using Update = void (*)(void*, const void*);
Jump orig_jump=nullptr;
Gravity orig_gravity=nullptr;
Collision orig_collision=nullptr;
Update orig_update=nullptr;

[[maybe_unused]] bool jump_hook(void* self,const void* method) {
  return jump_enabled.load() || (orig_jump ? orig_jump(self,method): false);
}
[[maybe_unused]] float gravity_hook(void* self,const void* method) {
  return hover_enabled.load()? 0.f : (orig_gravity?orig_gravity(self,method):0.f);
}
[[maybe_unused]] bool collision_hook(void* self,void* collider,const void* method) {
  return noclip_enabled.load()? false : (orig_collision?orig_collision(self,collider,method):false);
}
extern "C" int egemodder_apply_game_time_scale(float);
[[maybe_unused]] void update_hook(void* self,const void* method) {
  if(orig_update)orig_update(self,method);
  static float applied=1.f;
  float desired=time_scale.load();
  if(std::fabs(applied-desired)>=0.01f &&
     egemodder_apply_game_time_scale(desired)==0) applied=desired;
}

using DomainGet=void*(*)();
using Attach=void*(*)(void*);
using GetAssemblies=const void*const*(*)(void*,size_t*);
using GetImage=void*(*)(const void*);
using GetImageName=const char*(*)(const void*);
using GetClass=void*(*)(const void*,const char*,const char*);
using GetMethod=const void*(*)(void*,const char*,int);
using ParamCount=unsigned int(*)(const void*);
using ReturnType=const void*(*)(const void*);
using TypeName=char*(*)(const void*);
using Il2cppFree=void(*)(void*);
template<typename T> T sym(void* lib,const char* name) {
  return reinterpret_cast<T>(dlsym(lib,name));
}
struct Api {
  DomainGet domain; Attach attach; GetAssemblies assemblies; GetImage image;
  GetImageName image_name; GetClass klass; GetMethod method; ParamCount params;
  ReturnType result; TypeName type_name; Il2cppFree free;
};
[[maybe_unused]] bool expected_type(const Api& a,const void* m,const char* n) {
  if(!m||!a.result||!a.type_name||!a.free)return false;
  char* name=a.type_name(a.result(m));
  if(!name)return false;
  bool ok=std::strcmp(name,n)==0;
  a.free(name);
  return ok;
}
#ifdef __ANDROID__
bool hook_method(const Api& a,const void* image,const char* cls,
                 const char* name,int argc,const char* result,
                 void* detour,void** original) {
  void* k=a.klass(image,"",cls);
  const void* m=k?a.method(k,name,argc):nullptr;
  if(!m||a.params(m)!=static_cast<unsigned>(argc)||!expected_type(a,m,result)){
    EGLOG("Missing or invalid %s.%s",cls,name);return false;
  }
  // Current Unity 32-bit IL2CPP MethodInfo places methodPointer first.
  void* address=*reinterpret_cast<void*const*>(m);
  Dl_info loc={};
  if(!address||!dladdr(address,&loc)||!loc.dli_fname||
     !std::strstr(loc.dli_fname,"libil2cpp.so")) {
    EGLOG("Rejected non-IL2CPP method pointer %s.%s",cls,name);return false;
  }
  int err=DobbyHook(address,detour,original);
  EGLOG("Installed %s.%s: %d",cls,name,err);
  return err==0;
}
void* init_worker(void*) {
  for(int attempt=0;attempt<240;++attempt){
    void* lib=dlopen("libil2cpp.so",RTLD_NOW|RTLD_NOLOAD);
    if(!lib){usleep(250000);continue;}
    Api a={sym<DomainGet>(lib,"il2cpp_domain_get"),
      sym<Attach>(lib,"il2cpp_thread_attach"),
      sym<GetAssemblies>(lib,"il2cpp_domain_get_assemblies"),
      sym<GetImage>(lib,"il2cpp_assembly_get_image"),
      sym<GetImageName>(lib,"il2cpp_image_get_name"),
      sym<GetClass>(lib,"il2cpp_class_from_name"),
      sym<GetMethod>(lib,"il2cpp_class_get_method_from_name"),
      sym<ParamCount>(lib,"il2cpp_method_get_param_count"),
      sym<ReturnType>(lib,"il2cpp_method_get_return_type"),
      sym<TypeName>(lib,"il2cpp_type_get_name"),
      sym<Il2cppFree>(lib,"il2cpp_free")};
    if(!a.domain||!a.attach||!a.assemblies||!a.image||!a.image_name||
       !a.klass||!a.method||!a.params||!a.result||!a.type_name||!a.free){
      dlclose(lib);usleep(250000);continue;
    }
    void* domain=a.domain();
    if(!domain||!a.attach(domain)){dlclose(lib);usleep(250000);continue;}
    size_t count=0;
    auto list=a.assemblies(domain,&count);
    const void* game=nullptr;
    if(list&&count<1024){
      for(size_t i=0;i<count;++i){
        auto image=a.image(list[i]);
        const char* name=image?a.image_name(image):nullptr;
        if(name&&(!std::strcmp(name,"Assembly-CSharp")||
                  !std::strcmp(name,"Assembly-CSharp.dll"))) {game=image;break;}
      }
    }
    if(!game){dlclose(lib);usleep(250000);continue;}
    bool j=hook_method(a,game,"PlayerStateInAir","CanDoubleJump",0,"System.Boolean",
              reinterpret_cast<void*>(jump_hook),reinterpret_cast<void**>(&orig_jump));
    bool g=hook_method(a,game,"PlayerStateInAir","GetGravityForce",0,"System.Single",
              reinterpret_cast<void*>(gravity_hook),reinterpret_cast<void**>(&orig_gravity));
    bool c=hook_method(a,game,"PlayerControllerBase","IsColliderValidForCollisions",1,
              "System.Boolean",reinterpret_cast<void*>(collision_hook),
              reinterpret_cast<void**>(&orig_collision));
    bool u=hook_method(a,game,"PlayerControllerBase","Update",0,"System.Void",
              reinterpret_cast<void*>(update_hook),reinterpret_cast<void**>(&orig_update));
    hooked.store(j&&g&&c&&u);
    EGLOG("SBA hooks jump=%d gravity=%d collision=%d update=%d",j,g,c,u);
    dlclose(lib);
    return nullptr;
  }
  EGLOG("Timeout waiting for SBA IL2CPP runtime");
  return nullptr;
}
#endif
} // namespace

// Menu API: 1 hover/low-gravity, 2 unlimited double jump, 3 noclip, 4 time scale.
// A game UI must be added before these toggles are accessible from the screen.
extern "C" int egemodder_set_feature(int feature,float value){
  if(!std::isfinite(value))return -1;
  switch(feature){
    case 1:hover_enabled.store(value>0.5f);return 0;
    case 2:jump_enabled.store(value>0.5f);return 0;
    case 3:noclip_enabled.store(value>0.5f);return 0;
    case 4:
      if(value<0.25f||value>3.f)return -1;
      time_scale.store(value);return 0;
    default:return -1;
  }
}
extern "C" int egemodder_hook_state(){return hooked.load()?1:0;}
extern "C" void egemodder_start(void*){
#ifdef __ANDROID__
  if(started.exchange(true))return;
  pthread_t id;
  if(pthread_create(&id,nullptr,init_worker,nullptr)==0)pthread_detach(id);
  else started.store(false);
#endif
}

#ifdef __ANDROID__
extern "C" JNIEXPORT jint JNICALL Java_com_egemodder_Menu_nativeSetFeature(
    JNIEnv*, jclass, jint feature, jfloat value) {
  return egemodder_set_feature(feature,value);
}
#endif
