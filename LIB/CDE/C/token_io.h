/* Copyright (c) 1985-2012, B-Core (UK) Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "basic_io.h"

#define INI_token_io()       { ; }

#define _GET_TOK(t)           GET_NAT ( t )

#define _GET_PROMPT_TOK(t,s)  GET_PROMPT_NAT ( t, s )

#define _PUT_TOK(t)           PUT_NAT ( t )

#ifndef _NO_PROTO

#define _PUT_ACT_TOK(T,t) \
  if   ( ! strlen ( T [ 0 ] ) ) { \
    PUT_STR( T [ t ] ); \
  } \
  else { \
    if ( ( strcmp ( T [ 0 ], "SCALAR" ) != 0 )        && \
         ( strcmp ( T [ 0 ], "0..2147483646" ) != 0 )    ) { \
      PUT_STR( T [ 0 ] ); PUT_CHR ('.'); \
    } \
    PUT_NAT ( t ); \
  }

#define _GET_PROMPT_ACT_TOK(t,T,s) \
  PUT_STR ( s ); \
  _GET_ACT_TOK (t, T )

#else

extern void PUT_ACT_TOK_M ();
extern void GET_ACT_TOK_M ();

#define _PUT_ACT_TOK(T,t)            PUT_ACT_TOK_M ( T, t )

#define _GET_ACT_TOK(t,T) \
  GET_ACT_TOK_M ( T, t, "      Select token:     " );

#define _GET_PROMPT_ACT_TOK(t,T,s)   GET_ACT_TOK_M ( T, t, s )

#endif
