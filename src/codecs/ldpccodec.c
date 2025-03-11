/*****************************************************************************
**
**  Implementation of the reed solomon codec interface
**
**  Creation date:  2016/06/28
**  Created by:     Piql
**
**
**  Copyright (c) 2016 Piql AS. All rights reserved.
**
**  This file is part of the boxing library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include "boxing/log.h"
#include <stdio.h>
#include "mod2sparse.h"
#include "mod2dense.h"
#include "rcode.h"
#include "distrib.h"
#include "rand.h"
#include "alloc.h"
#include "check.h"
#include "mod2convert.h"
#include "boxing/codecs/ldpccodec.h"
#include "enc.h"
#include "dec.h"
#include "boxing/utils.h"

//  DEFINES
//

#define MAX_DEGREE_LIMIT 256
#define CODEC_MEMBER(name) (((boxing_codec_ldpc *)codec)->name)
#define CODEC_BASE_MEMBER(name) (((boxing_codec_ldpc *)codec)->base.name)


//  CONSTANTS
//


//  PRIVATE INTERFACE
//

static DBOOL codec_encode(void * codec, gvector * data);
static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data);

/**
 * @brief prprp_decode
 *
 * @param
 
 * @param H         Parity check matrix
 * @param lratio    Likelihood ratios for bits
 * @param dblk      Place to store decoding
 * @param pchk      Place to store parity checks
 * @param bprb	    Place to store bit probabilities
 * @param max_iter  Max iterations
 * @return iterations
 */
static unsigned ldpc_decode_prprp(mod2sparse *H, double *lratio, char *dblk, char *pchk, double *bprb, int max_iter)
{
    int N, n, c;

    N = mod2sparse_cols(H);

    /* Initialize probability and likelihood ratios, and find initial guess. */

    initprp(H, lratio, dblk, bprb);

    /* Do up to abs(max_iter) iterations of probability propagation, stopping
       early if a codeword is found, unless max_iter is negative. */

    for (n = 0;; n++)
    {
        c = check(H, dblk, pchk);

        if (n == max_iter || n == -max_iter || (max_iter > 0 && c == 0))
        {
            break;
        }

        iterprp(H, lratio, dblk, bprb);
    }

    return n;
}

/* The procedures in this module obtain the generator matrix to use for
   encoding from the global variables declared in rcode.h */

/**
* @brief ENCODE A BLOCK ANY GENERATOR MATRIX.
*
* @param G
* @param sblk
* @param cblk
*/

static void ldpc_encode(mod2sparse *H, gen_matrix *gm, char *sblk, char *cblk)
{
    switch (gm->type)
    {
    case 's':
    { sparse_encode(sblk, cblk, H, gm);
    break;
    }
    case 'd':
    { 
        mod2dense *u = mod2dense_allocate(gm->dim.N - gm->dim.M, 1);
        mod2dense *v = mod2dense_allocate(gm->dim.M, 1);
        dense_encode(sblk, cblk, u, v, gm);
        mod2dense_free(v);
        mod2dense_free(u);
    break;
    }
    case 'm':
    { 
        mod2dense *u = mod2dense_allocate(gm->dim.M, 1);
        mod2dense *v = mod2dense_allocate(gm->dim.M, 1);
        mixed_encode(sblk, cblk, u, v, H, gm);
        mod2dense_free(v);
        mod2dense_free(u);
    break;
    }
    }
}


static void print_generator_check_info(mod2sparse *H, gen_matrix *gen_matrix)
{
    int i, j, c, c2;
    int M = gen_matrix->dim.M;
    int N = gen_matrix->dim.N;

    if (gen_matrix->type == 'd')
    {
        c = 0;
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < N - M; j++)
            {
                c += mod2dense_get(gen_matrix->data.G, i, j);
            }
        }
        fprintf(stderr,
            "Number of 1s per check in Inv(A) X B is %.1f\n", (double)c / M);
    }

    if (gen_matrix->type == 'm')
    {
        c = 0;
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < M; j++)
            {
                c += mod2dense_get(gen_matrix->data.G, i, j);
            }
        }
        c2 = 0;
        for (i = M; i < N; i++)
        {
            c2 += mod2sparse_count_col(H, gen_matrix->cols[i]);
        }
        fprintf(stderr,
            "Number of 1s per check in Inv(A) is %.1f, in B is %.1f, total is %.1f\n",
            (double)c / M, (double)c2 / M, (double)(c + c2) / M);
    }
}


