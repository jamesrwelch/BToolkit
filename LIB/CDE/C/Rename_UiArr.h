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
extern int Rename_UiArrP1;
extern int Rename_UiArrP2;
extern int Rename_UiArrP3;
extern int Rename_UiArrP4;

/************* STATE *****************/

extern int Rename[];

/************* OPERATIONS ************/

#define INI_Rename_UiArr() { \
                             int i = Rename_UiArrP2; \
                             while ( i<=Rename_UiArrP3 ) { \
                               Rename[(i++)-(Rename_UiArrP2)] = Rename_UiArrP4; \
                             } \
                           }

#define Rename_IDX(b,i) \
*(b) =  (((i>=Rename_UiArrP2) && (i<=Rename_UiArrP3))!=0)

#define Rename_VAL(v,i) \
*(v) = Rename[(i)-(Rename_UiArrP2)] 

#define Rename_STO(i,v) \
Rename[(i)-(Rename_UiArrP2)] = v 


#define Rename_EQ(b,i,v) \
*(b) = ((Rename[(i)-(Rename_UiArrP2)]==(v))!=0)
