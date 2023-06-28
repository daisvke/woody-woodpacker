# woody-woodpacker

This project is about coding a simple packer.
<br /><br />
“Packers” are tools whose task consists of compressing executable programs (.exe, .dll,
.ocx ...) and encrypting them simultaneously. During execution, a program passing
through a packer is loaded in memory, compressed and encrypted, then it will be decom-
pressed (decrypted as well) and finally be executed.
<br /><br />
The existence of such programs is related to the fact that antivirus programs gener-
ally analyse programs when they are loaded in memory, before they are executed. Thus,
encryption and compression of a packer allow to bypass this behavior by obfuscating the
content of an executable until it execution.