static mod2sparse *mod2sparse_clone(mod2sparse *matrix)
{
    if (!matrix)
    {
        return NULL;
    }

    mod2sparse *clone = mod2sparse_allocate(matrix->n_rows, matrix->n_cols);

    if (!clone)
    {
        return NULL;
    }

    mod2sparse_copy(matrix, clone);

    return clone;
}


typedef enum
{ 
    gen_unknown,
    gen_sparse, 
    gen_dense, 
    gen_mixed 
} gen_make_method;      /* Ways of making it */


/**
* @brief MAKE DENSE OR MIXED REPRESENTATION OF GENERATOR MATRIX.
*/

static gen_matrix *ldpc_generator_make_dense_mixed(mod2sparse *H, gen_make_method method)
{
    mod2dense *DH, *A, *A2, *AI, *B;
    int i, n;
    int *rows_inv;
    int M = mod2sparse_rows(H);
    int N = mod2sparse_cols(H);
    //mod2sparse * H = par_chk->H;
    char type;

    if (method == gen_dense)
    {
        type = 'd';
    }
    else if (method == gen_mixed)
    {
        type = 'm';
    }
    else
    {
        return NULL;
    }

    /* Allocate space for row and column permutations. */
    gen_matrix *gm = malloc(sizeof(gen_matrix));

    gm->dim.M = mod2sparse_rows(H);
    gm->dim.N = mod2sparse_cols(H);
    gm->type = type;
    gm->cols = chk_alloc(N, sizeof *gm->cols);
    { // TODO: check this more closely, this is iffy; initializing both sides of a union?
        gm->data.sparse.L = NULL;
        gm->data.sparse.U = NULL;
        gm->data.sparse.rows = chk_alloc(M, sizeof *gm->data.sparse.rows);
    }

    DH = mod2dense_allocate(M, N);
    AI = mod2dense_allocate(M, M);
    B = mod2dense_allocate(M, N - M);
    gm->data.G = mod2dense_allocate(M, N - M);

    mod2sparse_to_dense(H, DH);


    A = mod2dense_allocate(M, N);
    A2 = mod2dense_allocate(M, N);

    n = mod2dense_invert_selected(DH, A2, gm->data.sparse.rows, gm->cols);
    mod2sparse_to_dense(H, DH);  /* DH was destroyed by invert_selected */

    if (n > 0)
    {
        fprintf(stderr, "Note: Parity check matrix has %d redundant checks\n", n);
    }

    rows_inv = chk_alloc(M, sizeof *rows_inv);

    for (i = 0; i < M; i++)
    {
        rows_inv[gm->data.sparse.rows[i]] = i;
    }

    mod2dense_copyrows(A2, A, gm->data.sparse.rows);
    mod2dense_copycols(A, A2, gm->cols);
    mod2dense_copycols(A2, AI, rows_inv);

    mod2dense_copycols(DH, B, gm->cols + M);

    /* Form final generator matrix. */

    if (method == gen_dense)
    {
        mod2dense_multiply(AI, B, gm->data.G);
    }
    else if (method == gen_mixed)
    {
        gm->data.G = AI;
    }

    /* Compute and print number of 1s. */

    print_generator_check_info(H, gm);

    return gm;
}

static void ldpc_generator_free(gen_matrix *gen_matrix)
{
    free(gen_matrix->cols);
    if (gen_matrix->type == 's') {
        free(gen_matrix->data.sparse.rows);
        free(gen_matrix->data.sparse.L);
        free(gen_matrix->data.sparse.U);
    } else {
        mod2dense_free(gen_matrix->data.G);
    }
    free(gen_matrix);
}


/**
* @brief PARTITION THE COLUMNS ACCORDING TO THE SPECIFIED PROPORTIONS.  It
* may not be possible to do this exactly.  Returns a pointer to an
* array of integers containing the numbers of columns corresponding
* to the entries in the distribution passed.
*
* @param d      List of proportions and number of check-bits
* @param n		 Total number of columns to partition
* @return      Returns a pointer to an array of integers on success, othervise NULL
*/

