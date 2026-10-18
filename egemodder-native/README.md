# Egemodder 13.0.8 — IL2CPP bridge patch

This is a **patch for an Android ARMv7 native library**, not a working modded Super Bear Adventure APK.

## Improvements
- Rejects out-of-range, NaN, and infinite time-scale values before calling Unity.
- Resolves IL2CPP APIs in a library that must already be loaded into the same process.
- Calls UnityEngine.Time.set_timeScale through IL2CPP method reflection.
- Adds status strings for diagnostics and cleanup of the dlopen handle on every path.
- Adds mocked IL2CPP tests for rejection, argument passing, and managed exceptions. The mock intentionally does not claim compatibility with the actual game.
- Android NDK target: armeabi-v7a, API 23; GitHub Actions compiles/tests both host mock and Android library.

## Missing for real SBA game APK
- A mechanism to load libegemodder.so into the game process in a compatible and safe manner.
- Game-specific player ability methods and their runtime linkage for fly, noclip, unlimited jump, and movement speed.
- An actual in-game password-protected mod menu that triggers these functions.
- APK/split packaging, signing, installation and on-device gameplay verification.

Passing CI means the C++ library **compiles and mock tests pass**, not that any cheat works in the game.
