#!/usr/bin/env bash
# Sign both SBA split APKs with a *private* test key after zipalign.
# Usage: bash sbagold/integration/sign_split_apks.sh INPUT_DIR OUTPUT_DIR KEYSTORE
set -euo pipefail
if [[ $# -ne 3 ]]; then echo "Usage: $0 INPUT_DIR OUTPUT_DIR KEYSTORE" >&2; exit 2; fi
INPUT_DIR="$1"; OUT_DIR="$2"; KEYSTORE="$3"
: "${ANDROID_HOME:?Android SDK required}"
: "${SBAGOLD_KEY_PASSWORD:?Set SBAGOLD_KEY_PASSWORD in your local environment}"
: "${SBAGOLD_KEY_ALIAS:=sbagold}"
mkdir -p "$OUT_DIR"
BUILD_TOOLS="$(find "$ANDROID_HOME/build-tools" -mindepth 1 -maxdepth 1 -type d | sort -V | tail -1)"
test -x "$BUILD_TOOLS/zipalign" && test -x "$BUILD_TOOLS/apksigner"
for spec in "base-UNSIGNED.apk:base-SIGNED.apk" "config.armeabi_v7a-UNSIGNED.apk:config.armeabi_v7a-SIGNED.apk"; do
    source="${spec%%:*}"
    target="${spec#*:}"
    test -s "$INPUT_DIR/$source" || { echo "Missing $source" >&2; exit 1; }
    "$BUILD_TOOLS/zipalign" -f -p 4 "$INPUT_DIR/$source" "$OUT_DIR/${target%.apk}.aligned.apk"
    "$BUILD_TOOLS/apksigner" sign \
      --ks "$KEYSTORE" --ks-key-alias "$SBAGOLD_KEY_ALIAS" \
      --ks-pass env:SBAGOLD_KEY_PASSWORD --key-pass env:SBAGOLD_KEY_PASSWORD \
      --out "$OUT_DIR/$target" "$OUT_DIR/${target%.apk}.aligned.apk"
    "$BUILD_TOOLS/apksigner" verify --verbose "$OUT_DIR/$target"
    rm "$OUT_DIR/${target%.apk}.aligned.apk"
done
echo "Signed both splits; install together using: adb install-multiple -r base-SIGNED.apk config.armeabi_v7a-SIGNED.apk"
echo "WARNING: Different certificate than official game; back up saves and never uninstall the original without backup."
