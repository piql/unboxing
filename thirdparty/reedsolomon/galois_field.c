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
#include "galois_field.h"
#include "boxing/platform/platform.h"


//  DEFINES
//

//  PRIVATE INTERFACE
//
static void gf_init_tables(galois_field * gf);


// PUBLIC RS FUNCTIONS
//

galois_field * gf_create(uint32_t prime_plonomial)
{
    uint32_t polynomial = prime_plonomial >> 1;
    uint32_t polynomial_degree = 0;
    while (polynomial)
    {
        polynomial_degree++;
        polynomial >>= 1;
    }
    
    galois_field *gf = malloc(sizeof(galois_field));
    gf->prim_polynom = prime_plonomial;
    gf->alphabet_size = 1 << polynomial_degree;
    gf->mask = gf->alphabet_size - 1;

    gf_init_tables(gf);

    return gf;
}

void gf_free(galois_field *gf)
{
    if (!gf)
        return;

    free(gf->exp);
    free(gf->log);
    free(gf);
}

uint32_t gf_multiply(galois_field * gf, uint32_t a, uint32_t b)
{
    if (a == 0 || b == 0) return (0);
    return (gf->exp[gf->log[a] + gf->log[b]]);
}

uint32_t gf_roots_summ(galois_field * gf, uint32_t a, uint32_t b)
{
    if (b == 0 || gf->exp[a] == 0) return (0);
    return (gf->exp[a + gf->log[b]]);
}

uint32_t gf_inverse(galois_field * gf, uint32_t a)
{
    return (gf->exp[gf->mask - gf->log[a]]);
}

/*
* @brief calculatets the product of p1 * p2 and stores the result in dst
* @param dst      stores the product of p1 * p2 and is (p1_size + p2_size - 1) elements long
* @param p1       first polynomia
* @param p1_size  size of first polynomial
* @param p2       second polynomial
* @param p2_size  size of first polynomial
* returns the product of p1 * p2 and stores it in dst. The results have (p1_size + p2_size - 1) elements
*/
void gf_multiply_polynomial(galois_field *gf, uint32_t *dst, uint32_t *p1, uint32_t p1_size, uint32_t *p2, uint32_t p2_size)
{
    uint32_t i, j;
    uint32_t *tmp1 = alloca(sizeof(uint32_t) * (p1_size + p2_size - 1));
    memset(dst, 0, (p1_size + p2_size - 1) * sizeof(uint32_t));

    for (i = 0; i < p1_size; i++)
    {
        memset(tmp1, 0, (p1_size + p2_size - 1) * sizeof(uint32_t));

        for (j = i; (j - i)<p2_size; j++)
        {
            tmp1[j] = gf_multiply(gf, p2[j - i], p1[i]);
        }

        for (j = 0; j < (p1_size + p2_size - 1); j++)
        {
            dst[j] ^= tmp1[j];
        }
    }
}

// PRIVATE RS FUNCTIONS
//

static void gf_init_tables(galois_field * gf)
{
    gf->exp = calloc(gf->alphabet_size * 2, sizeof(uint32_t));
    gf->log = calloc(gf->alphabet_size, sizeof(uint32_t));

    memset(gf->exp, 0, gf->alphabet_size * 2);
    memset(gf->log, 0, gf->alphabet_size);

    uint32_t x = 1;
    for (uint32_t i = 0; i < gf->mask; i++)
    {
        gf->exp[i] = x;
        gf->log[x] = i;

        x <<= 1;
        if (x & gf->alphabet_size) // this is ok since size is allways 2^n
        {
            x ^= gf->prim_polynom;
        }
    }

    for (uint32_t i = gf->mask; i < (gf->alphabet_size * 2); i++)
    {
        gf->exp[i] = gf->exp[i - gf->mask];
    }

}

