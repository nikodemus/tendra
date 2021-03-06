/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#ifndef SPARCDDECS_INCLUDED
#define SPARCDDECS_INCLUDED

#include <diag3/dg_first.h>
#include <diag3/diaginfo.h>

#include <reader/exp.h>

#include <construct/installtypes.h>

extern void init_stab(void);
extern void init_stab_aux(void);
extern void stab_types(void);
extern long currentfile;

#endif /* SPARCDDECS_INCLUDED */
