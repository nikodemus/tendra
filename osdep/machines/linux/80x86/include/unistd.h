/* $Id$ */

#ifndef __HACKED_UNISTD_H
#define __HACKED_UNISTD_H

#ifdef _GLIBC2_14
#ifdef __BUILDING_TDF_POSIX2_UNISTD_H
#define _POSIX2_C_VERSION 199209L
#endif
#endif

#include_next <unistd.h>

#endif
