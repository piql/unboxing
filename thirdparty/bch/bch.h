/*
 * Generic binary BCH encoding/decoding library
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright © 2011 Parrot S.A.
 *
 * Author: Ivan Djelic <ivan.djelic@parrot.com>
 *
 * Description:
 *
 * This library provides runtime configurable encoding/decoding of binary
 * Bose-Chaudhuri-Hocquenghem (BCH) codes.
*/
#ifndef _BCH_H
#define _BCH_H

#include "boxing/platform/types.h"

#define BCH_PRIM_POLY_0x25        0x25    /*<< m = 5  */
#define BCH_PRIM_POLY_0x43        0x43    /*<< m = 6  */
#define BCH_PRIM_POLY_0x83        0x83    /*<< m = 7  */
#define BCH_PRIM_POLY_0x11D       0x11d   /*<< m = 8  */
#define BCH_PRIM_POLY_0x211       0x211   /*<< m = 9  */
#define BCH_PRIM_POLY_0x409       0x409   /*<< m = 10 */
#define BCH_PRIM_POLY_0x805       0x805   /*<< m = 11 */
#define BCH_PRIM_POLY_0x1053      0x1053  /*<< m = 12 */
#define BCH_PRIM_POLY_0x201B      0x201b  /*<< m = 13 */
#define BCH_PRIM_POLY_0x402B      0x402b  /*<< m = 14 */
#define BCH_PRIM_POLY_0x8003      0x8003  /*<< m = 15 */

/*
 * struct bch_control - BCH control structure
 * @m:          Galois field order
 * @n:          maximum codeword size in bits (= 2^m-1)
 * @t:          error correction capability in bits
 * @ecc_bits:   ecc exact size in bits, i.e. generator polynomial degree (<=m*t)
 * @ecc_bytes:  ecc max size (m*t bits) in bytes
 * @a_pow_tab:  Galois field GF(2^m) exponentiation lookup table
 * @a_log_tab:  Galois field GF(2^m) log lookup table
 * @mod8_tab:   remainder generator polynomial lookup tables
 * @ecc_buf:    ecc parity words buffer
 * @ecc_buf2:   ecc parity words buffer
 * @xi_tab:     GF(2^m) base for solving degree 2 polynomial roots
 * @syn:        syndrome buffer
 * @cache:      log-based polynomial representation buffer
 * @elp:        error locator polynomial
 * @poly_2t:    temporary polynomials of degree 2t
 */

struct bch_control {
	unsigned int    m;
	unsigned int    n;
	unsigned int    t;
	unsigned int    ecc_bits;
	unsigned int    ecc_bytes;
/* private: */
	uint16_t       *a_pow_tab;
	uint16_t       *a_log_tab;
	uint32_t       *mod8_tab;
	uint32_t       *ecc_buf;
	uint32_t       *ecc_buf2;
	unsigned int   *xi_tab;
	unsigned int   *syn;
	int            *cache;
	struct gf_poly *elp;
	struct gf_poly *poly_2t[4];
};

struct bch_control *init_bch(int m, int t, unsigned int prim_poly);

void free_bch(struct bch_control *bch);

void encode_bch(struct bch_control *bch, const uint8_t *data,
		unsigned int len, uint8_t *ecc);

int decode_bch(struct bch_control *bch, const uint8_t *data, unsigned int len,
	       const uint8_t *recv_ecc, const uint8_t *calc_ecc,
	       const unsigned int *syn, unsigned int *errloc);

#ifndef EINVAL
#define	EINVAL	22	/* Invalid argument */
#endif
#ifndef EBADMSG
#define	EBADMSG		74	/* Not a data message */
#endif

int deg(unsigned int poly);


#endif /* _BCH_H */
