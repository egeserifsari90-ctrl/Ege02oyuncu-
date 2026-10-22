# SBAGOLD experimental patch
Standalone testable **logic patch** for Egemodder SBA 13.0.8.

Features: unlimited double-jump flag, zero-gravity flag, noclip collision flag and time-scale range validation, with reset to safe defaults. SBAGOLD defaults OFF for all game-affecting flags.

This is **not an installable game APK**. The in-game IL2CPP hooks may differ from real SBA 13.0.8, and on-device behavior has NOT been tested. This branch adds pure host-unit tests that can be run using `cmake -S sbagold -B sbagold/build && cmake --build sbagold/build && ctest --test-dir sbagold/build --output-on-failure`. No proprietary game assets are redistributed.
