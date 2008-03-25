/*
 * Copyright (c) 2002-2006 The TenDRA Project <http://www.tendra.org/>.
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
 * extract.c - Front end to library extraction mode of TDF linker.
 *
 * This file provides the front end to the library extraction mode of the TDF
 * linker.
 */

#include <stdlib.h>

#include "../../shared/check/check.h"
#include "extract.h"
#include <exds/common.h>
#include <exds/error.h>
#include "../gen-errors.h"
#include "../adt/library.h"

#include "../adt/solve-cycles.h"

void
extract_main(ArgDataT *arg_data)
{
    BoolT     extract_all  = arg_data_get_extract_all(arg_data);
    BoolT     extract_base = arg_data_get_extract_basename(arg_data);
    BoolT     match_base   = arg_data_get_extract_match_base(arg_data);
    unsigned  num_files    = arg_data_get_num_files(arg_data);
    char * *files        = arg_data_get_files(arg_data);
    LibraryT * library;

    if (extract_all && (num_files > 1)) {
	E_all_specified_with_capsules();
	UNREACHED;
    } else if ((!extract_all) && (num_files == 1)) {
	E_no_capsules_specified();
	UNREACHED;
    }
    if ((library = library_create_stream_input(files[0])) !=
	NULL) {
	if (extract_all) {
	    library_extract_all(library, extract_base);
	} else {
	    num_files--;
	    files++;
	    library_extract(library, extract_base, match_base, num_files,
			     files);
	}
	library_close(library);
    } else {
	E_cannot_open_input_file(files[0]);
    }
    if (error_max_reported_severity() >= ERROR_SEVERITY_ERROR) {
	exit(EXIT_FAILURE);
	UNREACHED;
    }
}