static int *column_partition(distrib *d, int n)
{
    double *trunc;
    int *part;
    int cur, used;
    int i, j;

    trunc = chk_alloc(distrib_size(d), sizeof(double));
    part = chk_alloc(distrib_size(d), sizeof(int));

    used = 0;
    for (i = 0; i < distrib_size(d); i++)
    {
        cur = (int)floor(distrib_prop(d, i)*n);
        part[i] = cur;
        trunc[i] = distrib_prop(d, i)*n - cur;
        used += cur;
    }

    if (used > n)
    {
        return NULL;
    }

    while (used < n)
    {
        cur = 0;
        for (j = 1; j < distrib_size(d); j++)
        {
            if (trunc[j] > trunc[cur])
            {
                cur = j;
            }
        }
        part[cur] += 1;
        used += 1;
        trunc[cur] = -1;
    }

    free(trunc);
    return part;
}


typedef enum
{
    pchk_unknown,
    pchk_evencol, 	/**< Uniform number of bits per column, with number specified */
    pchk_evenboth 	/**< Uniform (as possible) over both columns and rows */
} pchk_make_method;

/**
* @brief CREATE A SPARSE PARITY-CHECK MATRIX.  Of size M by N
*
* @param seed      Random number seed
* @param method    How to make it
* @param d         Distribution list specified
* @param no4cycle  Eliminate cycles of length four?
* @param M Random  Check bits
* @param N Random  Message size including check bits
* @return a sparse parity check matrix on success, othervise NULL
*/

