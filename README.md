# woody-woodpacker

## Description
This project is about coding a simple packer.
<br /><br />
“Packers” are tools whose task consists of compressing executable programs (.exe, .dll,.ocx ...) and encrypting them simultaneously. <br />
During execution, a program passing through a packer is loaded in memory, compressed and encrypted, then it will be decompressed (decrypted as well) and finally be executed.
<br /><br />
The existence of such programs is related to the fact that antivirus programs generally analyse programs when they are loaded in memory, before they are executed. <br />
Thus, encryption and compression of a packer allow to bypass this behavior by obfuscating the content of an executable until it execution.

## Allowed functions

◦ open, close, exit
◦ fpusts, fflush, lseek
◦ mmap, munmap, mprotect
◦ perror, strerror
◦ syscall
◦ the functions of the printf family
◦ the function authorized within libft (read, write, malloc, free, for exam-
ple)

## Useful Links
https://packetstormsecurity.com/files/12327/elf-pv.txt.html
https://grugq.github.io/docs/phrack-58-05.txt
https://wh0rd.org/books/linkers-and-loaders/linkers_and_loaders.pdf
