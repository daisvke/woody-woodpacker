# **woody-woodpacker**

## **Table of Contents**

* [Overview](#overview)
* [Execution Flow](#execution-flow)
* [Runtime Behavior](#runtime-behavior)
* [ELF 64-bit File Structure](#elf-64-bit-file-structure)
* [Injection Mechanism](#injection-mechanism)
* [Stub Patch System](#stub-patch-system)
* [Entry Point Redirection](#entry-point-redirection)
* [Encryption System](#encryption-system)
* [Commands](#commands)
* [Automated Test Suite](#automated-test-suite)
* [Debug Tools](#debug-tools)
* [Compatibility Notes](#compatibility-notes)

---

# **Overview**

This project implements a **64-bit ELF packer with runtime unpacking capabilities**.

It transforms an ELF binary into a new executable (`woody`) that embeds:

* an encrypted copy of the original executable segment
* a custom unpacking stub injected into the binary

At runtime, the stub reconstructs the original program in memory and transfers execution back to it, preserving identical behavior while modifying only the binary representation.

---

# **Execution Flow**

```text
Original ELF
→ Parse ELF headers & program headers
→ Locate executable PT_LOAD segment
→ Extract segment (includes .text region)
→ Encrypt payload (XOR + additive cipher)
→ Inject stub into executable segment region
   (padding or shifting mode)
→ Patch stub with metadata (offsets, sizes, key)
→ Modify ELF entry point (e_entry → stub)
→ Generate "woody"

Runtime:
→ Kernel loads ELF
→ Execution starts in stub
→ Stub decrypts payload in memory
→ Restores original code
→ Prints: ....WOODY.....
→ Jumps to original entry point
→ Original program executes
```

---

# **Runtime Behavior**

At execution time, a packed `/bin/whoami` behaves as follows:

```text
....WOODY.....
username
```

Except for the printed string, the final behavior remains strictly identical to the original binary.

---

# **ELF 64-bit File Structure**

```text
+----------------------------------------------------+
|                    ELF Header                      |
|            (64 bytes, ELF64_Ehdr structure)        |
+----------------------------------------------------+
|                Program Header Table                |
|   (Variable size, ELF64_Phdr entries)              |
|   describes memory-mapped segments                 |
+----------------------------------------------------+
|                   Text Section                     |
|   (Executable code inside PT_LOAD segment)         |
+----------------------------------------------------+
|                   Data Section                     |
|   (Initialized data inside PT_LOAD segment)        |
+----------------------------------------------------+
|                 Symbol Table Section               |
|   (ELF64_Sym entries)                              |
+----------------------------------------------------+
|                 String Table Section               |
|   ("name1\0name2\0...")                            |
+----------------------------------------------------+
|                Section Header Table (optional)     |
|   (ELF64_Shdr entries)                             |
+----------------------------------------------------+
```

---

# **Injection Mechanism**

The stub is injected into the **executable PT_LOAD segment**, which typically contains:

* `.text`
* executable data pages
* padding between segments

It is not restricted to `.text` only, but placed inside the **executable memory-mapped region**.

---

## **1. Padding Injection (preferred mode)**

Used when sufficient space exists inside the executable segment.

Condition:

```
sizeof_stub ≤ padding_size
```

Where:

```
padding_size = next_segment.p_offset - injection_offset
```

Behavior:

* stub is written directly into existing padding
* no ELF relocation required
* no structural modification of headers

```text
[ PT_LOAD (code + .text) | padding | STUB ]
```

---

## **2. Shifting Injection (fallback mode)**

Used when padding is insufficient or explicitly required.

### Step 1 — Extend segment size

```c
p_filesz += sizeof_stub;
p_memsz  += sizeof_stub;
```

---

### Step 2 — Relocate ELF structures

All headers after injection are updated:

* program headers offsets shifted
* section headers offsets shifted
* ELF section header table offset updated

---

### Step 3 — Rebuild binary layout

```text
[ original ELF ]
[ STUB inserted ]
[ shifted ELF data ]
```

---

# **Stub Patch System**

The injected stub is patched with runtime metadata:

```c
ww_t_patch patch;
```

This includes:

* offset from stub to original entry point
* `.text` section offset relative to stub
* `.text` size
* segment relocation offsets

Layout inside binary:

```text
[ STUB ........ PATCH ........ KEY ]
```

---

# **Entry Point Redirection**

The ELF entry point is modified:

```c
e_entry = injection_addr;
```

Result:

```text
Before: entry → main()
After:  entry → stub → main()
```

---

# **Encryption System**

## XOR Cipher

* symmetric encryption
* same function used for encryption and decryption

## Additive Layer

Applied before XOR:

```
byte → +offset → XOR → encrypted
encrypted → XOR → -offset → original
```

---

# **Commands**

## Basic usage

```bash
make # Build project
./woody_woodpacker <binary> [OPTIONS]

# Make and run the packer with the default options, then run the packed binary, all with valgrind
make run

# Example: run the packer with verbose mode, padding injection mode, then run the binary
./woody_woodpacker /bin/ls -i p -s v  && ./woody

```

---

## Options

### Verbose mode

Displays detailed information about:

* ELF parsing
* injection offsets
* padding analysis
* relocation operations
* stub patch values

```bash
-v
--verbose
```

---

### Injection mode: padding

Attempts to inject stub into existing executable segment padding.

```bash
-i p
--injection-type padding
```

* safest mode
* no ELF restructuring
* requires enough free space

---

### Injection mode: shift

Forces structural modification of ELF when padding is insufficient or explicitly selected.

```bash
-i s
--injection-type shift
```

* rewrites ELF layout
* shifts headers and segments
* guarantees injection success

---

Here’s a clean README section you can drop in, matching your project tone and that test script:

---

## Automated Test Suite

The project includes a simple validation script to test packing behavior across multiple ELF binaries.

It builds the project, runs the packer on a set of sample binaries, and verifies execution output consistency.

### What the test does

For each binary in the `resources/` directory, the script:

* Builds the project using `make`
* Executes the packer in both modes:

  * `-i s` (shift injection mode)
  * `-i p` (padding injection mode)
* Runs the resulting packed binary (`./woody`)
* Checks execution output for the success signature:

  ```
  ....WOODY....
  ```
* Reports results per binary and mode:

  * 🟢 PASS → correct execution detected
  * 🔴 FAIL → missing or invalid output

---

### How to run tests

```bash
chmod +x test.sh
./test.sh
```

---

### Expected structure

```
project/
├── resources/
│   ├── bin1
│   ├── bin2
│   └── ...
├── woody_woodpacker
├── woody
└── test.sh
```

---

# **Debug tools**

```bash
readelf -l [filename]   # Check program headers of the file
readelf -S [filename]   # Check section headers

hexdump -C [filename]   # Check the file in hex format

vimdiff [filename 1] [filename 2]  # Check the difference between two files

# Extract the .text section from code.o
objcopy --dump-section .text=code-raw code.o

# Print the loaded file content in hex format at address 0x401040
gdb ./woody
run (or r)
x/16xw 0x401040

# Add breakpoint at relative address 11ad if base address is 0x4011ad
b *0x4011ad

# Show ELF entrypoint
gdb ./woody
starti
info files
info proc map

# Produce a trace trap that stops the execution at the position (useful when debugging)
int3
```

# Compatibility Notes

While the tool works on many standard Linux x86-64 ELF binaries, some binaries may fail to pack or may become unstable after packing.

## Why some binaries fail

Not all ELF binaries have the same layout characteristics. The shifting injection mode depends on being able to safely:

* Extend a `PT_LOAD` segment (`p_filesz` / `p_memsz`)
* Shift all subsequent program and section headers correctly
* Preserve valid page-aligned memory mappings between segments
* Maintain consistent execution flow after entry point redirection

Some binaries cannot satisfy these conditions safely due to:

* **Tight segment packing**

  * No sufficient contiguous space between `PT_LOAD` segments
  * Stub overflow crosses into adjacent mapped regions

* **Complex or hardened ELF layouts**

  * Multiple overlapping or tightly coupled `PT_LOAD` segments
  * Special-purpose loader assumptions (e.g. glibc, ld-linux optimizations)

* **RELRO / TLS / dynamic loader constraints**

  * `GNU_RELRO` and `DYNAMIC` segments may break if shifted incorrectly
  * Runtime loader expects strict offsets for relocation and linking

* **PIE + ASLR edge cases**

  * Some binaries rely on exact relative layout between segments
  * Small changes in layout can invalidate runtime assumptions

# **Bonus ideas**
- 32-bit support
- Custom key parameter
- PE / Mach-O support
- Compression
