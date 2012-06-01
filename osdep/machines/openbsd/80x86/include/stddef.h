/* $Id$ */

#ifndef __HACKED_STDDEF_H
#define __HACKED_STDDEF_H

/*
 * We're providing this primarily for offset(), because the system header
 * has the typical implementation which gives a non-constant expression.
 * Unfortunately it's not in an #ifndef guard, and so ee can't override
 * just that macro.
 */
#ifdef _OPENBSD5_1

#define NULL 0

#define _SIZE_T_DEFINED_
typedef unsigned long size_t;

typedef long ptrdiff_t;

typedef int wchar_t;

#pragma TenDRA begin
#pragma TenDRA keyword __literal for keyword literal
#pragma TenDRA conversion analysis (pointer-int) off
#define offsetof(__s, __m) (__literal (size_t) &(((__s *) 0)->__m))
#pragma TenDRA end

#endif

#endif
