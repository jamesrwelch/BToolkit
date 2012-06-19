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
#include "Bool_TYPE.h"

/*************** STATE *****************/

extern int Rename;

/************** OPERATIONS *************/

#define INI_Rename_Bool_File() { Rename = 0; }

#define Rename_VAL(v)     { *(v) = Rename; }

#define Rename_STO(v)     { Rename = v; }

#define Rename_SET()      { Rename = TRUE; }

#define Rename_RESET()    { Rename = FALSE; }

#define Rename_EQL(b,v)   { *(b) = ((Rename==(v))!=0); }

#define Rename_NOT()      { Rename = !(Rename); }

#define Rename_AND(v)     { Rename = (Rename && (v)); }

#define Rename_OR(v)      { Rename = (Rename || (v)); }

#define Rename_XOR(v)     { Rename = (Rename ^ (v)); }