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
#include "boxing/platform/types.h"
#include "boxing/platform/memory.h"


//  DEFINES
//


//  CONSTANTS
//

static const uint32_t exponents[512] = 
{
    1,2  ,4  ,8  ,16 ,32 ,64 ,128,29 ,58 ,116,232,205,135,19 ,38 ,76 ,152,45 ,90 ,180,117,234,201,143,3  ,6  ,12 ,24 ,48 ,96 ,192,157,39 ,78 ,156,
    37 ,74 ,148,53 ,106,212,181,119,238,193,159,35 ,70 ,140,5  ,10 ,20 ,40 ,80 ,160,93 ,186,105,210,185,111,222,161,95 ,190,97 ,194,153,47 ,94 ,
    188,101,202,137,15 ,30 ,60 ,120,240,253,231,211,187,107,214,177,127,254,225,223,163,91 ,182,113,226,217,175,67 ,134,17 ,34 ,68 ,136,13 ,26 ,
    52 ,104,208,189,103,206,129,31 ,62 ,124,248,237,199,147,59 ,118,236,197,151,51 ,102,204,133,23 ,46 ,92 ,184,109,218,169,79 ,158,33 ,66 ,132,
    21 ,42 ,84 ,168,77 ,154,41 ,82 ,164,85 ,170,73 ,146,57 ,114,228,213,183,115,230,209,191,99 ,198,145,63 ,126,252,229,215,179,123,246,241,255,
    227,219,171,75 ,150,49 ,98 ,196,149,55 ,110,220,165,87 ,174,65 ,130,25 ,50 ,100,200,141,7  ,14 ,28 ,56 ,112,224,221,167,83 ,166,81 ,162,89 ,
    178,121,242,249,239,195,155,43 ,86 ,172,69 ,138,9  ,18 ,36 ,72 ,144,61 ,122,244,245,247,243,251,235,203,139,11 ,22 ,44 ,88 ,176,125,250,233,
    207,131,27 ,54 ,108,216,173,71 ,142,1  ,2  ,4  ,8  ,16 ,32 ,64 ,128,29 ,58 ,116,232,205,135,19 ,38 ,76 ,152,45 ,90 ,180,117,234,201,143,3  ,
    6  ,12 ,24 ,48 ,96 ,192,157,39 ,78 ,156,37 ,74 ,148,53 ,106,212,181,119,238,193,159,35 ,70 ,140,5  ,10 ,20 ,40 ,80 ,160,93 ,186,105,210,185,
    111,222,161,95 ,190,97 ,194,153,47 ,94 ,188,101,202,137,15 ,30 ,60 ,120,240,253,231,211,187,107,214,177,127,254,225,223,163,91 ,182,113,226,
    217,175,67 ,134,17 ,34 ,68 ,136,13 ,26 ,52 ,104,208,189,103,206,129,31 ,62 ,124,248,237,199,147,59 ,118,236,197,151,51 ,102,204,133,23 ,46 ,
    92 ,184,109,218,169,79 ,158,33 ,66 ,132,21 ,42 ,84 ,168,77 ,154,41 ,82 ,164,85 ,170,73 ,146,57 ,114,228,213,183,115,230,209,191,99 ,198,145,
    63,126,252,229,215,179,123,246,241,255,227,219,171,75,150,49,98,196,149,55,110,220,165,87,174,65,130,25,50,100,200,141,7,14,28,56,112,224,221,
    167,83,166,81,162,89,178,121,242,249,239,195,155,43,86,172,69,138,9,18,36,72,144,61,122,244,245,247,243,251,235,203,139,11,22,44,88,176,125,250,
    233,207,131,27,54,108,216,173,71,142,1,3452816845
};

static const uint32_t logarithm[256] = 
{
    0  ,0  ,1  ,25 ,2  ,50 ,26 ,198,3  ,223,51 ,238,27 ,104,199,75 ,4  ,100,224,14 ,52 ,141,239,129,28 ,193,105,248,200,8  ,76 ,113,5  ,138,101,47 ,
    225,36 ,15 ,33 ,53 ,147,142,218,240,18 ,130,69 ,29 ,181,194,125,106,39 ,249,185,201,154,9  ,120,77 ,228,114,166,6  ,191,139,98 ,102,221,48 ,
    253,226,152,37 ,179,16 ,145,34 ,136,54 ,208,148,206,143,150,219,189,241,210,19 ,92 ,131,56 ,70 ,64 ,30 ,66 ,182,163,195,72 ,126,110,107,58 ,40 ,
    84 ,250,133,186,61 ,202,94 ,155,159,10 ,21 ,121,43 ,78 ,212,229,172,115,243,167,87 ,7  ,112,192,247,140,128,99 ,13 ,103,74 ,222,237,49 ,197,254,
    24 ,227,165,153,119,38 ,184,180,124,17 ,68 ,146,217,35 ,32 ,137,46 ,55 ,63 ,209,91 ,149,188,207,205,144,135,151,178,220,252,190,97 ,242,86 ,211,
    171,20 ,42 ,93 ,158,132,60 ,57 ,83 ,71 ,109,65 ,162,31 ,45 ,67 ,216,183,123,164,118,196,23 ,73 ,236,127,12 ,111,246,108,161,59 ,82 ,41 ,157,85 ,
    170,251,96 ,134,177,187,204,62 ,90 ,203,89 ,95 ,176,156,169,160,81 ,11 ,245,22 ,235,122,117,44 ,215,79 ,174,213,233,230,231,173,232,116,214,244,
    234,168,80 ,88 ,175
};

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
    
    galois_field* gf = BOXING_MEMORY_ALLOCATE_TYPE(galois_field);
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

    boxing_memory_free(gf->exp);
    boxing_memory_free(gf->log);
    boxing_memory_free(gf);
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
    uint32_t *tmp1 = BOXING_STACK_ALLOCATE_TYPE_ARRAY(uint32_t, p1_size + p2_size - 1);
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

    BOXING_STACK_FREE( tmp1 );
}

// PRIVATE RS FUNCTIONS
//

static void gf_init_tables(galois_field * gf)
{
    gf->exp = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(uint32_t, gf->alphabet_size * 2);
    gf->log = BOXING_MEMORY_ALLOCATE_TYPE_ARRAY(uint32_t, gf->alphabet_size);

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

