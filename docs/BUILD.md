# NP-DOS Derleme ve Test Kılavuzu

## Gereksinimler

Ubuntu 24.04 üzerinde aşağıdaki araçlar yeterlidir:

```sh
sudo apt-get install gcc binutils nasm grub-pc-bin grub-common xorriso qemu-system-x86 qemu-utils mtools ovmf
```

Çapraz compiler zorunlu değildir. Kod host GCC ile `-ffreestanding`, `-mno-red-zone`, `-mno-sse`, `-mno-sse2`, `-mno-mmx` ve kernel code model kullanılarak derlenir. SIMD kullanılmamasının nedeni, boot sırasında CR4.OSFXSR açılmadan SSE talimatı çalıştırmanın exception üretmesidir.

## ISO üretimi

```sh
cd np-dos
make clean
make all
```

`make all` önce kernel ELF’ini linkler, sonra GRUB rescue ISO’sunu oluşturur ve `grub-file --is-x86-multiboot2` ile kernel başlığını doğrular. Aynı hedef C ve ASM ELF örneklerini de linkler.

## Otomatik test

```sh
make test
```

Test komutları şunlardır:

1. `tests/elf-test.sh`, `hello-asm.elf` ve `hello-c.elf` dosyalarının ELF64, x86-64 ve PT_LOAD özelliklerini kontrol eder.
2. `tests/boot-test.sh`, ISO’yu QEMU’ya CD-ROM olarak bağlar, seri günlük dosyasına boot çıktısını kaydeder ve kernel’in long mode’a ulaştığını kontrol eder.
3. Manuel ekran doğrulaması QEMU HMP `screendump` komutuyla yapılır. Görseller PNG’ye çevrilir ve kaynak ağacında saklanır.

## QEMU shell testi

Görünür curses testi için QEMU monitörünü UNIX soketine bağlamak mümkündür:

```sh
qemu-system-x86_64 \
  -cdrom build/np-dos.iso \
  -m 128M \
  -display curses \
  -monitor unix:build/qemu-mon.sock,server,nowait \
  -serial file:build/qemu-serial.log
```

HMP üzerinden gecikmeli tuş olayları gönderildiğinde shell komutları gerçek PS/2 yolundan test edilebilir. Son doğrulama kaydı bu yöntemle `VER` ve `PROC` komutlarını çalıştırmıştır.

## Dosya bütünlüğü kontrolü

```sh
file build/np-dos.iso build/npdos.elf
readelf -h build/npdos.elf
readelf -l build/hello-asm.elf
readelf -l build/hello-c.elf
```

## Referanslar

[1]: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html "GNU GRUB Multiboot2 Specification"
[2]: https://www.qemu.org/docs/master/system/index.html "QEMU System Emulation Documentation"
[3]: https://refspecs.linuxfoundation.org/elf/gabi4+/contents.html "System V ABI ELF Specification"
