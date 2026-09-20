# NP-DOS Doğrulama Kaydı

## Kayıt özeti

Bu kayıt, NP-DOS 0.1 kaynak ağacındaki son değişikliklerden sonra alınmıştır. Doğrulama yöntemi QEMU 8.2.2, GRUB rescue ISO, host `readelf` kontrolleri ve QEMU HMP ekran dökümleridir.

## Otomatik sonuçlar

```text
make clean && make test

PASS ELF64 build/hello-asm.elf
PASS ELF64 build/hello-c.elf
PASS QEMU long-mode boot and serial diagnostics
PASS grub-file --is-x86-multiboot2 build/npdos.elf
```

Kernel linki, ISO üretimi, GRUB Multiboot2 tanıması ve iki farklı kaynak dilindeki ELF örneği aynı komut zincirinde tamamlanmıştır.

## Görsel sonuçlar

Son QEMU ekranı [`build/screenshot-final.png`](../build/screenshot-final.png) dosyasındadır. Ekranda şu gerçek durumlar görülebilir:

- `BOOT: Multiboot2 / x86_64 long mode [OK]`
- `MEM: page allocator 32457 free pages`
- `ELF: NP_ELF_LOADER ELF64 ET_EXEC/ET_DYN parser [LINKED]`
- `PROC: 16-slot process registry / no context switch [READY]`
- `INT 10h output: . | INT 16h software-vector smoke test [OK]`
- `A:\>` shell prompt

Gecikmeli HMP keyboard testinin [`build/screenshot-final.png`](../build/screenshot-final.png) çıktısında `proc` komutu, süreç sayısı 1, PID 0 ve READY kernel satırı görülür.

## Negatif ve sınır sonuçları

QEMU testi yalnızca ISO CD-ROM bağlar. Primary ATA disk olmadığı için `ATA PIO primary` satırı `[--]`, ext4 satırı `[NOT MOUNTED]` olur. Bu durum beklenmektedir ve filesystem mount başarısı olarak raporlanmamıştır.

İlk QEMU HMP testinde tuşlar gecikmesiz gönderildiğinde ekran değişmedi. İkinci test her tuş arasında 150–200 ms bekleyerek `VER` ve `PROC` komutlarını görünür biçimde çalıştırdı. Bu nedenle input testi gecikmeli yöntemle geçmiştir.

## Dosyalar

- [`screenshots/verification-findings-v02.md`](../screenshots/verification-findings-v02.md): kronolojik görsel ve sınırlama notları.
- [`build/qemu-serial-proc.log`](../build/qemu-serial-proc.log): son boot seri çıktısı.
- [`tests/elf-test.sh`](../tests/elf-test.sh): ELF format testi.
- [`tests/boot-test.sh`](../tests/boot-test.sh): QEMU boot testi.

Bu kayıt, hiçbir kullanıcı modu ELF çalıştırma veya ext4 mount başarısı iddia etmez; o davranışlar yol haritasında sonraki aşamalardır.
