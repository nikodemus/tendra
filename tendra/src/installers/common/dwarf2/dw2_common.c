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
#include "fmm.h"

#include "common_types.h"
#include "basicread.h"
#include "install_fns.h"
#include "dg_aux.h"
#include "externs.h"
#include "dg_globs.h"
#include "shapemacs.h"
#include "dw2_common.h"


static dg_name_list capnames;
static int cni = 0;

static void
make_bastype(char * s, dg_type t)
{
	dg_name this_name;
	dg_tag ans = (dgtag_struct *) xmalloc (sizeof(dgtag_struct));
	init_dgtag (ans);
	ans->outref.k = LAB_STR;
	ans->outref.u.s = s;
	this_name = f_dg_tag_name (ans, f_dg_type_name (
								   no_dg_idname_option,
								   f_dg_null_sourcepos,
								   no_dg_accessibility_option,
								   yes_dg_type_option (t),
								   f_false,
								   no_bool_option,
								   no_dg_constraint_list_option));
	capnames = add_dg_name_list (capnames, this_name, cni++);
}


static dg_idname
m_id(char * s)
{
	dg_idname ans;
	ans.id_key = DG_ID_SRC;
	ans.idd.nam = s;
	return ans;
}


static void
make_basic_types()
{
	make_bastype ("__D_void_star", f_dg_address_type (
					  m_id ("void *"), f_pointer (f_alignment (scharsh))));
	make_bastype ("__D_signed_char", f_dg_char_type (
					  m_id ("signed char"), scharsh));
	make_bastype ("__D_unsigned_char", f_dg_char_type (
					  m_id ("unsigned char"), ucharsh));
	make_bastype ("__D_short", f_dg_integer_type (
					  m_id ("short"), swordsh));
	make_bastype ("__D_unsigned_short", f_dg_integer_type (
					  m_id ("unsigned short"), uwordsh));
	make_bastype ("__D_int", f_dg_integer_type (
					  m_id ("int"), slongsh));
	make_bastype ("__D_unsigned_int", f_dg_integer_type (
					  m_id ("unsigned int"), ulongsh));
	make_bastype ("__D_long", f_dg_integer_type (
					  m_id ("long"), slongsh));
	make_bastype ("__D_unsigned_long", f_dg_integer_type (
					  m_id ("unsigned long"), ulongsh));
	make_bastype ("__D_long_long", f_dg_integer_type (
					  m_id ("long long"), s64sh));
	make_bastype ("__D_unsigned_long_long", f_dg_integer_type (
					  m_id ("unsigned long long"), u64sh));
	make_bastype ("__D_float", f_dg_float_type (
					  m_id ("float"), FV_REAL_SHORT));
	make_bastype ("__D_double", f_dg_float_type (
					  m_id ("double"), FV_REAL));
	make_bastype ("__D_long_double", f_dg_float_type (
					  m_id ("long double"), FV_DOUBLE));
	make_bastype ("__D_complex", f_dg_complex_float_type (
					  m_id ("complex"), FV_COMPLEX_SHORT));
	make_bastype ("__D_double_complex", f_dg_complex_float_type (
					  m_id ("double complex"), FV_COMPLEX));
	make_bastype ("__D_long_double_complex", f_dg_complex_float_type (
					  m_id ("long double complex"), FV_COMPLEX_DOUBLE));
}


void
make_dwarf_common()
{
	dg_compilation ans;
	capnames = new_dg_name_list (15);
	make_basic_types ();
	ans = (dg_compilation) xmalloc (sizeof (struct dg_comp_t));
	ans->prim_file = get_filename ((long)0, "", "", "");
	ans->comp_deps = new_string_list (0);
	ans->date = 0;
	ans->language = 1;	/* assume ANSI C */
	ans->id_case = 0;	/* case sensitive */
	ans->producer = "TenDRA";
	ans->comp_dir = get_filename ((long)0, "", "", "");
	ans->options = new_string_list (0);
	ans->dn_list = capnames;
	ans->macros = new_dg_macro_list (0);
	ans->another = (dg_compilation)0;
	all_comp_units = ans;
}