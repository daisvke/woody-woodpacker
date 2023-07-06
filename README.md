# woody-woodpacker

## Description
This project is about coding packers for ELF64 binary files.
<br /><br />
"Packers" are tools whose task consists of compressing executable programs (.exe, .dll,.ocx ...) and encrypting them simultaneously. <br />
During execution, a program passing through a packer is loaded in memory, compressed and encrypted, then it will be decompressed (decrypted as well) and finally be executed.
<br /><br />
The existence of such programs is related to the fact that antivirus programs generally analyse programs when they are loaded in memory, before they are executed. <br />
Thus, encryption and compression of a packer allow to bypass this behavior by obfuscating the content of an executable until it execution.

## Technical aspects
### Two packers
```
Original Packer                                Final Packer
+-------------------------+                   +-------------------------+
|                         |                   |                         |
|    Input Binary         |                   |   Encrypted/Compressed  |
|                         |                   |       Binary            |
+----------+--------------+                   +-----------+-------------+
           |                                              |
           |             Encryption/Compression           |
           |                                              |
           v                                              v
+----------+--------------+                   +-----------+-------------+
|                         |                   |                         |
|    Encrypted/Compressed |                   |  Decrypted/Decompressed |
|        Binary           |                   |         Binary          |
|                         |                   |                         |
+-------------------------+                   +-------------------------+
```
Two distinct packers are involved: the original packer and the final packer.

The original packer is a separate program that is used to encrypt or compress the target binary. It takes the original binary as input, applies specific encryption or compression techniques, and generates an encrypted or compressed binary as output. The original packer is responsible for creating a specific format or structure for the encrypted/compressed binary, which may include additional information such as decryption routines or metadata.

Next, the final packer is the packer that is included inside the generated file during the build process. It is a special code or routine that is loaded and executed when the generated file is started. Its role is to decrypt the encrypted binary or decompress the compressed binary to restore it to its original form before execution.

So, in summary, the original packer is used to encrypt or compress the target binary, while the final packer is included in the generated file and is loaded during execution to decrypt or decompress the binary before execution.

### The executable 64-bit file structure

Below is a textual representation of a binary file with its ELF header (64-bit) and other regions, along with their byte sizes and corresponding structures:

```
Binary File
+----------------------------------------------------+
|                    ELF Header                      |
|            (64 bytes, ELF64_Ehdr structure)        |
+----------------------------------------------------+
|                Program Header Table                |
|   (Variable size, contains ELF64_Phdr structures)  |
|                                                    |
|                     Segment 1                      |
|                     Segment 2...                   |
|                  ...Segemnt n                      |
+----------------------------------------------------+
|                   Text Section                     |
|               (Variable size and data)             |
+----------------------------------------------------+
|                   Data Section                     |
|               (Variable size and data)             |
+----------------------------------------------------+
|                 Symbol Table Section               |
|   (Variable size, contains ELF64_Sym structures)   |
+----------------------------------------------------+
|              String Table Section                  |
|          (Variable size, contains strings)         |
+----------------------------------------------------+
|                Section Header Table (optional)     |
|   (Variable size, contains ELF64_Shdr structures)  |
+----------------------------------------------------+
```

Here's a brief explanation of each component:

- ELF Header: The ELF header contains essential information about the file, including the ELF identification, file type, architecture, entry point, program header table offset, section header table offset, and more. It follows the ELF64_Ehdr structure and has a size of 64 bytes.

- Program Header Table: This table describes the segments or sections in the binary file, such as the code segment, data segment, and dynamic linking information. Each entry follows the ELF64_Phdr structure and provides details like the segment type, offset, virtual address, file size, and memory size. The size of the program header table can vary depending on the number of entries.

- Section Header Table: This table contains information about each section in the binary file, such as the name, type, flags, offset, size, and more. Each entry follows the ELF64_Shdr structure. The section header table size can vary based on the number of sections.

- Data Section: This section holds initialized data used by the program. It can have a variable size depending on the data present in the binary.

- Text Section: This section contains the executable code of the program. Its size varies based on the code written.

- Symbol Table Section: This section stores information about symbols defined or referenced by the program, such as function and variable names, their addresses, and other attributes. Each entry in the symbol table follows the ELF64_Sym structure. The symbol table size can vary depending on the number of symbols.

- String Table Section: This section stores the names of various symbols, section names, and other string data referenced by the binary. It has a variable size and contains strings.

Please note that the actual sizes and structures may vary depending on the specific binary format and the contents of the file. The representation provided here gives a general overview of the components typically found in a binary file with a 64-bit ELF header.

### ELF infection

A process image consists of a 'text segment' and a 'data segment'.  The text
segment is given the memory protection r-x (from this its obvious that self
modifying code cannot be used in the text segment).  The data segment is
given the protection rw-.

### Keygen

Our keygen function generates a random encryption key of a specified width using a given character set. It allocates memory for the key, seeds the random number generator with the current time, and selects random characters from the character set to build the key.

### Data encryption
* XOR-based Encryption

## Allowed functions

* open, close, exit
* fpusts, fflush, lseek
* mmap, munmap, mprotect
* perror, strerror
* syscall
* the functions of the printf family
* the function authorized within libft (read, write, malloc, free, for example)

## Useful Links
https://packetstormsecurity.com/files/12327/elf-pv.txt.html<br />
https://grugq.github.io/docs/phrack-58-05.txt<br />
https://wh0rd.org/books/linkers-and-loaders/linkers_and_loaders.pdf