static mod2sparse * ldcp_pchk_make(int seed, pchk_make_method method, distrib *d, int no4cycle, int M, int N)
{
    mod2entry *e, *f, *g, *h;
    int added, uneven, elim4, all_even, n_full, left;
    int i, j, k, t, z, cb_N;
    int *part, *u;

    rand_seed(10 * seed + 1);

    mod2sparse * H = mod2sparse_allocate(M, N);
    part = column_partition(d, N);
    if (!part)
    {
        mod2sparse_free(H);
        return NULL;
    }
    /* Create the initial version of the parity check matrix. */

    switch (method)
    {
    case pchk_evencol:
    {
        z = 0;
        left = part[z];

        for (j = 0; j < N; j++)
        {
            while (left == 0)
            {
                z += 1;
                if (z > distrib_size(d))
                {
                    mod2sparse_free(H);
                    return NULL;
                }
                left = part[z];
            }
            for (k = 0; k < distrib_num(d, z); k++)
            {
                do
                {
                    i = rand_int(M);
                } while (mod2sparse_find(H, i, j));
                mod2sparse_insert(H, i, j);
            }
            left -= 1;
        }

        break;
    }

    case pchk_evenboth:
    {
        cb_N = 0;
        for (z = 0; z < distrib_size(d); z++)
        {
            cb_N += distrib_num(d, z) * part[z];
        }

        u = chk_alloc(cb_N, sizeof *u);

        for (k = cb_N - 1; k >= 0; k--)
        {
            u[k] = k%M;
        }

        uneven = 0;
        t = 0;
        z = 0;
        left = part[z];

        for (j = 0; j < N; j++)
        {
            while (left == 0)
            {
                z += 1;
                if (z > distrib_size(d))
                {
                    mod2sparse_free(H);
                    return NULL;
                }
                left = part[z];
            }

            for (k = 0; k < distrib_num(d, z); k++)
            {
                for (i = t; i < cb_N && mod2sparse_find(H, u[i], j); i++);

                if (i == cb_N)
                {
                    uneven += 1;
                    do
                    {
                        i = rand_int(M);
                    } while (mod2sparse_find(H, i, j));
                    mod2sparse_insert(H, i, j);
                }
                else
                {
                    do
                    {
                        i = t + rand_int(cb_N - t);
                    } while (mod2sparse_find(H, u[i], j));
                    mod2sparse_insert(H, u[i], j);
                    u[i] = u[t];
                    t += 1;
                }
            }

            left -= 1;
        }

        if (uneven > 0)
        {
            fprintf(stderr, "Had to place %d checks in rows unevenly\n", uneven);
        }

        break;
    }

    default:
        mod2sparse_free(H);
        return NULL;
    }

    /* Add extra bits to avoid rows with less than two checks. */

    added = 0;

    for (i = 0; i < M; i++)
    {
        e = mod2sparse_first_in_row(H, i);
        if (mod2sparse_at_end(e))
        {
            j = rand_int(N);
            e = mod2sparse_insert(H, i, j);
            added += 1;
        }
        e = mod2sparse_first_in_row(H, i);
        if (mod2sparse_at_end(mod2sparse_next_in_row(e)) && N>1)
        {
            do
            {
                j = rand_int(N);
            } while (j == mod2sparse_col(e));
            mod2sparse_insert(H, i, j);
            added += 1;
        }
    }

    if (added > 0)
    {
        fprintf(stderr, "Added %d extra bit-checks to make row counts at least two\n", added);
    }

    /* Add extra bits to try to avoid problems with even column counts. */

    n_full = 0;
    all_even = 1;
    for (z = 0; z < distrib_size(d); z++)
    {
        if (distrib_num(d, z) == M)
        {
            n_full += part[z];
        }
        if (distrib_num(d, z) % 2 == 1)
        {
            all_even = 0;
        }
    }

    if (all_even && N - n_full > 1 && added < 2)
    {
        int a;
        for (a = 0; added + a < 2; a++)
        {
            do
            {
                i = rand_int(M);
                j = rand_int(N);
            } while (mod2sparse_find(H, i, j));
            mod2sparse_insert(H, i, j);
        }
        fprintf(stderr, "Added %d extra bit-checks to try to avoid problems from even column counts\n", a);
    }

    /* Eliminate cycles of length four, if asked, and if possible. */

    if (no4cycle)
    {
        elim4 = 0;

        for (t = 0; t < 10; t++)
        {
            k = 0;
            for (j = 0; j < N; j++)
            {
                for (e = mod2sparse_first_in_col(H, j);
                    !mod2sparse_at_end(e);
                    e = mod2sparse_next_in_col(e))
                {
                    for (f = mod2sparse_first_in_row(H, mod2sparse_row(e));
                        !mod2sparse_at_end(f);
                        f = mod2sparse_next_in_row(f))
                    {
                        if (f == e) continue;
                        for (g = mod2sparse_first_in_col(H, mod2sparse_col(f));
                            !mod2sparse_at_end(g);
                            g = mod2sparse_next_in_col(g))
                        {
                            if (g == f) continue;
                            for (h = mod2sparse_first_in_row(H, mod2sparse_row(g));
                                !mod2sparse_at_end(h);
                                h = mod2sparse_next_in_row(h))
                            {
                                if (mod2sparse_col(h) == j)
                                {
                                    do
                                    {
                                        i = rand_int(M);
                                    } while (mod2sparse_find(H, i, j));
                                    mod2sparse_delete(H, e);
                                    mod2sparse_insert(H, i, j);
                                    elim4 += 1;
                                    k += 1;
                                    goto nextj;
                                }
                            }
                        }
                    }
                }
            nextj:;
            }
            if (k == 0) break;
        }

        if (elim4 > 0)
        {
            fprintf(stderr, "Eliminated %d cycles of length four by moving checks within column\n", elim4);
        }

        if (t == 10)
        {
            fprintf(stderr, "Couldn't eliminate all cycles of length four in 10 passes\n");
        }
    }
    return H;
}


// PUBLIC LDPC FUNCTIONS
//

//----------------------------------------------------------------------------
/*!
 *  \brief Create an boxing_codec_ldpc instance.
 *
 *  Allocate memory for the boxing_codec_ldpc type
 *  and initializes all structure data according to the input values.
 *  Return instance of allocated structure.
 *
 *  \param[in]  properties  Hash table with codec properties.
 *  \param[in]  config      Pointer to the boxing_config structure.
 *  \return instance of allocated boxing_codec_ldpc structure reduced to boxing_codec structure.
 */

