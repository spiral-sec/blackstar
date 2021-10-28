# LIBSTAR

## Rationale

This library has been built following the idea in this [whitepaper](http://papermint-designs.com/dmo-blog/2016-01-pocrypt-a-proof-of-concept-for-dynamically-decrypt-linux-binaries).
Essentially, it explains how you can implement obfuscation in ELF files, by
encrypting sections of a binary file with other sections of the same binary file.
This would need the program to rewrite itself, or at the very least to decrypt
itself.

The goal of this project was to create a library to easily do some research on
this field, and improve known obfuscation thechniques.

## Credits

- [PoCrypt](http://papermint-designs.com/dmo-blog/2016-01-pocrypt-a-proof-of-concept-for-dynamically-decrypt-linux-binaries)
- [WhiteComet](https://github.com/PocInnovation/WhiteComet)

## Example

Here, you can see an easy way to encrypt a program:

```c
void encrypt_program(char const *binary_path, char *key,
char *ksection, char *bsection, char *csection)
{
    blackstar_t *bstar = NULL;

   bstar = bl_read(binary_path);
    if (!bstar)
        return;
    bl_encrypt_section(bstar, csection, key_section, bsection, &xor_crypt, key);
    bl_destroy(bstar);
}
```

As you can see, you give a function pointer to the `bl_encrypt_section` call,
which allows you to have some kind of custom algorithm to encrypt your ELF
section.
