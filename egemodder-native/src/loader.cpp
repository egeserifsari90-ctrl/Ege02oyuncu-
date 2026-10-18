// JNI proxy for the original Unity libmain (renamed liborig.so).
// Experimental: currently only targets armeabi-v7a.
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* reserved){
  void* original=dlopen("liborig.so",RTLD_NOW|RTLD_GLOBAL);
  if(!original){
    __android_log_print(ANDROID_LOG_ERROR,"EGEMODDER","liborig.so unavailable: %s",dlerror());
    return JNI_ERR;
  }
  auto old_init=reinterpret_cast<jint(*)(JavaVM*,void*)>(dlsym(original,"JNI_OnLoad"));
  if(!old_init)return JNI_ERR;
  jint status=old_init(vm,reserved);
  if(status==JNI_ERR)return status;
  void* plugin=dlopen("libegemodder.so",RTLD_NOW|RTLD_GLOBAL);
  if(plugin){
    auto start=reinterpret_cast<void(*)(void*)>(dlsym(plugin,"egemodder_start"));
    if(start)start(vm);
  }else{
    __android_log_print(ANDROID_LOG_WARN,"EGEMODDER","Game hooks inactive: %s",dlerror());
  }
  return status;
}
