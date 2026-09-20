#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
log="$root/build/qemu-boot.log"
rm -f "$log"
timeout 8s qemu-system-x86_64 \
  -cdrom "$root/build/np-dos.iso" \
  -m 128M \
  -display none \
  -serial "file:$log" \
  -no-reboot \
  -no-shutdown >/dev/null 2>&1 || true

grep -q 'kernel_main reached in long mode' "$log"
grep -q 'VGA OK' "$log"
grep -q 'ELF64 loader linked' "$log"
echo "PASS QEMU long-mode boot and serial diagnostics"
