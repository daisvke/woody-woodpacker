# woody-woodpacker

## Description
This project is about coding a packer for ELF (Executable and Linkable Format) 64-bit binary files.
<br /><br />
"Packers" are tools whose task consists of compressing executable programs (.exe, .dll,.ocx ...) and encrypting them simultaneously. <br />
During the execution of a packer, a program passing through that packer is loaded in memory, compressed and encrypted. Then, during execution of the packed program, it will be decompressed, decrypted, and finally be executed.
<br /><br />
The existence of such programs is related to the fact that antivirus programs generally analyse programs when they are loaded in memory, before they are executed. <br />
Thus, encryption and compression of a packer allow to bypass this behavior by obfuscating the content of an executable until its execution.

## Technical aspects
### Packers and unpackers
```
Packer                                        Unpacker
+-------------------------+                   +-------------------------+
|                         |                   |                         |
|    Input Binary         |                   |   Encrypted/Compressed  |
|                         |                   |       Binary            |
+----------+--------------+                   +-----------+-------------+
           |                                              |
           |                                              |
           |                                              |
           v                                              v
+----------+--------------+                   +-----------+-------------+
|                         |                   |                         |
|    Encrypted/Compressed |                   |  Decrypted/Decompressed |
|        Binary           |                   |         Binary          |
|                         |                   |                         |
+-------------------------+                   +-------------------------+
```
Two distinct packers are involved: the packer and the unpacker.

The packer is a separate program that is used to encrypt or compress the target binary. It takes the original binary as input, applies specific encryption or compression techniques, and generates an encrypted or compressed binary as output. The packer is responsible for creating a specific format or structure for the encrypted/compressed binary, which may include additional information such as decryption routines or metadata.

Next, the unpacker is included inside the generated file during the build process. It is a special code or routine that is loaded and executed when the generated file is started. Its role is to decrypt the encrypted binary and/or decompress the compressed binary to restore it to its original form before execution.

### The 64-bit ELF file structure

Below is a textual representation of a binary file with its ELF header (64-bit) and other regions, along with their byte sizes and corresponding structures:

```
+----------------------------------------------------+
|                    ELF Header                      |
|            (64 bytes, ELF64_Ehdr structure)        |
+----------------------------------------------------+
|                Program Header Table                |
|   (Variable size, contains ELF64_Phdr structures)  |
|                 contains info about:               |
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
|                 String Table Section               |
|          (Variable size, contains strings)         |      
|       Contains a single string in the format:      |
|              "name1\0name2\0name3\0"               |
+----------------------------------------------------+
|                Section Header Table (optional)     |
|   (Variable size, contains ELF64_Shdr structures)  |
+----------------------------------------------------+
```

Here's a brief explanation of each component:

- ELF Header: The ELF header contains essential information about the file, including the ELF identification, file type, architecture, entry point, program header table offset, section header table offset, and more.

- Program Header Table: This table describes the segments or sections in the binary file, such as the code segment, data segment, and dynamic linking information. Each entry follows the ELF64_Phdr structure and provides details like the segment type, offset, virtual address, file size, and memory size. The size of the program header table can vary depending on the number of entries.<br />
Details about program headers and segments can be displayed by the command: readelf -l.

- Section Header Table: This table contains information about each section in the binary file, such as the name, type, flags, offset, size, and more. Each entry follows the ELF64_Shdr structure. The section header table size can vary based on the number of sections.

- Data Section: This section holds initialized data used by the program. It can have a variable size depending on the data present in the binary.

- Text Section: This section contains the executable code of the program. Its size varies based on the code written.

- Symbol Table Section: This section stores information about symbols defined or referenced by the program, such as function and variable names, their addresses, and other attributes. Each entry in the symbol table follows the ELF64_Sym structure. The symbol table size can vary depending on the number of symbols.

- String Table Section: This section stores the names of various symbols, section names, and other string data referenced by the binary. It has a variable size and contains substrings grouped in a single string.

Please note that the actual sizes and structures may vary depending on the specific binary format and the contents of the file. The representation provided here gives a general overview of the components typically found in a binary file with a 64-bit ELF header.<br />
Thus, an EIP-independent program would be preferable for our case.<br />
When software or code is EIP-independent, it means that it does not rely on specific memory addresses or offsets relative to the EIP register, making it more reliable across different environments.
<br /><br />
Please check man ELF for more details.

### The section header

In the context of object file formats, such as ELF, the section header is optional in certain cases. The section header table provides information about the layout and attributes of sections within the object file.
<br /><br />
Here are some cases where the section header may be optional:

1. Simple Object Files: In some cases, when dealing with simple object files that do not have multiple sections or complex attributes, the section header table may be omitted to save space. This is often seen in smaller or minimalistic object files.

2. Stripped Executables: When creating an executable file for distribution, it is common to strip the symbol and debug information from the binary. In this case, the section header table, along with other debugging-related sections, may be removed to reduce the file size and protect the original source code and symbols.

