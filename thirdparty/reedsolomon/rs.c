/***********************************************************************
* Copyright Henry Minsky (hqm@alum.mit.edu) 1991-2009
*
* This software library is licensed under terms of the GNU GENERAL
* PUBLIC LICENSE
*
*
* RSCODE is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* RSCODE is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Rscode.  If not, see <http://www.gnu.org/licenses/>.
*
* Commercial licensing is available under a separate license, please
* contact author for details.
*
* Source code is available at http://rscode.sourceforge.net
* Berlekamp-Peterson and Berlekamp-Massey Algorithms for error-location
*
* From Cain, Clark, "Error-Correction Coding For Digital Communications", pp. 205.
*
* This finds the coefficients of the error locator polynomial.
*
* The roots are then found by looking for the values of a^n
* where evaluating the polynomial yields zero.
*
* Error correction is done using the error-evaluator equation  on pp 207.
*
*/

//  PROJECT INCLUDES
//
#include "rs.h"
#include "galois_field.h"
#include "boxing/platform/types.h"
#include "boxing/platform/memory.h"

//  DEFINES
//

typedef void (*rs_encode_impl)(rs_codec *rs, gvector * data, gvector * data_encode);
typedef void(*rs_decode_impl)(rs_codec *rs, gvector * data, gvector * data_decode, unsigned int *resolved_errors, unsigned int *fatal_errors, int *max_errors_per_block);

struct rs_codec_s
{
    uint32_t * generator_polynomial;
    galois_field *galois_field;
    int message_size;
    int parity_size;
    rs_encode_impl encode;
    rs_decode_impl decode;
};

//  CONSTANTS
//

//  PRIVATE INTERFACE
//

static void encode_8(rs_codec *rs, gvector * data, gvector * data_encode);
static void decode_8(rs_codec *rs, gvector * data, gvector * data_decode, unsigned int *resolved_errors, unsigned int *fatal_errors, int *max_errors_per_block);
static void encode_16(rs_codec *rs, gvector * data, gvector * data_encode);
static void decode_16(rs_codec *rs, gvector * data, gvector * data_decode, unsigned int *resolved_errors, unsigned int *fatal_errors, int *max_errors_per_block);

static void rs_generate_polynomial(rs_codec * rs);
static void compute_modified_omega(rs_codec * rs, uint32_t *error_locator_polynomial, uint32_t *error_evaluator_polynomial, uint32_t *syndrome_bytes);
static void modified_berlekamp_massey(rs_codec *rs, uint32_t *error_locator_polynomial, uint32_t *error_evaluator_polynomial, uint32_t *syndrome_bytes);
static uint32_t find_roots(rs_codec *rs, uint32_t *error_locator_polynomial, uint32_t *error_locations);
static uint32_t correct_errors_erasures(rs_codec *rs, uint32_t *codeword, uint32_t codeword_size, uint32_t *syndrome_bytes, unsigned int *fatal_errors, unsigned int *resolved_errors);

// PUBLIC RS FUNCTIONS
//

rs_codec* rs_create(int message_size, int parity_size, uint32_t prime_plonomial)
{
    rs_codec* rs = BOXING_MEMORY_ALLOCATE_TYPE(rs_codec);
    
    rs->galois_field = gf_create(prime_plonomial);
    if (rs->galois_field->alphabet_size < (1 << 9))
    {
        rs->encode = encode_8;
        rs->decode = decode_8;
    }
    else if (rs->galois_field->alphabet_size < (1 << 17))
    {
        rs->encode = encode_16;
        rs->decode = decode_16;
    }
    else
    {
        rs_free(rs);
        return NULL;
    }
    rs->message_size = message_size;
    rs->parity_size = parity_size;

    rs_generate_polynomial(rs);
    return rs;
}

void rs_free(rs_codec *rs)
{
    gf_free(rs->galois_field);
    boxing_memory_free(rs->generator_polynomial);
    boxing_memory_free(rs);
}

void rs_encode(rs_codec *rs, gvector * data, gvector * data_encode)
{
    rs->encode(rs, data, data_encode);
}

