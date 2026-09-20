# No Problem DOS (NP-DOS)

**No Problem Disk Operating System**, kısa adıyla **NP-DOS**, klasik DOS çalışma modelini 64-bit x86 üzerinde yeniden kuran küçük bir işletim sistemi çekirdeğidir. Proje, ayrı bir ürün olarak dağıtılan monolitik bir kernel yerine bütünleşik bir DOS benzeri sistem sunar. Çekirdek vardır; ancak kullanıcıya ayrıca paketlenmiş bir kernel ürünü olarak ayrıştırılmaz.

> **Bu proje Yapay zeka tarafından yürütülmektedir.**

Projenin geliştiren çatısı **No Problem Family**, üreticisi **No Problem**, yazarı **Manus.AI** ve seri adı **No Problem DOS (NP-DOS)** olarak tanımlanmıştır. Kaynak kodu **MIT Lisansı** ile dağıtılır.

## Mevcut sürüm

Bu teslim, **NP-DOS 0.1 stage 0.1** olarak adlandırılır. ISO, GRUB 2 Multiboot2 aracılığıyla yüklenir. Boot kodu NASM ile yazılmıştır. Çekirdek ve sürücüler freestanding C ile, kesme girişleri NASM ile derlenir. Çekirdek 32-bit protected mode’dan kendi sayfa tablolarını kurarak x86-64 long mode’a geçer.

Mevcut ISO QEMU’da açılır ve `A:\>` istemine ulaşır. VGA metin arayüzü 80×25’tir. PS/2 klavye tarama kodları Türkçe Q fiziksel yerleşimine göre eşlenir. VGA ROM’unun 8-bit doğası nedeniyle Türkçe banner satırı görüntüde ASCII fallback ile gösterilir; seri günlükte UTF-8 metin korunur.

## Uygulanan bileşenler

| Bileşen | Durum | Açıklama |
| --- | --- | --- |
| GRUB 2 / Multiboot2 | Hazır | ISO içinden `npdos.elf` yükler. |
| 64-bit giriş | Hazır | PML4, PDP, 2 MiB sayfalar, GDT ve long mode geçişi uygulanır. |
| VGA metin sürücüsü | Hazır | Karakter, satır, scroll, renk ve imleç yönetimi vardır. |
| PS/2 klavye | Hazır | BIOS tarzı polling ve Türkçe Q ASCII eşleme vardır. |
| INT 10h | Hazır | `AH=0Eh`, `AL=karakter` ile VGA çıktısı verir. |
| INT 13h | Hazır | ATA PIO sürücüsüne `AH=02` okuma ve `AH=03` yazma yolları bağlanmıştır. |
| INT 14h | Hazır | COM1 seri karakter gönderimi için `AH=01` desteklenir. |
| INT 16h | Hazır | Non-blocking PS/2 karakter polling yapılır. |
| INT 17h | Hazır | LPT1 karakter gönderimi için `AH=00` desteklenir. |
| ATA PIO | Hazır | Primary ATA kanalında tek sektör döngülü PIO okuma/yazma uygulanır. |
| ext4 | Kısmi | Superblock okunur, magic doğrulanır ve mount durumu raporlanır. Dizin/inode işlemleri sonraki aşamadadır. |
| Bellek | Hazır | Multiboot2 bellek haritasından fiziksel sayfa bitmap’i kurulur. |
| Süreç | Kısmi | 16 yuvalı PID ve durum tablosu vardır. Scheduler ve context switch yoktur. |
| NP_ELF_LOADER | Kısmi | ELF64 ET_EXEC/ET_DYN, x86-64, PT_LOAD segmentleri doğrulanır ve sayfalara yüklenir. Ring 3’e geçiş yoktur. |
| TUI shell | Hazır | `HELP`, `CLS`, `VER`, `MEM`, `DRV`, `ELF`, `PROC`, `REBOOT` komutları vardır. |

