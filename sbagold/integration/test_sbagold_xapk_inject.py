import io
import json
import pathlib
import tempfile
import unittest
import zipfile
from sbagold_xapk_inject import stage

def zipbytes(files):
    out=io.BytesIO()
    with zipfile.ZipFile(out,"w") as z:
        for name, data in files.items():
            z.writestr(name,data)
    return out.getvalue()

class TestPrivateXapkPatching(unittest.TestCase):
    def test_inserts_components_preserves_orig(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)
            base=zipbytes({"AndroidManifest.xml":b"manifest","classes.dex":b"dex"})
            arm=zipbytes({"lib/armeabi-v7a/libmain.so":b"original","lib/armeabi-v7a/libil2cpp.so":b"il2cpp"})
            (p/"input.xapk").write_bytes(zipbytes({
                "manifest.json":json.dumps({"package_name":"com.Earthkwak.Platformer","version_name":"13.0.8"}),
                "com.Earthkwak.Platformer.apk":base,"config.armeabi_v7a.apk":arm}))
            (p/"hooks.zip").write_bytes(zipbytes({
                "build-armv7/libmain.so":b"proxy",
                "build-armv7/libegemodder.so":b"mod",
                "build-menu/dex/classes5.dex":b"dex5"}))
            outputs=stage(p/"input.xapk",p/"hooks.zip",p/"out")
            with zipfile.ZipFile(outputs[0]) as z:
                self.assertEqual(z.read("classes5.dex"),b"dex5")
                self.assertEqual(z.read("classes.dex"),b"dex")
                self.assertIsNone(z.testzip())
            with zipfile.ZipFile(outputs[1]) as z:
                self.assertEqual(z.read("lib/armeabi-v7a/libmain.so"),b"proxy")
                self.assertEqual(z.read("lib/armeabi-v7a/liborig.so"),b"original")
                self.assertEqual(z.read("lib/armeabi-v7a/libegemodder.so"),b"mod")
                self.assertEqual(z.read("lib/armeabi-v7a/libil2cpp.so"),b"il2cpp")
                self.assertIsNone(z.testzip())

    def test_refuses_wrong_version(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)
            (p/"input.xapk").write_bytes(zipbytes({"manifest.json":json.dumps({
                "package_name":"com.Earthkwak.Platformer","version_name":"13.0.7"})}))
            (p/"hooks.zip").write_bytes(zipbytes({"readme.txt":b"x"}))
            with self.assertRaisesRegex(ValueError,"Wrong package or version"):
                stage(p/"input.xapk",p/"hooks.zip",p/"out")
if __name__=="__main__":
    unittest.main()
