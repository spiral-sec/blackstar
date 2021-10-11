# blackstar

blackstar is a [[Polymorphic]] [[Keylogger]] written in C, for research purposes
 only. It is based on the
[whitecomet project](https://github.com/PoCInnovation/Whitecomet-Research).

## Introduction

The goal of the project was to build some knowledge regarding obfuscation,
 as well as Advanced Unix Programming. To achieve that, I looked at how the
 Whitecomet project managed to work around anti-viruses, and I implemented it in
 a [keylogger](github.com/bogdzn/keylogger) I had written before.

### how does whitecomet work ?

To hide malicious code, Whitecomet exploits sections of an ELF binary. It has one
 section for the malicious code, one for the decryption key, and one for a boolean,
 set if the code is encrypted.  Once started for the first time, the program will
 read itself and decrypt the code, then execute it. In WhiteComet's case, the
 malicious code was a [reverse-shell](https://fr.wikipedia.org/wiki/Reverse_shell).

### What are our objectives, then ?

First, we want to understand how to create and manipulate section to change our
binary. Then, we will find a better way to hide our code, because the current
method has some limitations.

## WhiteComet's AV-evasion limitations

Although it is a really cool project, there are still limitiations for the
current approach Whitecomet uses. The first one is that when inspecting the
binary, the sections are one of the first things a researcher looks at. Second is
 that it requires a setup that is not very easy to modify. You must get the exact
 size of your section, so that you decrypt a fixed length. Third is that you still
leave a lot of information present in the binary, that make it easier for somebody
to get clues about what your program does.
We hope to tackle at least some of these problems.
