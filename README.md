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
