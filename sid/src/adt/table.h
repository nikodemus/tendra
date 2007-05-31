/*
 * Copyright (c) 2002-2005 The TenDRA Project <http://www.tendra.org/>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of The TenDRA Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific, prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
/*
    		 Crown Copyright (c) 1997

    This TenDRA(r) Computer Program is subject to Copyright
    owned by the United Kingdom Secretary of State for Defence
    acting through the Defence Evaluation and Research Agency
    (DERA).  It is made available to Recipients with a
    royalty-free licence for its use, reproduction, transfer
    to other parties and amendment for any purpose not excluding
    product development provided that any such use et cetera
    shall be deemed to be acceptance of the following conditions:-

        (1) Its Recipients shall ensure that this Notice is
        reproduced upon any copies or amended versions of it;

        (2) Any amended version of it shall be clearly marked to
        show both the nature of and the organisation responsible
        for the relevant amendment or amendments;

        (3) Its onward transfer from a recipient to another
        party shall be deemed to be that party's acceptance of
        these conditions;

        (4) DERA gives no warranty or assurance as to its
        quality or suitability for any purpose and DERA accepts
        no liability whatsoever in relation to any use to which
        it may be put.
*/

/*
 * table.h - Identifier table ADT.
 *
 * See the file "table.c" for more information.
 */

#ifndef H_TABLE
#define H_TABLE

#include "../os-interface.h"
#include "../eds/dstring.h"
#include "entry.h"
#include "../eds/ostream.h"
#include "key.h"

/* To avoid cicularity: */
struct GrammarT;

#define TABLE_SIZE	(127)

typedef struct TableT {
    EntryT *			contents[TABLE_SIZE];
} TableT;

extern void	table_init(TableT *);
extern EntryT *	table_add_type(TableT *, NStringT *);
extern EntryT *	table_add_basic(TableT *, NStringT *, struct GrammarT *, BoolT);
extern EntryT *	table_add_action(TableT *, NStringT *);
extern EntryT *	table_add_rule(TableT *, NStringT *);
extern EntryT *	table_add_generated_rule(TableT *, BoolT);
extern EntryT *	table_add_name(TableT *, NStringT *);
extern EntryT *	table_add_generated_name(TableT *);
extern EntryT *	table_add_rename(TableT *);
extern EntryT *	table_add_non_local(TableT *, NStringT *, EntryT *);
extern EntryT *	table_get_entry(TableT *, NStringT *);
extern EntryT *	table_get_type(TableT *, NStringT *);
extern EntryT *	table_get_basic(TableT *, NStringT *);
extern EntryT *	table_get_basic_by_number(TableT *, unsigned);
extern EntryT *	table_get_action(TableT *, NStringT *);
extern EntryT *	table_get_rule(TableT *, NStringT *);
extern void	table_iter(TableT *, void(*)(EntryT *, void *), void *);
extern void	table_untrace(TableT *);
extern void	table_unlink_untraced_rules(TableT *);

#endif /* !defined (H_TABLE) */
