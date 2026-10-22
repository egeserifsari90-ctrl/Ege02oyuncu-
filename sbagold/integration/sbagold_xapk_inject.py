#!/usr/bin/env python3
"""Privately prepare SBA 13.0.8 split APKs with experimental native hooks.
Output APKs are UNSIGNED and cannot be installed until aligned and signed.
"""
import argparse
import json
import pathlib
import zipfile

PKG = "com.Earthkwak.Platformer"
ARM = "lib/armeabi-v7a/"

def repack(source, dest, replacements):
    with zipfile.ZipFile(source) as old, zipfile.ZipFile(dest, "w") as new:
        if old.testzip() is not None:
            raise ValueError("Invalid source APK CRC")
        names=set()
        for info in old.infolist():
            name=info.filename
            if name in names:
                raise ValueError("Duplicate file " + name)
            names.add(name)
            if name.startswith("META-INF/") and (name.endswith((".RSA",".DSA",".SF")) or name=="META-INF/MANIFEST.MF"):
                continue
            new.writestr(info,replacements.pop(name,old.read(name)))
        for name, data in replacements.items():
            new.writestr(name,data,compress_type=zipfile.ZIP_DEFLATED)
    with zipfile.ZipFile(dest) as check:
        if check.testzip() is not None:
            raise ValueError("Output APK CRC failure")

def stage(xapk, patch, output_dir):
    output_dir=pathlib.Path(output_dir)
    output_dir.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(xapk) as original, zipfile.ZipFile(patch) as hooks:
        if original.testzip() is not None or hooks.testzip() is not None:
            raise ValueError("Input CRC failure")
        m=json.loads(original.read("manifest.json"))
        if m.get("package_name")!=PKG or m.get("version_name")!="13.0.8":
            raise ValueError("Wrong package or version")
        base="com.Earthkwak.Platformer.apk"
        split="config.armeabi_v7a.apk"
        if base not in original.namelist() or split not in original.namelist():
            raise ValueError("Missing expected APK split")
        keys=["build-armv7/libmain.so","build-armv7/libegemodder.so","build-menu/dex/classes5.dex"]
        if not all(k in hooks.namelist() for k in keys):
            raise ValueError("Incomplete native hook package")
        src_base=output_dir/"source-base.apk"
        src_arm=output_dir/"source-arm.apk"
        src_base.write_bytes(original.read(base))
        src_arm.write_bytes(original.read(split))
        with zipfile.ZipFile(src_base) as z:
            if "classes5.dex" in z.namelist():
                raise ValueError("classes5.dex already exists")
        with zipfile.ZipFile(src_arm) as z:
            if ARM+"liborig.so" in z.namelist():
                raise ValueError("Original loader backup already exists")
            libmain=z.read(ARM+"libmain.so")
            z.getinfo(ARM+"libil2cpp.so")
        out_base=output_dir/"base-UNSIGNED.apk"
        out_arm=output_dir/"config.armeabi_v7a-UNSIGNED.apk"
        repack(src_base,out_base,{"classes5.dex":hooks.read(keys[2])})
        repack(src_arm,out_arm,{
            ARM+"liborig.so":libmain,
            ARM+"libmain.so":hooks.read(keys[0]),
            ARM+"libegemodder.so":hooks.read(keys[1]),
        })
        return [str(out_base),str(out_arm)]

if __name__=="__main__":
    p=argparse.ArgumentParser()
    p.add_argument("xapk")
    p.add_argument("patch")
    p.add_argument("output_dir")
    a=p.parse_args()
    print("\n".join(stage(a.xapk,a.patch,a.output_dir)))
    print("UNSIGNED SPLITS — BOTH REQUIRE ALIGNMENT AND SAME-KEY SIGNING")