static void encode_8(rs_codec *rs, gvector * data, gvector * data_encode)
{
	uint8_t * data_pointer = (uint8_t *)data->buffer;
	uint8_t * data_encode_pointer = (uint8_t *)data_encode->buffer;
    int parity_size = rs->parity_size;
    int message_size = rs->message_size;
    uint32_t *LFSR = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, (parity_size + 1));
	uint32_t dbyte;
    galois_field *gf = rs->galois_field;

	for (int position = 0, position_next = 0; position < (int)data->size; position += message_size, position_next += (message_size + parity_size)) {
		memset(LFSR, 0, (parity_size + 1)*sizeof(*LFSR));

		for (int i = 0; i < message_size; i++)
		{
			dbyte = (uint8_t)data_pointer[position + i] ^ LFSR[parity_size - 1];
			for (int j = parity_size - 1; j > 0; j--)
			{
				LFSR[j] = LFSR[j - 1] ^ gf_multiply(gf, rs->generator_polynomial[j], dbyte);
			}
            LFSR[0] = gf_multiply(gf, rs->generator_polynomial[0], dbyte);
		}
		for (int i = 0; i < message_size; i++)
		{
			data_encode_pointer[i + position_next] = data_pointer[i + position];
		}
		for (int i = 0; i < parity_size; i++)
		{
			data_encode_pointer[position_next + message_size + i] = (uint8_t)LFSR[parity_size - 1 - i];
		}
	}
    BOXING_STACK_FREE( LFSR );    
}

static void encode_16(rs_codec *rs, gvector * data, gvector * data_encode)
{
    uint16_t * data_pointer = (uint16_t *)data->buffer;
    uint16_t * data_encode_pointer = (uint16_t *)data_encode->buffer;
    int parity_size = rs->parity_size;
    int message_size = rs->message_size;
    uint32_t *LFSR = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, (parity_size + 1));
    uint32_t dbyte;
    galois_field *gf = rs->galois_field;

    for (int position = 0, position_next = 0; position < (int)data->size; position += message_size, position_next += (message_size + parity_size)) {
        memset(LFSR, 0, (parity_size + 1)*sizeof(*LFSR));

        for (int i = 0; i < message_size; i++)
        {
            dbyte = (uint16_t)data_pointer[position + i] ^ LFSR[parity_size - 1];
            for (int j = parity_size - 1; j > 0; j--)
            {
                LFSR[j] = LFSR[j - 1] ^ gf_multiply(gf, rs->generator_polynomial[j], dbyte);
            }
            LFSR[0] = gf_multiply(gf, rs->generator_polynomial[0], dbyte);
        }
        for (int i = 0; i < message_size; i++)
        {
            data_encode_pointer[i + position_next] = data_pointer[i + position];
        }
        for (int i = 0; i < parity_size; i++)
        {
            data_encode_pointer[position_next + message_size + i] = (uint16_t)LFSR[parity_size - 1 - i];
        }
    }
    BOXING_STACK_FREE( LFSR );    
}

void rs_decode(rs_codec *rs, gvector * data, gvector * data_decode, unsigned int *resolved_errors, unsigned int *fatal_errors, int *max_errors_per_block)
{
    rs->decode(rs, data, data_decode, resolved_errors, fatal_errors, max_errors_per_block);
}

static void decode_8(rs_codec *rs, gvector * data, gvector * data_decode, unsigned int *resolved_errors, unsigned int *fatal_errors, int *max_errors_per_block)
{
    int parity_size = rs->parity_size;
    int message_size = rs->message_size;
    uint32_t block_size = message_size + parity_size;
    uint8_t * data_pointer = (uint8_t *)data->buffer;
    uint8_t * data_decode_pointer = (uint8_t *)data_decode->buffer;
    galois_field *gf = rs->galois_field;

    for (int position = 0, position_next = 0; position < (int)data->size; position += (block_size), position_next += message_size)
    {
        uint32_t * codeword = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(uint32_t, block_size);
        uint32_t * syndrome_bytes = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size);
        for (uint32_t i = 0; i < (block_size); i++)
        {
            codeword[i] = (uint32_t)data_pointer[i + position];
        }

        DBOOL has_errors = DFALSE;
        for (uint32_t j = 1; j <= (uint32_t)parity_size; j++)
        {
            uint32_t sum = 0;
            for (uint32_t i = 0; i < (uint32_t)(block_size); i++)
            {
                sum = codeword[i] ^ gf_roots_summ(gf, j, sum);
            }
            syndrome_bytes[j - 1] = sum;
            has_errors = has_errors || sum != 0;
        }

        if (has_errors)
        {
            uint32_t errors_number = correct_errors_erasures(rs, codeword, block_size, syndrome_bytes, fatal_errors, resolved_errors);
            if (*max_errors_per_block < (int)errors_number)
            {
                *max_errors_per_block = errors_number;
            }
        }

        for (int32_t i = 0; i < message_size; i++)
        {
            data_decode_pointer[i + position_next] = (uint8_t)codeword[i];
        }

        boxing_memory_free(codeword);
        boxing_memory_free(syndrome_bytes);
    }
}