3. Raw Data Files: If the object file is intended to be used purely as a container for raw data, without any need for detailed section information or processing, the section header table may be omitted. This is typically seen in cases where the object file is used as a data resource or input file for other tools or processes.
<br /><br />
It's important to note that the absence of the section header table in these cases may limit or hinder the ability to analyze or manipulate the object file in certain ways. The section header provides valuable metadata and allows tools and systems to understand the structure and characteristics of the object file.

### ELF infection

* A process image consists of a 'text segment' and a 'data segment'. The text
segment is given the memory protection r-x (from this its obvious that self
modifying code cannot be used in the text segment).  The data segment is
given the protection rw-.
* Program header: e_entry: This member gives the virtual address to which the system first transfers control, thus starting the process.
* Encrypting all sections of an ELF file can have some implications:

    1. Compatibility: Encrypting all sections may affect the functionality of the ELF file, especially if there are sections that require specific processing or are needed for proper execution.

    2. Performance: Decrypting sections at runtime can introduce overhead and affect the performance of the program.

    3. Debugging and Analysis: Encrypting all sections can make it more challenging to analyze and debug the program, as it obscures the contents of the sections. This can make it difficult to troubleshoot issues or understand the program's behavior.

    4. Dependencies: Some sections, such as the ELF header and program headers, are critical for proper execution and loading of the ELF file.
* Encryption is typically applied to specific sections or segments in an ELF file to protect sensitive code or data. These sections can be selectively encrypted while leaving the section header and other critical parts of the ELF file intact. By doing so, the ELF file retains its structure and can still be processed by tools without issues. Thus, we will always leave out the ELF header and the program header from the encryption.

* In the ELF file format, the program header type (`p_type`) field specifies the type of each program header entry. Here are some common program header types and their corresponding decimal values:

1. `PT_NULL`: Null type (0)
2. `PT_LOAD`: Loadable segment (1) => ENCRYPT
3. `PT_DYNAMIC`: Dynamic linking information (2) => ENCRYPT
4. `PT_INTERP`: Program interpreter (3)
5. `PT_NOTE`: Auxiliary information (4)
6. `PT_SHLIB`: Reserved (5)
7. `PT_PHDR`: Program header table (6)
8. `PT_TLS`: Thread-local storage segment (7)
9. `PT_LOOS` to `PT_HIOS`: Operating system-specific values (inclusive range)
10. `PT_LOPROC` to `PT_HIPROC`: Processor-specific values (inclusive range)

These are just a few examples, and there may be other program header types depending on the specific needs and extensions of the ELF file format.

* In the context of packers and unpackers, the stub and the unpacker are closely related but not necessarily synonymous terms.
<br /><br />
The stub refers to a small piece of code that is inserted into the packed binary. Its purpose is to perform the initial processing and setup necessary for the unpacker to execute. The stub typically contains minimal functionality and is responsible for locating and executing the unpacker code.
<br />
The unpacker, on the other hand, is the actual code responsible for unpacking or decrypting the packed portions of the binary. It extracts the original, unpacked code and data, restoring the binary to its original state.

* When inserting the stub or the additional code into an ELF file, there are a few common approaches to consider:

    1. At the End of the File: One approach is to append the stub code at the end of the ELF file, after all existing sections and data. This can be done by extending the file size and writing the new code at the appended location. The stub can then be executed as part of the program's execution flow.

    2. In Padding Areas: ELF files often have padding areas between sections or segments. These padding areas are typically filled with zeroes. We can utilize these unused spaces to insert the stub code. However, these padding areas may not always be present or of sufficient size, depending on the specific ELF file structure.

    3. Reserved Sections: If the ELF file contains reserved or unused sections, we can repurpose one of these sections to hold the stub code. In that case we have to make sure that the selected section does not interfere with the original functionality of the ELF file or any dependencies.

### Keygen
Our keygen function generates a random encryption key of a specified width using a given character set. It seeds the random number generator with the current time, and selects random characters from the character set to build the key.

### Data encryption
* XOR-based Encryption
* In addition to XOR encryption, we've enhanced our encryption algorithm by incorporating an additive cipher. The additive cipher, also known as a Caesar cipher, shifts each character of the plaintext by a fixed amount (the key) before performing the XOR operation. This additional step adds another layer of complexity to the encryption process.

## Allowed functions
* open, close, exit
* fpusts, fflush, lseek
* mmap, munmap, mprotect
* perror, strerror
* syscall
* the functions of the printf family
* the function authorized within libft (read, write, malloc, free, for example)

## Todo
- Protect xor.s args

## Useful commands
```
readelf [filename]
hexdump -C [filename]
vimdiff [filename 1] [filename 2]

// Print the loaded file content in hex form at address 0x401040
gdb ./woody
run
x/16xw 0x401040

// Add breakpoint at relative address 11ad if base address is 0x4011ad
b *0x4011ad
```

## Useful Links
https://packetstormsecurity.com/files/12327/elf-pv.txt.html<br />
https://grugq.github.io/docs/phrack-58-05.txt<br />
https://wh0rd.org/books/linkers-and-loaders/linkers_and_loaders.pdf<br />
https://hackademics.fr/forum/hacking-connaissances-avanc%C3%A9es/reversing/autres-aq/3096-reversing-tutorials-level-2-le-format-elf
