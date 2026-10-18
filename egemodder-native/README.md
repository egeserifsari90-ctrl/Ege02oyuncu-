# Egemodder native – Super Bear Adventure 13.0.8

**This is native proof-of-concept source, NOT a finished mod APK.** It is separate from the standalone WebView demo.

Verified from the user-provided XAPK:
- Base package: com.Earthkwak.Platformer.apk plus config.armeabi_v7a.apk.
- Unity IL2CPP runtime is in lib/armeabi-v7a/libil2cpp.so.
- Target runtime exports il2cpp_domain_get, il2cpp_thread_attach, il2cpp_domain_assembly_open, il2cpp_assembly_get_image, il2cpp_class_from_name, il2cpp_class_get_method_from_name, il2cpp_runtime_invoke.
- The native function egemodder_apply_game_time_scale(factor) can call UnityEngine.Time.set_timeScale (when integrated and invoked on Unity main thread). Game-wide speed is NOT verified player movement speed.
- Syntax checked locally with g++ -std=c++17 -fPIC -Wall -Wextra -Werror -fsyntax-only.

Required before calling anything a working SBA mod: load native library within actual game process, implement game-specific flying/unlimited jumps/no-clip/player-speed hooks, wire the Egemodder 1234 on-screen menu to hooks, re-sign appropriate APK splits, and verify all effects on an Android device. This source has not been injected or tested on device.