static void decode_16(rs_codec *rs, gvector * data, gvector * data_decode, unsigned int *resolved_errors, unsigned int *fatal_errors, int *max_errors_per_block)
{
    int parity_size = rs->parity_size;
    int message_size = rs->message_size;
    uint32_t block_size = message_size + parity_size;
    uint16_t * data_pointer = (uint16_t *)data->buffer;
    uint16_t * data_decode_pointer = (uint16_t *)data_decode->buffer;
    galois_field *gf = rs->galois_field;

    for (int position = 0, position_next = 0; position < (int)data->size; position += (block_size), position_next += message_size)
    {
        uint32_t * codeword = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(uint32_t, block_size);
        uint32_t * syndrome_bytes = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size);
        for (uint32_t i = 0; i < (block_size); i++)
        {
            codeword[i] = (uint32_t)data_pointer[i + position];
        }

        DBOOL has_errors = DFALSE;
        for (uint32_t j = 1; j <= (uint32_t)parity_size; j++)
        {
            uint32_t sum = 0;
            for (uint32_t i = 0; i < (uint32_t)(block_size); i++)
            {
                sum = codeword[i] ^ gf_roots_summ(gf, j, sum);
            }
            syndrome_bytes[j - 1] = sum;
            has_errors = has_errors || sum != 0;
        }

        if (has_errors)
        {
            uint32_t errors_number = correct_errors_erasures(rs, codeword, block_size, syndrome_bytes, fatal_errors, resolved_errors);
            if (*max_errors_per_block < (int)errors_number)
            {
                *max_errors_per_block = errors_number;
            }
        }

        for (int32_t i = 0; i < message_size; i++)
        {
            data_decode_pointer[i + position_next] = (uint16_t)codeword[i];
        }

        boxing_memory_free(codeword);
        boxing_memory_free(syndrome_bytes);
    }
}

// PRIVATE RS FUNCTIONS
//

static void rs_generate_polynomial(rs_codec * rs)
{
    uint32_t parity_size = rs->parity_size;
    uint32_t polynomial_size = parity_size * 2;
    galois_field *gf = rs->galois_field;


    uint32_t *tp = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, 2);
    uint32_t *tp1 = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, polynomial_size);
    uint32_t * polynomial = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(uint32_t, polynomial_size + 2 - 1);

    /* multiply (x + a^n) for n = 1 to nbytes */
    memset(tp1, 0, polynomial_size * sizeof(uint32_t));
    tp1[0] = 1;

    for (uint32_t i = 1; i <= parity_size; i++) {
        tp[0] = gf->exp[i]; /* set up x+a^n */
        tp[1] = 1;

        gf_multiply_polynomial(gf, polynomial, tp, 2, tp1, i);
        memcpy(tp1, polynomial, (2 + i - 1) * sizeof(uint32_t));
    }

    rs->generator_polynomial = polynomial;
    BOXING_STACK_FREE( tp1 );    
    BOXING_STACK_FREE( tp );    
}

static void compute_modified_omega(
    rs_codec * rs,
    uint32_t *error_locator_polynomial,
    uint32_t *error_evaluator_polynomial,
    uint32_t *syndrome_bytes)
{
    uint32_t i, j;
    uint32_t parity_size = rs->parity_size;
    galois_field * gf = rs->galois_field;

    memset(error_evaluator_polynomial, 0, sizeof(uint32_t) * parity_size * 2);


    for (i = 0; i < parity_size; i++)
    {

        for (j = i; (j)<parity_size; j++)
        {
            error_evaluator_polynomial[j] ^= gf_multiply(gf, syndrome_bytes[j - i], error_locator_polynomial[i]);
        }
    }
}

static void modified_berlekamp_massey(
    rs_codec *rs,
    uint32_t *error_locator_polynomial,
    uint32_t *error_evaluator_polynomial,
    uint32_t *syndrome_bytes)
{
    uint32_t L, L2, k, d;
    uint32_t n;

    uint32_t parity_size = rs->parity_size;
    galois_field *gf = rs->galois_field;

    uint32_t *psi = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size * 2);
    uint32_t *psi2 = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size * 2);
    uint32_t *D = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size * 2);

    memset(D, 0, sizeof(uint32_t) * parity_size * 2);
    memset(psi, 0, sizeof(uint32_t) * parity_size * 2);
    psi[0] = D[1] = 1;

    k = -1;
    L = 0;

    for (n = 0; n < parity_size; n++)
    {
        uint32_t sum = 0;
        for (uint32_t i = 0; i <= L; i++)
            sum ^= gf_multiply(gf, psi[i], syndrome_bytes[n - i]);
        d = sum;

        if (d != 0) {

            /* psi2 = psi - d*D */
            for (unsigned int i = 0; i < parity_size * 2; i++)
            {
                psi2[i] = psi[i] ^ gf_multiply(gf, d, D[i]);
            }


            if (L < (n - k))
            {
                L2 = n - k;
                k = n - L;
                /* D = scale_poly(ginv(d), psi); */
                for (unsigned int i = 0; i < parity_size * 2; i++)
                {
                    D[i] = gf_multiply(gf, psi[i], gf_inverse(gf, d));
                }
                L = L2;
            }

            /* psi = psi2 */
            memcpy(psi, psi2, sizeof(uint32_t) * parity_size * 2);
        }

        for (int i = parity_size * 2 - 1; i > 0; i--)
            D[i] = D[i - 1];
        D[0] = 0;
    }

    memcpy(error_locator_polynomial, psi, sizeof(uint32_t) * parity_size * 2); // parity_size

    compute_modified_omega(rs, error_locator_polynomial, error_evaluator_polynomial, syndrome_bytes); // parity size

    BOXING_STACK_FREE( psi );    
    BOXING_STACK_FREE( psi2 );    
    BOXING_STACK_FREE( D );    
}

