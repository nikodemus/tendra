# Copyright 2002-2011, The TenDRA Project.
# Copyright 1997, United Kingdom Secretary of State for Defence.
#
# See doc/copyright/ for the full copyright terms.


$LINKAGE = "C++" ;
$NAMESPACE = "std" ;
+USE ("c/c95"), "wchar.h.ts" ;

%%
wchar_t *wcschr ( wchar_t *, wchar_t ) ;
const wchar_t *wcschr ( const wchar_t *, wchar_t ) ;
wchar_t *wcsrchr ( wchar_t *, wchar_t ) ;
const wchar_t *wcsrchr ( const wchar_t *, wchar_t ) ;
wchar_t *wcspbrk ( wchar_t *, const wchar_t * ) ;
const wchar_t *wcspbrk ( const wchar_t *, const wchar_t * ) ;
wchar_t *wcsstr ( wchar_t *, const wchar_t * ) ;
const wchar_t *wcsstr ( const wchar_t *, const wchar_t * ) ;
wchar_t *wmemchr ( wchar_t *, wchar_t, size_t ) ;
const wchar_t *wmemchr ( const wchar_t *, wchar_t, size_t ) ;
%%
