/* $Id$ */

/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#ifndef LEXI_OUTPUT_COMMON_H
#define LEXI_OUTPUT_COMMON_H

#include <stdio.h>

/*
 * Common output routines shared between multiple languages.
 * These are used by the output-* code generation backends.
 */


enum output_comment {
	OUTPUT_COMMENT_C90,	/* block style C comments */
	OUTPUT_COMMENT_C99	/* single-line C comments */
};


/*
 * OUTPUT INDENTATION
 *
 * This routine outputs an indentation of d.
 */
void
output_indent(FILE *, unsigned int depth);

/*
 * OUTPUT GENERATED BY LEXI Version
 *
 * This routine outputs a statement stating that the file is automatically
 * generated.
 */
void
output_generated_by_lexi(enum output_comment, FILE *out);

#endif
