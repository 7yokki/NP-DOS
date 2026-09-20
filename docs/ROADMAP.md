# NP-DOS Yol Haritası

## Stage 0.2 — Dosya ve yükleme akışı

Öncelik, gerçek bir block device veya GRUB module üzerinden dosya imajı almak ve ext4 superblock’ten sonra group descriptor, inode, extent ve directory traversal eklemektir. Böylece `A:\>` promptu ISO veya disk içindeki dosyaları okuyabilir.

NP_ELF_LOADER daha sonra dosya sistemi yolundan ELF byte dizisi alacak, program başlıklarını map edecek ve süreç tablosuna kayıt açacaktır. Bu aşamada yalnızca kernel seviyesinde yükleme doğrulaması yapılacaktır.

## Stage 0.3 — Zamanlayıcı ve kullanıcı modu

PIT veya APIC timer, TSS, kernel stack, ring 3 code/data segmentleri, syscall kapısı ve adres alanı tablosu eklenecektir. Süreç yapısı register context ve page-table kökünü taşıyacaktır. Bu adım tamamlanmadan ELF uygulamasının CPU üzerinde çalıştırıldığı söylenmeyecektir.

## Stage 0.4 — DOS uyumlu dosya işlemleri

Dosya açma, okuma, yazma, directory enumeration ve redirection komutları eklenebilir. INT 21h benzeri bir dosya ve process API’si, mevcut INT 10h/13h/14h/16h/17h servislerinin yanına eklenebilir.

## Stage 1 — TUI sürücüsü

VGA text sürücüsünün üstüne pencere, menü, renk teması, input focus ve terminal buffer katmanı eklenebilir. Bu katman eski DOS TUI hissini koruyacak ancak 64-bit uygulama ABI’siyle uyumlu kalacaktır.

Her aşama QEMU boot, seri günlük, görsel ekran dökümü ve mümkün olduğunda negatif testlerle doğrulanacaktır.
