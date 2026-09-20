# NP-DOS Mimari Notları

## Genel tasarım

NP-DOS, BIOS’ın 16-bit gerçek mod arayüzüne dönmek yerine GRUB 2’nin sağladığı Multiboot2 girişinden sonra kendi 64-bit çalışma ortamını kurar. Bu karar, sistemin klasik DOS benzeri servis numaralarını korumasını ve aynı zamanda C ile ELF64 tabanlı yeni kodu doğrudan kullanmasını sağlar.

Önyükleme sırası şöyledir:

1. GRUB, Multiboot2 başlığını tanır ve `npdos.elf` imajını yaklaşık 1 MiB fiziksel adrese yükler.
2. `boot/boot.asm`, GRUB’un verdiği Multiboot2 bilgi adresini saklar.
3. İlk 1 GiB için 2 MiB sayfalardan oluşan identity map hazırlanır.
4. CR4.PAE, EFER.LME ve CR0.PG ayarlanır.
5. 64-bit GDT yüklenir ve far jump ile long mode’a geçilir.
6. C çekirdeği VGA, seri port, klavye, LPT, süreç tablosu, fiziksel bellek ve IDT’yi başlatır.
7. ATA cihazı varsa ext4 superblock’ü okunur.
8. Açılış durumu VGA’ya ve seri porta yazılır; TUI shell `A:\>` istemini açar.

## Adresleme ve bellek

İlk aşama en fazla 1 GiB fiziksel adresi bitmap ile takip eder. Her bit bir 4 KiB sayfayı temsil eder. Multiboot2 memory map içindeki `type=1` alanlar serbest kabul edilir. İlk 1 MiB, kernel image, sayfa tabloları ve stack ayrılmıştır.

Sayfa ayırıcı iki temel işlem sağlar:

- `pmm_alloc_page`, tek bir fiziksel 4 KiB sayfa ayırır.
- `pmm_alloc_pages`, bitişik bir fiziksel sayfa aralığı ayırır.

Bu katman sayfa tablolarını değiştirmez ve süreç adres alanı kurmaz. Bu nedenle stage 0.1’deki bellek yönetimi fiziksel rezervasyon seviyesindedir.

## Sürücü katmanı

VGA sürücüsü doğrudan `0xB8000` metin belleğine yazar ve CRT cursor portlarını günceller. PS/2 sürücüsü 8042 durum portunu polling ile izler; IRQ tabanlı input kuyruğu henüz yoktur. ATA sürücüsü primary channel için PIO komutlarını kullanır. Seri port COM1’de 38400 baud, 8N1 ayarıyla başlatılır. LPT1 sürücüsü veri ve strobe portlarını kullanır.

## Kesme katmanı

`kernel/idt.c`, 256 girişlik IDT kurar. INT 10h, 13h, 14h, 16h ve 17h, `boot/interrupts.asm` içindeki korumalı giriş stublarına bağlanır. Stub’lar genel amaçlı register’ları saklar, C servis fonksiyonunu çağırır ve `iretq` ile döner.

Bu vektörler gerçek BIOS ROM çağrıları değildir. Boot aşamasında CPU artık long mode’dadır. Bunlar NP-DOS’un kendi ABI’sini sağlayan software interrupt kapılarıdır.

## ext4 katmanı

`fs/ext4.c`, ATA’dan 2. ve 3. sektörleri okuyarak ext4 superblock alanını alır. Magic alanı `0xEF53` ise mount durumu tutulur ve block size/block count okunabilir. Dizin ağacı, inode, extent, journaling ve yazma işlemleri henüz bu katmanda yoktur.

## Süreç tablosu

`kernel/process.c` en fazla 16 kayıt tutar. PID 0 kernel için `READY` durumda başlatılır. Kayıtlar PID, state, entry adresi ve kısa isim içerir. Bir ELF yükleyici kaydı bu tabloya bağlanabilecek temel API’ye sahiptir; ancak scheduler, register context, ring 3 geçişi ve page-table adres alanı yoktur.

Bu nedenle shell’de `PROC` çıktısındaki “context switching is not enabled” ifadesi bir özellik eksikliğini açıkça belirtir.

## NP_ELF_LOADER

`kernel/elf_loader.c` şu koşulları doğrular:

- ELF magic, class, endianness ve version.
- x86-64 machine id.
- ET_EXEC veya ET_DYN tipleri.
- Program header tablosunun dosya sınırları içinde bulunması.
- En fazla 16 adet, boş olmayan `PT_LOAD` segmenti.
- `filesz <= memsz` ve file offset aralığı.

Her segment için fiziksel sayfa ayırıcı çağrılır. Ayrılan alan sıfırlanır, dosya içindeki segment bytes kopyalanır ve `elf_image_t` içinde sanal adres, yüklenen adres, boyut ve bayraklar tutulur.

Yükleyici gerçek bir segment yükleme çekirdeğidir. Fakat henüz kullanıcı adres alanı, relocation, dynamic linker, syscall ABI veya `iretq` ile ring 3’e geçiş eklenmemiştir.

## Referanslar

[1]: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html "GNU GRUB Multiboot2 Specification"
[2]: https://refspecs.linuxfoundation.org/elf/gabi4+/contents.html "System V ABI ELF Specification"
[3]: https://docs.kernel.org/filesystems/ext4/index.html "Linux ext4 Documentation"
[4]: https://wiki.osdev.org/Interrupt_Descriptor_Table "OSDev Interrupt Descriptor Table"
