# $TenDRA$
#
# Binary / variable definitions for the Linux operating system.

# The execution startup routines.
# crt is for normal support, crti for _init, crtn for _fini.
# gcrt is for profiling support (gprof).
# mcrt is for profiling support (prof).

CRT0?=          
CRT1?=		/usr/lib/crt1.o
CRTI?=		/usr/lib/crti.o
CRTN?=		/usr/lib/crtn.o
GCRT0?=         
GCRT1?=		/usr/lib/gcrt1.o
MCRT0?=         /usr/lib/Mcrt1.o

AR?=		/usr/bin/ar
AWK?=		/bin/awk
BASENAME?=	/bin/basename
CAT?=		/bin/cat
CHGRP?=		/bin/chgrp
CHMOD?=		/bin/chmod
CHOWN?=		/bin/chown
CP?=		/bin/cp
CP_VERBOSE?=	${CP} -v
CUT?=		/usr/bin/cut
DC?=		/usr/bin/dc
DIRNAME?=	/usr/bin/dirname
ECHO?=		/bin/echo
EGREP?=		/bin/egrep
FALSE?=		/bin/false
FILE?=		/usr/bin/file
FIND?=		/usr/bin/find
GREP?=		/bin/grep
GTAR?=		/bin/tar
GUNZIP?=	/bin/gunzip
GZCAT?=		/usr/pubsw/bin/gzcat
GZIP?=		/bin/gzip
HEAD?=		/usr/bin/head
ID?=		/usr/bin/id
LDCONFIG?=	
LN?=		/bin/ln
LS?=		/bin/ls
MKDIR?=		/bin/mkdir
MTREE?=		
MV?=		/bin/mv
PATCH?=		/usr/bin/patch
PAX?=		
PERL?=		/usr/bin/perl
RM?=		/bin/rm
RMDIR?=		/bin/rmdir
SED?=		/bin/sed
SETENV?=	/usr/bin/env
SH?=		/bin/sh
SORT?=		/bin/sort
SU?=		/bin/su
TAIL?=		/usr/bin/tail
TEST?=		/usr/bin/test
TOUCH?=		/bin/touch
TR?=		/usr/bin/tr
TRUE?=		/bin/true
TYPE?=		
WC?=		/usr/bin/wc
XARGS?=		/usr/bin/xargs
