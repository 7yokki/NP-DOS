# NP-DOS Stage 0.1 ABI

Bu belge, NP-DOS’un long mode içindeki DOS-benzeri yazılım kesme sözleşmesini tanımlar. Tüm servisler CPL0’dan çağrılmak üzere tasarlanmıştır. Register’ların korunması kesme stubları tarafından yapılır. Durum değeri veren servisler RAX kullanır.

## INT 10h — Video / ekran

`AH=0Eh` karakterin VGA metin ekranına yazılmasını ister. Karakter `AL` içindedir. Satır sonu, carriage return ve backspace NP-DOS VGA sürücüsünde işlenir. Stage 0.1 grafik modları veya framebuffer modu sağlamaz; VGA 80×25 text mode kullanır.

Örnek ASM:

```asm
mov ah, 0x0E
mov al, 'A'
int 0x10
```

## INT 13h — Disk

`AH=02` primary ATA kanalından sektör okur. `AH=03` sektör yazar. Stage 0.1 C servis köprüsünde:

- `EDX`: 28-bit LBA’nın düşük kısmı.
- `AL`: sektör sayısı; ATA katmanı tek sektör komutlarıyla döngü kurar.
- `RDI`: buffer adresi.
- `RAX`: servis dönüşü; `0` başarı, negatif sürücü hata kodu olarak genişletilmiş değer.

Bu ABI henüz BIOS Disk Address Packet sözleşmesi değildir. Gerçek cihaz bulunamazsa `ATA_STATUS` yoklaması başarısız olur ve ext4 mount edilmez.

## INT 14h — Seri port

`AH=01`, `AL` karakterini COM1’e gönderir. Seri port 38400 baud, 8 data bit, no parity, one stop bit olarak açılır.

## INT 16h — Klavye

Non-blocking polling servisidir. PS/2 durum kuyruğunda bir karakter varsa ASCII karakteri RAX’ın düşük byte’ında döner. Karakter yoksa RAX sıfırdır. Klavye, Türkçe Q fiziksel tarama kodu tablosunu kullanır. Unicode Türkçe harfleri için stage 0.1 VGA ROM kod sayfası nedeniyle ASCII fallback tercih edilir.

## INT 17h — Paralel port

`AH=00`, `AL` karakterini LPT1’e göndermeyi dener. Status portundaki ready biti beklenir, veri portuna yazılır ve strobe darbesi üretilir.

## Shell komutları

| Komut | İşlem |
| --- | --- |
| `HELP` | Komut listesini gösterir. |
| `CLS` | VGA ekranını temizler. |
| `VER` | Sürüm ve ABI bilgisini gösterir. |
| `MEM` | Serbest sayfa ve en yüksek takip edilen sayfayı gösterir. |
| `DRV` | INT ve sürücü matrisini gösterir. |
| `ELF` | NP_ELF_LOADER sınırlarını gösterir. |
| `PROC` | Süreç kayıt tablosunu gösterir. |
| `REBOOT` | 8042 reboot komutu gönderir. |

## Uygulama ELF sözleşmesi

ASM ve C örnekleri `_start` sembolüyle ELF64 ET_EXEC olarak linklenir. Uygulama örnekleri `AH=0Eh` ile INT 10h kullanır. Host testleri ELF class, machine ve PT_LOAD program başlığı bulunmasını kontrol eder. NP-DOS stage 0.1’de bu ELF’ler henüz ring 3’e aktarılmaz.

## Referanslar

[1]: https://refspecs.linuxfoundation.org/elf/gabi4+/contents.html "System V ABI ELF Specification"
[2]: https://wiki.osdev.org/Interrupt_Descriptor_Table "OSDev Interrupt Descriptor Table"
[3]: https://wiki.osdev.org/ATA_PIO_Mode "OSDev ATA PIO Mode"
