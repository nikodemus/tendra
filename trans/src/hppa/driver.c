/*
 * Copyright 2002-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stdio.h>
#include <string.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/getopt.h>
#include <shared/error.h>

#include <reader/exp.h>
#include <local/out.h>

#include <reader/main_reads.h>		/* for d_capsule */
#include <reader/externs.h>

#include <construct/exp.h>		/* for current_alloc_size */
#include <construct/installglob.h>	/* for top_def */
#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/machine.h>

#include <flpt/flpt.h>

#include <main/driver.h>
#include <main/flags.h>

#include "procrec.h"
#include "addr.h"		/* for where */
#include "make_code.h"		/* for nowhere */
#include "stabs_diag3.h"
#include "frames.h"
#include "translate.h"

#define VERSION_STR "7.8"

/*
 * XXX: Some of the conditional optimisations local to this installer are
 * miscategorised as OPTIM_PEEPHOLE (which I'm using here for general purpose
 * optimisations, rather than introduce a new category specifically for
 * hppatrans). In particular the jump table optimisations ought to be split
 * out to OPTIM_JUMPS instead.
 */

static void
init(void)
{
	endian    = ENDIAN_BIG;
	assembler = ASM_HP;
	format    = FORMAT_SOM;
	diag      = DIAG_NONE;
	cconv     = CCONV_HPPA;
	abi       = ABI_HPUX;

	redo_structfns     = false; /* procs delivering structs recast to extra param */
	redo_structparams  = true;  /* struct and union value parameters indirected   */
	do_profile         = false; /* -P option for profiling info */
	do_alloca          = false; /* inline alloca */
	PIC_code           = false;
	keep_PIC_vars      = true;
	diagnose_registers = false;

	local_prefix = "$";
	name_prefix  = "";
}

static int
option(char c, const char *optarg)
{
	UNUSED(optarg);

	switch (c) {
	case 'd': plusZ            = 1; break; /* -d emulates cc's +Z flag */
	case 'i': do_indexed_loads = 0; break;

	default:
		return -1;
	}

	return 0;
}

static void
unhas(void)
{
	/* Things trans.hppa does not "has" */
	has &= ~HAS_BYTEOPS;
	has &= ~HAS_BYTEREGS;
	has &= ~HAS_NEGSHIFT;
	has &= ~HAS_ROTATE;
	has &= ~HAS_MAXMIN;
	has &= ~HAS_DIV0;
	has &= ~HAS_SETCC;
	has &= ~HAS_COMPLEX;
	has &= ~HAS_64_BIT;

	/* not implemented */
	optim &= ~OPTIM_TAIL;
	optim &= ~OPTIM_ZEROOFFSETS;
	optim &= ~OPTIM_SUBSTPARAMS;

	/* Careful with procedure results */
	optim &= ~OPTIM_UNPAD_APPLY;

	if (writable_strings) {
		/* TODO: either always on, or always off. error out accordingly */
	}

	if (diag != DIAG_NONE) {
		/* TODO: do gdb diagnostics depend on gcc assembly? */
	}

	if (do_profile && PIC_code) {
		error(ERR_WARN, "\"-P\" and \"-D\" are mutually exclusive; \"-P\" ignored.");
		do_profile = false;
	}

	if (has & HAS_LONG_DOUBLE) {
		target_dbl_maxexp = 16384;
	} else {
		target_dbl_maxexp = 308;
	}
}

static void
cleanup(void)
{
	exit_translator();
}

struct driver driver = {
	VERSION_STR,

	init,
	unhas,
	NULL,
	NULL,
	cleanup,

	"di",
	option,
	NULL,

	local_translate_capsule,

	ABI_HPUX,
	CCONV_HPPA,
	ENDIAN_BIG | ENDIAN_LITTLE,
	FORMAT_SOM,
#ifdef _SYMTAB_INCLUDED
	DIAG_NONE | DIAG_STABS,
#else
	DIAG_NONE | DIAG_STABS | DIAG_XDB | DIAG_GDB,
#endif
	ASM_HP | ASM_GAS
};

