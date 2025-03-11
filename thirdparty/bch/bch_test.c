#include "bch.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BCH_UNUSED_PARAMETER(parameter) do { (void)(parameter); } while (0)

int main(int args, char *argv[])
{
    BCH_UNUSED_PARAMETER(args);
    BCH_UNUSED_PARAMETER(argv);

    const unsigned int polynomial_size = 14;
    const unsigned int protected_bits = 58;

    struct bch_control *bc = init_bch(polynomial_size, protected_bits, 0);

    const unsigned int block_size = bc->n / 8;
    const unsigned int ecc_size = bc->ecc_bytes;
    const unsigned int data_size = block_size - ecc_size;

    unsigned char *data = calloc(block_size, sizeof(unsigned char));
    unsigned char *data_ecc = data + data_size;
    memset(data_ecc, 0, ecc_size);
    unsigned int  *errloc = calloc(block_size, sizeof(unsigned int));


    for (unsigned int i = 0; i < data_size; i++)
    {
        data[i] = (unsigned char)i;
    }

    encode_bch(bc, data, data_size, data_ecc);

    data[0] ^= 0xFF; // insert error
    data[1] ^= 0xFF; // insert error
    data[2] ^= 0xFF; // insert error
    data[3] ^= 0xFF; // insert error
    data[4] ^= 0xFF; // insert error
    data[5] ^= 0xFF; // insert error
    data[6] ^= 0xFF; // insert error
    data[7] ^= 0x03; // insert error
    int retval = decode_bch(bc, data, data_size, data_ecc, NULL, NULL, errloc);
    switch (retval)
    {
    case -EINVAL:
        fprintf(stderr, "Error\n");
        break;  
    case -EBADMSG:
        fprintf(stderr, "Too many errors detected\n");
        break;
    default:
        {
            if (retval > 0)
            {
                for (int n = 0; n < retval; n++)
                {
                    if (errloc[n] < data_size*8)
                    {
                        data[errloc[n] / 8] ^= 1 << (errloc[n] % 8);
                    }
                }

                for (unsigned int i = 0; i < data_size; i++)
                {
                    if (data[i] != (unsigned char)i)
                    {
                        fprintf(stderr, "Decoded data is not valid\n");
                    }
                }
            }
        }
    }
    free(data);
    free(errloc);
    return 0;
}