static uint32_t find_roots(rs_codec *rs, uint32_t *error_locator_polynomial, uint32_t *error_locations)
{
    uint32_t sum, r, k, loop_to;
    uint32_t parity_size = rs->parity_size;
    uint32_t errors_number = 0;
    galois_field *gf = rs->galois_field;

    loop_to = parity_size;

    /* searching for error locations by calculating all
     * roots in the alphabet */
    for (r = 1; r < gf->alphabet_size; r++)
    {
        sum = 0;
        /* evaluate error_locator_polynomial at r */
        for (k = 0; k <= loop_to; k++)
        {
            sum ^= gf_roots_summ(gf, (k*r) % gf->mask, error_locator_polynomial[k]);
        }
        if (sum == 0)
        {            
            /* there is no need to continue to evaluate the
             * error_locator_polynomial in case where the number 
             * of errors exceeds the parity size as the results 
             * will not relieable */
            if (errors_number >= parity_size)
            {
                return errors_number + 1;
            }
            error_locations[errors_number] = (gf->mask - r);
            errors_number++;
        }
    }
    return errors_number;
}

static uint32_t correct_errors_erasures(
    rs_codec *rs,
    uint32_t *codeword,
    uint32_t codeword_size,
    uint32_t *syndrome_bytes,
    unsigned int *fatal_errors,
    unsigned int *resolved_errors)
{

    uint32_t r, i, j, err;

    uint32_t parity_size = rs->parity_size;
    galois_field *gf = rs->galois_field;

    uint32_t *error_locator_polynomial = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size * 2); // may be optimized to parity_size
    uint32_t *error_evaluator_polynomial = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size * 2); // may be optimized to parity_size
    uint32_t *error_locations = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, parity_size);

    modified_berlekamp_massey(rs, error_locator_polynomial, error_evaluator_polynomial, syndrome_bytes);
    uint32_t errors_number = find_roots(rs, error_locator_polynomial, error_locations);

    if ((errors_number <= parity_size) && errors_number > 0) {
        /* first check for illegal error locs */
        for (r = 0; r < errors_number; r++) {
            if (error_locations[r] >= codeword_size) {
                *fatal_errors += errors_number;
                BOXING_STACK_FREE( error_locator_polynomial );
                BOXING_STACK_FREE( error_evaluator_polynomial );
                BOXING_STACK_FREE( error_locations );
                return errors_number;
            }
        }
        for (r = 0; r < errors_number; r++) {
            int num, denom;
            i = error_locations[r];
            /* evaluate Omega at alpha^(-i) */
            num = 0;
            for (j = 0; j < parity_size * 2; j++)
                num ^= gf_multiply(gf, error_evaluator_polynomial[j], gf->exp[(((gf->mask - i)*j) % gf->mask)]);

            /* evaluate error_locator_polynomial' (derivative) at alpha^(-i) ; all odd powers disappear */
            denom = 0;
            for (j = 1; j < parity_size * 2; j += 2) {
                denom ^= gf_multiply(gf, error_locator_polynomial[j], gf->exp[(((gf->mask - i)*(j - 1)) % gf->mask)]);
            }
            err = gf_multiply(gf, num, gf_inverse(gf, denom));
            codeword[codeword_size - i - 1] ^= err;
        }
        *resolved_errors += errors_number;
        BOXING_STACK_FREE( error_locator_polynomial );
        BOXING_STACK_FREE( error_evaluator_polynomial );
        BOXING_STACK_FREE( error_locations );
        return errors_number;
    }
    else {
        if (errors_number)
        {
            *fatal_errors += errors_number;
        }
        BOXING_STACK_FREE( error_locator_polynomial );
        BOXING_STACK_FREE( error_evaluator_polynomial );
        BOXING_STACK_FREE( error_locations );
        return errors_number;
    }
}
