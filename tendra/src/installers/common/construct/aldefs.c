/*
 * Copyright (c) 2002, The Tendra Project <http://www.ten15.org/>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *    		 Crown Copyright (c) 1997
 *
 *    This TenDRA(r) Computer Program is subject to Copyright
 *    owned by the United Kingdom Secretary of State for Defence
 *    acting through the Defence Evaluation and Research Agency
 *    (DERA).  It is made available to Recipients with a
 *    royalty-free licence for its use, reproduction, transfer
 *    to other parties and amendment for any purpose not excluding
 *    product development provided that any such use et cetera
 *    shall be deemed to be acceptance of the following conditions:-
 *
 *        (1) Its Recipients shall ensure that this Notice is
 *        reproduced upon any copies or amended versions of it;
 *
 *        (2) Any amended version of it shall be clearly marked to
 *        show both the nature of and the organisation responsible
 *        for the relevant amendment or amendments;
 *
 *        (3) Its onward transfer from a recipient to another
 *        party shall be deemed to be that party's acceptance of
 *        these conditions;
 *
 *        (4) DERA gives no warranty or assurance as to its
 *        quality or suitability for any purpose and DERA accepts
 *        no liability whatsoever in relation to any use to which
 *        it may be put.
 *
 * $TenDRA$
 */


#include "config.h"
#include "msgcat.h"

#include "common_types.h"
#include "installglob.h"
#include "basicread.h"
#include "messages_c.h"

#include "aldefs.h"

#define max(x,y) ((x)>(y)) ? (x) : (y)


/* The alignment definitions form a set of simultaneous equations
 *   of the form
 *
 *     a1 = f1(a1,a2 .. an)
 *     a2 = f2(a1,a2 ... an)
 *     ...
 *
 *   The functions f1 f2 etc. are formed entirely from f_alignment,
 *   which takes a shape and delivers an alignment, and unite_sets.
 *   f_alignment provides constants. Since the representation of
 *   alignments must be a homomorphism of sets under unite_sets, and
 *   since any program contains a finite number of basic alignments,
 *   these equations can be solved by iterating unite_sets until
 *   no further change occurs. Unite_sets is represented here by max.
 */

void
process_aldefs(void)
{
	struct aldef_t *ap, *a1, *a2;
	int changed, complete;
	

	/* iterate until there will be no changes */	
	do {
		ap = top_aldef;
		changed = 0;
		complete = 1;
		while (ap != NULL) {
			switch (ap->al_n) {
			case ALDS_SOLVED:
				break;
			case ALDS_AB:
				a1 = ap->a;
				a2 = ap->b;
				if (a1->al_n == ALDS_SOLVED && a2->al_n == ALDS_SOLVED) {
					ap->al_n = ALDS_SOLVED;
					ap->al = max(a1->al, a2->al);
					changed = 1;
				} else
					complete = 0;
				break;
			case ALDS_A:
				a1 = ap->a;
				if (a1->al_n == ALDS_SOLVED) {
					ap->al_n = ALDS_SOLVED;
					ap->al = a1->al;
					changed = 1;
				} else
					complete = 0;
				break;
			default:
				MSG_fatal_illegal_alignment_state(ap->al_n);
			}
			ap = ap->next_aldef;
		}
	} while (changed);

	if (!complete)
		MSG_fatal_incomplete_alignment_eqs();
	return;
}
