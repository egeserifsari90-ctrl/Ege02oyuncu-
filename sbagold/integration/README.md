# SBAGOLD 13.0.8 private split-APK preparation

The supplied Super Bear Adventure 13.0.8 XAPK contains TWO Android splits: `com.Earthkwak.Platformer.apk` and `config.armeabi_v7a.apk`. It must not be treated as a single base APK.

Run locally (never upload the copyrighted game package or signing keys to this public repository):

```sh
python sbagold/integration/sbagold_xapk_inject.py path/to/SBA-13.0.8.xapk path/to/Egemodder-GameHooks-ARMv7-EXPERIMENTAL-NOT-APK.zip output
python -m unittest discover -s sbagold/integration -p 'test_*.py' -v
```

The script preserves original ARMv7 `libmain.so` as `liborig.so`, inserts proxy `libmain.so` and `libegemodder.so`, and adds `classes5.dex` to base APK. It validates package/version/CRC and aborts on collisions. Produces TWO **UNSIGNED** split APKs, not an installable APK.

**Important:** Both splits must be ZIP-aligned and signed with the SAME private Android key using Android build tools before installing as a split set; game data may require additional compatibility handling. A re-signed build cannot update the official installation with a different certificate; back up saves. No Android emulator/physical game test is claimed. The Unity/IL2CPP hook ABI and in-game menu launch remain experimental.

GitHub CI uses ONLY tiny synthetic fixture files, never the real proprietary game.

## Standard APK signing (local only)

After preparing the two split APKs, use Android SDK's **zipalign** and **apksigner** so both have the same signing identity. Never commit the keystore, game APK or proprietary game assets.

```bash
export ANDROID_HOME="$HOME/Android/Sdk"
export SBAGOLD_KEY_PASSWORD='your-private-password'
keytool -genkeypair -keystore sbagold-testing.jks -alias sbagold -keyalg RSA -keysize 3072 -validity 3650 -storepass "$SBAGOLD_KEY_PASSWORD" -keypass "$SBAGOLD_KEY_PASSWORD" -dname "CN=SBAGOLD local test"
bash sbagold/integration/sign_split_apks.sh output output/signed sbagold-testing.jks
adb install-multiple output/signed/base-SIGNED.apk output/signed/config.armeabi_v7a-SIGNED.apk
```

*Install-multiple is **not** an in-game test. An actual Android device or emulator must launch the game and verify the mod menu and each feature. Do not install over the official app when signing certificates differ. There is no mechanism for GitHub's public runner to access your private XAPK unless you supply it to a safe private runtime.*
