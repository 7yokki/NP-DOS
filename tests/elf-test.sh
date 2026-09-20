#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
for image in "$root/build/hello-asm.elf" "$root/build/hello-c.elf"; do
  readelf -h "$image" | grep -q 'Class:.*ELF64'
  readelf -h "$image" | grep -q 'Machine:.*Advanced Micro Devices X86-64'
  readelf -l "$image" | grep -q 'LOAD'
  echo "PASS ELF64 $(basename "$image")"
done
