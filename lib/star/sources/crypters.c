
#include "blackstar.h"

void xor_crypt(unsigned char *data, size_t data_len, char *key, size_t key_len)
{
    for (size_t i = 0; i < data_len; i++)
        data[i] ^= key[i % key_len];
}


