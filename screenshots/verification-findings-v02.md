# QEMU verification findings — 2026-09-20

The `screenshot-v02.png` capture shows NP-DOS 0.1 reaching the 64-bit long-mode kernel, rendering the VGA TUI, reporting `32457 free pages`, and stopping at the `A:\>` prompt. ATA PIO and ext4 are intentionally reported as unavailable in the empty QEMU CD-ROM-only test machine; the source paths and negative status are real, not placeholders.

The later `screenshot-shell.png` capture is identical to the boot capture. QEMU HMP `sendkey` commands were accepted by the monitor but did not produce visible PS/2 input in `-display none`; therefore the interactive command path is not marked as passed. The automated test verdict remains limited to boot, serial diagnostics, and host-side ELF validation.

A second QEMU curses run sent `v`, `e`, `r`, and Enter with 200 ms gaps through the HMP monitor. The `screenshot-curses-v02.png` capture visibly shows `A:\> ver`, the NP-DOS version/ABI line, and the next `A:\>` prompt. This is a passed keyboard/TUI command test. The first no-delay run remains a useful negative observation about timing, not a product failure.

The release capture `screenshot-release.png` shows the corrected `INT 10h output: . | INT 16h software-vector smoke test [OK]` line, 32,457 free pages, the DOS-style command prompt, and all driver status lines. `make test` passed the kernel build, GRUB Multiboot2 validation, both C/ASM ELF64 image checks, and the QEMU serial boot test immediately before this capture.

The final curses/HMP test entered `proc` with inter-key delays. `screenshot-proc.png` shows `Process registry: 1 entry (context switching is not enabled)`, `PID 0 READY kernel`, and the returned prompt. This confirms the new process registry path without claiming scheduler or user-mode execution.

After the last keyboard table correction, the clean `make test` run passed again. The canonical final capture is now `build/screenshot-final.png`; it shows the QEMU boot status, native INT smoke test, `proc`, one kernel process in READY state, and a fresh `A:\>` prompt.
