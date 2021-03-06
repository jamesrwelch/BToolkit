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
#include "Rename_UiSSeq.h"

#include "Rename_V_UiArr.h"
#include "Rename_L_Uns.h"

#include "Bool_TYPE.h"
#include "Uns_TYPE.h"

void
INI_Rename_UiSSeq()
{
  INI_Rename_V_UiArr();
  INI_Rename_L_Uns();
  Rename_L_STO(0);
}

void
Rename_UiSSeq_LEN(ll)
int *ll;
{
  Rename_L_VAL(ll);
}

void
Rename_UiSSeq_VAL(vv,idx)
int *vv,idx;
{
  Rename_V_VAL(vv,idx);
}

void
Rename_UiSSeq_FILL(vv)
int vv;
{
  int idx;
  idx = 1;
  while( idx<=Rename_UiSSeqP2 ){
    Rename_V_STO(idx,vv);
    UINC(&idx,idx);
  };
  Rename_L_STO(Rename_UiSSeqP2);
}

void
Rename_UiSSeq_CHG(idx,vv)
int idx,vv;
{
  Rename_V_STO(idx,vv);
}

void
Rename_UiSSeq_APP(vv)
int vv;
{
  int ll;
  Rename_L_INC();
  Rename_L_VAL(&ll);
  Rename_V_STO(ll,vv);
}

void
Rename_UiSSeq_KEEP(idx)
int idx;
{
  Rename_L_STO(idx);
}