boxing_codec * boxing_codec_ldpc_create(GHashTable * properties, const boxing_config * config)
{
    BOXING_UNUSED_PARAMETER( config );
    boxing_codec_ldpc * codec = malloc(sizeof(boxing_codec_ldpc));

    g_variant * var_message_size = g_hash_table_lookup(properties, PARAM_NAME_MESSAGE_SIZE);
    if (var_message_size == NULL)
    {
        DLOG_ERROR1( "(boxing_codec_ldpc_create) Required property '%s' not set", PARAM_NAME_MESSAGE_SIZE );
        return NULL;
    }
    
    g_variant * var_parity_size = g_hash_table_lookup(properties, PARAM_NAME_PARITY_SIZE);
    if (var_parity_size == NULL)
    {
        DLOG_ERROR1( "(boxing_codec_ldpc_create) Required property '%s' not set", PARAM_NAME_PARITY_SIZE );
        return NULL;
    }

    boxing_codec_init_base((boxing_codec *)codec);
    codec->base.free = boxing_codec_ldpc_free;
    codec->base.is_error_correcting = DTRUE;
    codec->base.name = codec_ldpc_name;

    /* convert to bits */
    int message_size = g_variant_to_uint(var_message_size) * 8;
    /* convert to bits */
    int parity_size = g_variant_to_uint(var_parity_size) * 8;

    // configure ldpc
    // ex-ldpc36-1000a.pchk 1600 16000 1 evenboth 3 no4cycle
    codec->iterations = 25;
    int seed = 1;
    int no4cycle = 1;
    distrib *d = distrib_create("3");
    mod2sparse * H = ldcp_pchk_make(seed, pchk_evenboth, d, no4cycle, parity_size, parity_size + message_size);
    codec->pchk_matrix = H;
    codec->gen_matrix = ldpc_generator_make_dense_mixed(H, gen_dense);

    codec->base.decoded_symbol_size = 8;
    codec->base.decoded_block_size = message_size / 8;

    codec->base.encoded_symbol_size = 1;
    codec->base.encoded_block_size = message_size + parity_size;

    codec->base.decode = codec_decode;
    codec->base.encode = codec_encode;

    return (boxing_codec *)codec;
}


//----------------------------------------------------------------------------
/*!
 *  \brief Frees occupied memory of boxing_codec_ldpc structure.
 *
 *  Frees occupied memory of all internal structure pointers and structure pointer.
 *
 *  \param[in]  codec  Pointer to the boxing_codec structure.
 */

void boxing_codec_ldpc_free(boxing_codec * codec)
{
    mod2sparse_free(CODEC_MEMBER(pchk_matrix));
    ldpc_generator_free(CODEC_MEMBER(gen_matrix));
    boxing_codec_release_base(codec);
    free(codec);
}


//----------------------------------------------------------------------------
/*!
  * \} end of codecs group
  */


// PRIVATE LDPC CODEC FUNCTIONS
//

static void unpack_data(char *src, char *dst, unsigned int src_size)
{
    uint32_t *value = (uint32_t *)dst;
    char *src_end = src + src_size;
    while (src != src_end)
    {
        *value++ =
            (0x10 & *src) << 20 |
            (0x20 & *src) << 11 |
            (0x40 & *src) << 2  |
            (0x80 & *src) >> 7;
        *value++ =
            (0x01 & *src) << 24 |
            (0x02 & *src) << 15 |
            (0x04 & *src) << 6  |
            (0x08 & *src) >> 3;
        src++;
    }
}

static void pack_data(char *src, char *dst, unsigned int src_size)
{
    uint32_t *unpacked_data = (uint32_t *)src;
    char      packed_data;
    while (src_size)
    {
        packed_data =
            (0x01000000 & *unpacked_data) >> 20 |
            (0x00010000 & *unpacked_data) >> 11 |
            (0x00000100 & *unpacked_data) >> 2  |
            (0x00000001 & *unpacked_data) << 7;
        unpacked_data++;
        packed_data |=
            (0x01000000 & *unpacked_data) >> 24 |
            (0x00010000 & *unpacked_data) >> 15 |
            (0x00000100 & *unpacked_data) >> 6  |
            (0x00000001 & *unpacked_data) << 3;
        unpacked_data++;

        *dst = packed_data;
        dst++;
        src_size -= 8;
    }
}

