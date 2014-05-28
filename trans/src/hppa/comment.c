/* $Id$ */

/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stdio.h>
#include <stdlib.h>

#include <local/out.h>

#include <refactor/const.h>

#include "comment.h"


/*
    FLAG : PRINT COMMENTS?

    This is set in hppatrans.c.
*/

bool do_comment ;


/*
    COMMENTING ROUTINES

    If DO_COMMENT is not defined then these routines are replaced by
    the macros in comment.h.
*/

#ifdef DO_COMMENT

void
comment(char * mess)
{
    if ( do_comment ) {
	fprintf(as_file, "!! " ) ;
	fprintf ( as_file,"%s\n", mess ) ;
    }
    return ;
}

void
comment1(char * mess, long p1)
{
    if ( do_comment ) {
	fprintf(as_file, "!! " ) ;
	fprintf ( as_file, mess, p1 ) ;
	fputc('\n',as_file);
    }
    return ;
}

void
comment2(char * mess, long p1, long p2)
{
    if ( do_comment ) {
	fprintf(as_file, "!! " ) ;
	fprintf ( as_file, mess, p1, p2 ) ;
	fputc('\n', as_file);
    }
    return ;
}

void
comment3(char * mess, long p1, long p2, long p3)
{
    if ( do_comment ) {
	fprintf(as_file, "!! " ) ;
	fprintf ( as_file, mess, p1, p2, p3 ) ;
	fputc('\n', as_file);
    }
    return ;
}

void
comment4(char * mess, long p1, long p2, long p3, long p4)
{
    if ( do_comment ) {
	fprintf(as_file, "!! " ) ;
	fprintf ( as_file, mess, p1, p2, p3, p4 ) ;
	fputc('\n', as_file);
    }
    return ;
}

#endif /* DO_COMMENT */


void
fail(char * mess)
{
    if ( do_comment ) {
	fprintf(as_file, "!! TRANSLATION FAILED : " ) ;
	fprintf(as_file, "%s\n", mess ) ;
	fflush ( as_file ) ;
    }
    fprintf ( stderr, "trans : internal error '%s'\n", mess ) ;
    if ( !do_comment ) exit ( EXIT_FAILURE ) ;
    /* generate more code to give the error context when commenting */
    return ;
}


