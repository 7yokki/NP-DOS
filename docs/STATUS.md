# NP-DOS 0.1 Durum Raporu

**Doğrulanan hedef:** x86-64, GRUB 2 Multiboot2, QEMU 8.2.2, 128 MiB RAM, CD-ROM ISO boot.

**Çalışan çekirdek yolu:** GRUB → Multiboot2 → 32-bit giriş → 2 MiB identity map → long mode → C kernel → IDT → VGA TUI.

**Çalışan servisler:** VGA metin sürücüsü, imleç, PS/2 polling, Türkçe Q fiziksel eşleme, COM1, LPT1, ATA PIO komut yolu, Multiboot2 fiziksel bellek bitmap’i, ext4 superblock yoklaması, 16 yuvalı süreç kaydı ve ELF64 PT_LOAD yükleme fonksiyonları.

**QEMU sonucu:** `make test` başarılıdır. VGA ekran görüntüsünde 32,457 serbest sayfa, native INT satırı, süreç katmanı ve `A:\>` prompt görülmüştür. Ayrı gecikmeli HMP testi `VER` ve `PROC` komutlarını çalıştırmıştır.

**Dürüst kapsam:** QEMU varsayılan testinde ATA diski yoktur. Bu nedenle ext4 mount sonucu `[NOT MOUNTED]` olarak kalır. Tam ext4 dizin ve inode işlemleri, user mode, scheduler, context switch, page-table address space, dynamic linking ve uygulama ELF’lerini ISO içinden otomatik keşfedip çalıştırma henüz yoktur.

Bu rapor, yapılmamış bir özelliği yapılmış gibi göstermemek için sınırlamaları açıkça ayırır.
