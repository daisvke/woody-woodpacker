# **woody-woodpacker**

## **Description**
This project is about coding a packer and an unpacker shellcode for ELF (Executable and Linkable Format) 64-bit binary files.

"Packers" are tools whose task consists of compressing executable programs (.exe, .dll, .ocx, etc.) and/or encrypting them.  
During the execution of a packer, a program passing through that packer is loaded in memory, compressed, and encrypted. Then, during execution of the packed program, the shellcode unpacker that the packer will have injected will decompress, decrypt, and finally execute the program.  
The existence of such programs is related to the fact that antivirus programs generally analyze programs when they are loaded into memory, before execution.  
Thus, encryption and compression of a packer allow it to bypass this behavior by obfuscating the content of an executable until its execution.


## **Commands**
```bash
# Make and run the packer with the default options, then run the packed binary, all with valgrind
make run

# Run the packer with verbose mode, padding injection, and virus mode on, then run the binary
./woody_woodpacker /bin/ls -v -i=p -s=v  && ./woody

# Useful commands

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

# Produce a trace trap that stops the execution at the position (useful when debugging)
int3
```


## **Technical Aspects**

### **Packers and Unpackers**
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
Two distinct elements are involved: the packer and the unpacker.

The **packer** is a separate program used to encrypt or compress the target binary. It takes the original binary as input, applies encryption or compression techniques, and generates an encrypted or compressed binary as output. The packer is responsible for creating a specific format or structure for the encrypted/compressed binary, which may include additional information such as decryption routines or metadata.

The **unpacker** is included inside the generated file during the build process. It is a special code or routine that is loaded and executed when the generated file is started. Its role is to decrypt and/or decompress the packed binary to restore it to its original form before execution.

### **The 64-bit ELF File Structure**

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
|                  ...Segment n                      |
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

- **ELF Header:** Contains essential information about the file, including the ELF identification, file type, architecture, entry point, program header table offset, section header table offset, and more.

- **Program Header Table:** Describes the segments or sections in the binary file (e.g., the code segment, data segment, dynamic linking information). Each entry follows the ELF64_Phdr structure and provides details such as segment type, offset, virtual address, file size, and memory size. The size of the program header table depends on the number of entries.  
You can view program header details with:  
```bash
readelf -l [filename]
```

- **Section Header Table:** Contains information about each section in the binary, including name, type, flags, offset, size, etc. Each entry follows the ELF64_Shdr structure. The section header table size depends on the number of sections.

- **Data Section:** Holds initialized data used by the program. Its size varies depending on the data in the binary.

- **Text Section:** Contains executable code of the program. Its size varies based on the code written.

- **Symbol Table Section:** Stores information about symbols defined or referenced by the program (e.g., function and variable names, addresses, and attributes). Each entry follows the ELF64_Sym structure. The symbol table size varies depending on the number of symbols.

- **String Table Section:** Stores symbol names, section names, and other string data referenced by the binary. It contains substrings grouped into a single string.

Please note that the actual sizes and structures can vary depending on the specific binary format and the file contents. The above representation is a general overview of components typically found in a 64-bit ELF file.

Since we are working with an **EIP-independent** program (it doesn’t rely on specific memory addresses or offsets relative to the EIP register), it is more reliable across different environments.

Please check the `man` page for ELF for more details.

### **ELF Infection**

In the context of packers and unpackers, the **stub** and the **unpacker** are closely related but not synonymous.

- **Stub:** A small piece of code inserted into the packed binary. Its purpose is to perform the initial processing and setup necessary for the unpacker to execute. The stub typically contains minimal functionality and is responsible for locating and executing the unpacker code.

- **Unpacker:** The actual code responsible for unpacking or decrypting the packed portions of the binary. It extracts the original, unpacked code and data, restoring the binary to its original state.

#### **Inserting Stub or Additional Code into ELF File:**
Here are a few common approaches for inserting a stub or additional code into an ELF file:

1. **At the End of the File/Segment/Section:** This method extends the file size, writing new code at the end and shifting existing elements.

2. **In Padding Areas:** ELF files often contain padding areas between sections or segments, which are typically filled with zeroes. These unused spaces can be utilized for the stub code. However, padding may not always be available or large enough, depending on the specific ELF structure.

3. **Reserved Sections:** If the ELF file has reserved or unused sections, these can be repurposed to hold the stub code. Care must be taken to ensure that the section does not interfere with the ELF file’s original functionality.

Our program uses **padding injection** by default. If the padding area is too small for the stub, we use the first approach (inserting the stub after the executable segment).

### **Data Encryption**

Our keygen function generates a random encryption key of a specified width using a given character set. It seeds the random number generator with the current time and selects random characters from the character set to build the key.

We have enhanced our XOR-based encryption algorithm by incorporating an **additive cipher**. The additive cipher (or Caesar cipher) shifts each character of the plaintext by a fixed amount (the key) before applying the XOR operation. This additional step increases the complexity of the encryption process.

### **Payload**

By default, the payload is a string displayed before the execution of the target program. However, we’ve added a **‘virus’ mode**, which is a quine binary executed by the shellcode. A quine is a piece of code that replicates itself in the current directory.


## **Notes**
```asm
lodsb             ; Load the next byte of the message into AL
xor al, key       ; XOR the byte with the key
stosb             ; Store the encrypted byte back into memory
loop encrypt_loop ; Repeat for the entire message

lodsb:
    lodsb stands for "load byte from source into AL."
    It loads a byte from the memory location pointed to by the ESI register into AL.
    After loading, the ESI register increments or decrements based on the direction flag (DF).

stosb
