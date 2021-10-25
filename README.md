# blackstar

blackstar is a Polymorphic Keylogger written in C, for research purposes only.
It is based on the [whitecomet project](https://github.com/PoCInnovation/Whitecomet-Research).

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

### WhiteComet's AV-evasion limitations

Although it is a really cool project, there are still limitiations for the
current approach Whitecomet uses. The first one is that when inspecting the
binary, the sections are one of the first things a researcher looks at. Second is
 that it requires a setup that is not very easy to modify. You must get the exact
 size of your section, so that you decrypt a fixed length. Third is that you still
leave a lot of information present in the binary, that make it easier for somebody
to get clues about what your program does.
We hope to tackle at least some of these problems.

## How do I build the project ?

```bash
make

# run with admin privileges
./blackstar -i 192.168.1.69 -p 6969

# clean the repo like so
make fclean
```

## Can I add my own payload instead of using the one that is present ?

Yes, you can ! But you will need to change the `ELF_FUNC_SIZE` according to your
section's size. To check your section size, compile the program without the UPX
packer, and call `size -d -A blackstar` to get the size of all section. You will
need to find the section named as `ELF_CODE` (by default `.banshee`).

To add some code in the section, create a normal C function and add the
`__attribute__((section(ELF_CODE)))` in front of the declaration. You can find
an example in `payload.c`.

## Current goals

- [ ] Create a usable and working spyware, reading and transmitting important files,
    logging keystrokes, and setting up a revshell
- [ ] Improve the obfuscation method to use the less amount of section possible
- [ ] Find a way to easely find the address of a value in the binary so that we
can just hardcode it and not depend on sections
- [ ] add python webserver to get a nice web interface with all the retrieved data
