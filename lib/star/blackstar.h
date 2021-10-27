
/**
 * \file blackstar.h
 * \brief libstar main header
 * \author winds0r
 * \date 27/10/2021
*/

#ifndef STAR_H
    #define STAR_H

#include <elf.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>

/**
 * \typedef struct star_s blackstar_t
 * \brief main object to handle ELF files
 *
 * \param path ELF executable filepath
 * \param content_len ELF's file content length
 * \param content ELF's complete content
 */
typedef struct star_s {
    char *path;
    size_t content_len;
    unsigned char *content;
} blackstar_t;

/**
 * \typedef void (*crypter_t)(unsigned char *ptr, size_t ptr_size, char *key, size_t key_size)
 * \brief function pointer encrypting ELF content with a key
 *
 * \param ptr pointer to edit
 * \param ptr_size pointer's length
 * \param key string which will encrypt ptr
 * \param key_size key's length
 */
typedef void (*crypter_t)(unsigned char *, size_t, char *, size_t);

// decrypt.c

/**
 * \fn char *gen_new_key(time_t seed)
 * \brief generates a random key with a seed
 *
 * \param seed data object used to generate random data for the key
 * \return random string
 */
char *gen_new_key(time_t);

/**
 * \fn int bl_decrypt_section(blackstar_t *bstar, char const *cname, char const *kname,
  char const *bname, crypter_t cr)
 * \brief wrapper function that decrypts a section with another, then the first
 one with the second, then syncs the binary. The boolean section is also set to false.
 *
 * \param bstar an object holding data for the ELF file you want to decrypt
 * \param cname obfuscated code's section name
 * \param kname key's section name
 * \param bname boolean's section name
 * \param cr function used to decrypt sections
 * \return zero
 */
int bl_decrypt_section(blackstar_t *, char const *, char const *, char const *,
crypter_t);

// crypt.c

/**
 * \fn void bl_naive_crypter(blackstar_t *bstar, const char *tname, const char *kname,
 crypter_t cr)
 * \brief calls cryting function without checking if boolean is set or not.
 *
 * \param bstar an object holding data for the ELF file you want to use
 * \param tname target's section name
 * \param kname key's section name
 * \param cr crypting function pointer
 */
void bl_naive_crypter(blackstar_t *, const char *, const char *, crypter_t);

/**
 * \fn char *bl_get_section_content(blackstar_t *bstar, const char *sname, size_t *size)
 * \brief reads a section and get its content
 *
 * \param bstar an object holding data for the ELF file you want to use
 * \param sname target's section name
 * \param size number of bytes that have been read
 * \return an array of characters with the section's content
 */
char *bl_get_section_content(blackstar_t *, const char *, size_t *);

/**
 * \fn void bl_edit_section(unsigned char *ptr, size_t ptr_size, char *replace_with)
 * \brief replaces a data in a pointer by another set of data
 *
 * \param ptr original pointer
 * \param ptr_size pointer's size
 * \param replace_with a string we want to set with ptr
 */
void bl_edit_section(unsigned char *, size_t, char *);

/**
 * \fn int bl_change_section_wperm(unsigned char *ptr, size_t ptr_size, bool should_write)
 * \brief change section's write permissions
 *
 * \param ptr original pointer
 * \param ptr_size pointer's size
 * \param should_write write_permissions status
 * \return 0 if successful
 */
int bl_change_section_wperm(unsigned char *, size_t, bool);

/**
 * \fn int bl_encrypt_section(blackstar_t *bstar, const char *cname, const char *kname,
  const char *bname, crypter_t cr, char *key)
 * \brief wrapper function that encrypts a section with another, then the first
 one with the second, then syncs the binary. The boolean section is also set to true.
 *
 * \param bstar an object holding data for the ELF file you want to decrypt
 * \param cname obfuscated code's section name
 * \param kname key's section name
 * \param bname boolean's section name
 * \param cr function used to decrypt sections
 * \param key encryption key
 * \return zero
 */
int bl_encrypt_section(blackstar_t *, const char *, const char *,
const char *, crypter_t, char *);

// crypters.c

/**
 * \fn void xor_crypt(unsigned char *ptr, size_t size, char *key, size_t ksize)
 * \brief calls a XOR on every character in ptr with a character from key
 *
 * \param ptr data to encrypt
 * \param size ptr's size
 * \param key data used to encrypt ptr
 * \param ksize key's size
 */
void xor_crypt(unsigned char *, size_t, char *, size_t);

// utils.c

/**
 * \fn blackstar_t *bl_read(char const *filepath)
 * \brief reads an ELF file into memory
 *
 * \param filepath ELF's path
 * \return a blackstar_t object
 */
blackstar_t *bl_read(char const *);

/**
 * \fn void bl_destroy(blackstar_t *bstar)
 * \brief frees a blackstar_t object from the heap
 *
 * \param bstar an object holding data for the ELF file you were holding
 */
void bl_destroy(blackstar_t *);

/**
 * \fn int bl_sync(blackstar_t *bstar)
 * \brief writes the ELF file from memory to the original file
 *
 * \param bstar an object holding the data you want to write
 * \return 0 if successful
 */
int bl_sync(blackstar_t *);

/**
 * \fn Elf64_Shdr *bl_find_section(void *data, char const *name)
 * \brief finds an object in the ELF header
 *
 * \param data a ptr holding the start of your binary, with the Prog. Header
 * \param name section's name we want to find
 * \return a section header object
 */
Elf64_Shdr *bl_find_section(void *, char const *);

#endif