static DBOOL codec_encode(void * codec, gvector * data)
{
    boxing_codec_ldpc * ldpc_codec = (boxing_codec_ldpc*)codec;

    mod2sparse *H = ldpc_codec->pchk_matrix;
    gen_matrix *gen_matrix = ldpc_codec->gen_matrix;
    gvector  *encoded_data = gvector_create(data->item_size, ldpc_codec->base.encoded_data_size);

    char *src = data->buffer;
    char *dst = encoded_data->buffer;
    int   blocks = (int)encoded_data->size / ldpc_codec->base.encoded_block_size;
    char *unpacked_data_block = calloc(ldpc_codec->base.decoded_block_size*ldpc_codec->base.decoded_symbol_size, sizeof(char));

    for (int i = 0; i < blocks; i++)
    {
        // convert data from byte to bit stream 
        unpack_data(src, unpacked_data_block, ldpc_codec->base.decoded_block_size);

        ldpc_encode(H, gen_matrix, unpacked_data_block, dst);
        src += ldpc_codec->base.decoded_block_size;
        dst += ldpc_codec->base.encoded_block_size;
    }
    free(unpacked_data_block);
    gvector_swap(data, encoded_data);
    gvector_free(encoded_data);
    return DTRUE;
}

static DBOOL codec_decode(void * codec, gvector * data, gvector * erasures, boxing_stats_decode * stats, void* user_data)
{
    BOXING_UNUSED_PARAMETER(erasures);
    BOXING_UNUSED_PARAMETER(user_data);


    BOXING_UNUSED_PARAMETER(stats); // not shure how to do this

    boxing_codec_ldpc * ldpc_codec = (boxing_codec_ldpc*)codec;

    mod2sparse *H = ldpc_codec->pchk_matrix;
    gen_matrix *gen_matrix = ldpc_codec->gen_matrix;
    gvector * decoded_data = gvector_create(data->item_size, ldpc_codec->base.decoded_data_size);

    char *src = data->buffer;
    char *dst = decoded_data->buffer;
    int blocks = (int)decoded_data->size / ldpc_codec->base.decoded_block_size;
    double *lratio = calloc(gen_matrix->dim.N, sizeof(double));
    char   *pchk   = calloc(gen_matrix->dim.M, sizeof(char));
    double *bitpr  = calloc(gen_matrix->dim.N, sizeof(double));
    char *recoverd_block_sd = calloc(gen_matrix->dim.N, sizeof(char));
    char *recoverd_block_hd = calloc(gen_matrix->dim.N, sizeof(char));
    char *data_block = calloc(gen_matrix->dim.N - gen_matrix->dim.M, sizeof(char));
    char *data_block_ptr;

    for (int i = 0; i < blocks; i++)
    {
        /* convert from packed log likelyhood ratio (char)
         * to likelyhood ratio (double)
         */
        for (int bit = 0; bit < gen_matrix->dim.N; bit++)
        {
            lratio[bit] = exp(src[bit]/10.0f);
            recoverd_block_hd[bit] = (lratio[bit] > 1.0f) ? 1 : 0;
        }
        (void)ldpc_decode_prprp(H, lratio, recoverd_block_sd, pchk, bitpr, ldpc_codec->iterations);

        int bit_alterations = 0;
        for (int bit = 0; bit < gen_matrix->dim.N; bit++)
        {
            bit_alterations += (recoverd_block_hd[bit] != recoverd_block_sd[bit]) ? 1 : 0;
        }
        //extract data
        data_block_ptr = data_block;
        for (int i = gen_matrix->dim.M; i < gen_matrix->dim.N; i++)
        {
            *data_block_ptr++ = recoverd_block_sd[(int)gen_matrix->cols[i]];
        }

        int c = 0;
        for (int i = 0; i < gen_matrix->dim.M; i++)
        {
            c += pchk[i];
        }

        if (c)
        {
            stats->unresolved_errors += bit_alterations;
        }
        else
        {
            stats->resolved_errors += bit_alterations;
        }
        // convert data from bit to byte stream 
        pack_data(data_block, dst, gen_matrix->dim.N - gen_matrix->dim.M);

        dst += ldpc_codec->base.decoded_block_size;
        src += ldpc_codec->base.encoded_block_size;
        //dst += ldpc_codec->base.decoded_block_size; 
    }

    free(lratio);
    free(pchk);
    free(bitpr);
    free(recoverd_block_sd);
    free(recoverd_block_hd);
    free(data_block);

    gvector_swap(data, decoded_data);
    gvector_free(decoded_data);
    return DTRUE;
}
