# Super Bear Adventure 13.0.8 — Egemodder oyun içi deneysel patch

Bu klasördeki kaynaklar, kullanıcı tarafından sağlanan 13.0.8 ARMv7 XAPK paketinin kişisel modifikasyonu için geliştirilmiştir.

## Oluşturulan bileşenler

- `src/player_hooks.cpp`: IL2CPP reflection ve Dobby aracılığıyla `PlayerStateInAir.CanDoubleJump`, `PlayerStateInAir.GetGravityForce`, `PlayerControllerBase.IsColliderValidForCollisions` ve `PlayerControllerBase.Update` hook girişimleri. Yöntem imzaları çalışma anında doğrulanır.
- `src/loader.cpp`: Unity orijinal `libmain.so` yükleyicisinin `liborig.so` adıyla korunmasını ve JNI üzerinden Egemodder kütüphanesinin yüklenmesini amaçlayan proxy.
- `java/com/egemodder/Menu.java`: Unity Activity üzerinde Egemodder düğmesi, PIN 1234 girişi ve seçenek arayüzü. `nativeSetFeature` ile C++ işaretlerine bağlanır.
- `egemodder.cpp`: Unity `Time.timeScale` IL2CPP bağlantı prototipi.
- `.github/workflows/egemodder-sba-hooks-armv7.yml`: host CTest, Android NDK ARMv7 ve Android DEX derleme doğrulamaları.
- Derleme çıktıları: `libmain.so`, `libegemodder.so`, `classes5.dex` (oyun dosyası değil).

## Deneysel durum / güvenlik

**Oyun içi çalıştığı doğrulanmış değildir.** GitHub Actions başarı sonucu yalnızca kod derlenmesi ve taklit IL2CPP testinin geçtiğini kanıtlar. Oyun üzerinde gerçek açılış, UI görünümü, zıplama, yerçekimi ve çarpışma sonuçları Android cihaz/emülatörde henüz test edilmedi. No Clip zeminden düşürmeye sebep olabilir.

Oyunun kendisi telifli olduğundan özgün oyunun APK/XAPK dosyası bu herkese açık GitHub deposuna eklenmemiştir. Deneysel paket yalnızca kullanıcının sağladığı 13.0.8 XAPK üzerinden özel çalışma ortamında yeniden paketlendi ve APK v2 ile imzalandı. Orijinal imzalı oyun üzerine kurulamaz; kayıtlar yedeklenmeden kaldırılmamalıdır.

**Bilinmeyenler:** Oyunun imza korumaları, Unity method adreslerinin çalışma zamanı uyumu, hook detour çağrı ABI'si, özel sürümde sınıf yükleyicisinin menüyü bulup bulamadığı, uygulamanın ARMv7 için cihaz desteği.

## Henüz eksik
Gerçek fiziksel uçuş (sıfır yerçekimiyle havada kalma prototipi var), oyunda güvenilir sınırsız zıplama ve No Clip testi, cihaz üzerinde açılış/doğrulama, imza korumaları ve kalite güvencesi.