Buradaki **Hazır** ifadesi, ilgili kodun yazıldığı ve mevcut test kapsamı içinde çalıştığı anlamına gelir. Örneğin ext4 sürücüsü gerçek bir superblock okuyabilir; ancak henüz tam dosya sistemi işlemleri sunmaz. Bu sınırlar bilerek saklanmamıştır.

## Derleme

Ubuntu üzerinde gerekli paketler kurulduktan sonra:

```sh
make clean
make all
```

Bu işlem şunları üretir:

```text
build/np-dos.iso
build/npdos.elf
build/npdos.debug
build/hello-asm.elf
build/hello-c.elf
```

QEMU ile seri günlük ve VGA ekranı olmadan çalıştırmak için:

```sh
make run
```

Testleri çalıştırmak için:

```sh
make test
```

`make test`, Multiboot2 ELF doğrulamasını, ASM ve C örneklerinin ELF64 biçim kontrolünü ve QEMU long-mode boot seri günlük kontrolünü çalıştırır.

## QEMU doğrulaması

Son doğrulama kaydı [`screenshots/verification-findings-v02.md`](screenshots/verification-findings-v02.md) dosyasındadır. Son ekran görüntüsü [`build/screenshot-final.png`](build/screenshot-final.png) içindedir. Bu görüntü QEMU içinde `proc` komutunun çalıştığını, `PID 0 READY kernel` kaydını ve shell’in tekrar isteme döndüğünü gösterir.

CD-ROM ile açılan varsayılan QEMU testinde ATA primary disk yoktur. Bu nedenle görüntüde `DISK: ATA PIO primary [--]` ve `FS: ext4 superblock [NOT MOUNTED]` görülmesi beklenen negatif test sonucudur. Bu sonuç, ext4’ün başarıyla mount edildiği iddiası değildir.

## Önemli kapsam sınırları

NP-DOS şu anda gerçek donanım BIOS çağrılarını yeniden kullanmaz. GRUB’dan sonra CPU long mode’a geçtiği için INT numaraları NP-DOS’un kendi 64-bit DOS-benzeri yazılım vektörleridir. Bu seçim, klasik API hissini korurken modern kodun doğrudan kontrolünü sağlar.

NP_ELF_LOADER, dosyadan ELF yükleme çekirdeğini sağlar ancak bu aşamada kullanıcı modu, ring 3, adres alanı izolasyonu, syscall kapısı ve process context switch uygulanmamıştır. Bu yüzden örnek C ve ASM ELF dosyaları host tarafında gerçek ELF64 olarak linklenir ve yükleyici format/segment düzeyinde hazırdır; ISO içinden henüz kullanıcı süreci olarak çalıştırıldıkları iddia edilmez.

## Dizin yapısı

```text
boot/       Multiboot2 giriş, GDT, sayfa geçişi, kesme stubları, GRUB ayarı
kernel/     çekirdek, IDT, bellek, süreç tablosu, ELF yükleyici
drivers/    VGA, PS/2, ATA PIO, COM1, LPT1
fs/         ext4 superblock katmanı
include/    çekirdek ve sürücü başlıkları
lib/        freestanding bellek yardımcıları
apps/       C ve ASM ELF64 örnekleri
tests/      host ve QEMU doğrulama betikleri
docs/       mimari, ABI, build, yol haritası ve doğrulama açıklamaları
screenshots/ doğrulama bulguları
```

## Lisans ve isimlendirme

Kaynak kodu [`LICENSE`](LICENSE) dosyasındaki MIT Lisansı ile yayımlanır. NP-DOS adı ve ürün metadatası kaynak ağacında ve açılış ekranında aynı biçimde tutulur.

## Referanslar

[1]: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html "GNU GRUB Multiboot2 Specification"
[2]: https://refspecs.linuxfoundation.org/elf/gabi4+/contents.html "System V ABI ELF Specification"
[3]: https://docs.kernel.org/filesystems/ext4/index.html "Linux ext4 Documentation"
[4]: https://wiki.osdev.org/Interrupt_Descriptor_Table "OSDev Interrupt Descriptor Table"
