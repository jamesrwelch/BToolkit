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
#include "../MOTIF/BMotif_globals.h"

#include "motif.h"

#include "bmotif.h"

#include "appmch.motif.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/utsname.h>

#include <Xm/XmAll.h>

#include "macros.c"

#include "trmmch.c"

#include "sysmch.c"

#include "winmch.c"

#include "natmch.c"

#include "ctxmch.c"

#include "ysnmch.c"

#include "seqmch.c"

#include "strmch.c"

#include "symmch.c"

#include "setmch.c"

#include "fncmch.c"

#include "errmch.c"

#include "lexmch.c"

#include "nodmch.c"

#include "frmmch.c"

#include "codmch.c"

#include "resmch.c"

#include "ficmch.c"

#include "outmch.c"

#include "rulmch.c"

#include "themch.c"

#include "filmch.c"

#include "macmch.c"

#include "hypmch.c"

#include "golmch.c"

#include "tacmch.c"

#include "tolmch.c"

#include "shlmch.c"

#include "inimch.c"