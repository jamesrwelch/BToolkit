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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
/* add*/
#include <string.h>

#define MAX_loop_count 15


struct sigaction act1;
FILE * fileptr;

char *banim;
char *bkit;
char *blic;

char cur_dir [ 250 ];
char fifo_file_2m [ 50 ];
char fifo_file_2b [ 50 ];
char bkit_tool [ 500 ];
char BSes_num [ 50 ];
char tty_id [ 50 ];
char cmd [ 600 ];
int n, done, i, j, c, count, Bxok_exists;
int cant_open_bses = 0;

struct stat statBxok;

void read_Bini_load_strings ();
void re_initialise ();
void int_trap ();
void get_new_dir_from_Bcom ();
void wait_Bxok_and_get_tty_id ();
void print_warning ();
void err_exit ();
void kill_BMotif ();
void kill_XTerm ();
void kill_bplatform ();
void kill_edit ();
void chk_info_reqd ();
void rm_files ();

char Bpim_num [ 25 ];

void
read_Bini_load_strings ()
{
  int noBini = 1;
  int i, c;
  int loop_count = MAX_loop_count + 2;
  FILE * fileid;
  int seconds = 15;

  /***
  this is now part of initialisation
  BMotif waits silently for MAX_loop_count*10 secs
  for TMP/.BBcom, and removes both
  ***/
  fileid = fopen ( "TMP/.BBcom", "w" );
  if ( fileid == NULL ) {
    printf ( "\n\n             Can't open \"TMP/.BBcom\" for reading\n" );
    exit ( 1 );
  }
  fclose ( fileid );

  /***
  first wait for MAX_loop_count*seconds secs for .Bini from BMotif
  ***/
  while ( ( noBini ) && ( loop_count ) ) {
    fileptr = fopen ( ".Bini", "r" );
    if ( fileptr != NULL ) {
      i = 0;
      c = getc ( fileptr );
      while ( ( c != EOF ) && ( c != '|' ) ) {
	cur_dir [ i ] = ( char ) c;
	i++;
	c = getc ( fileptr );
      }
      cur_dir [ i ] = '\0';
      if ( c != '|' ) err_exit ( 4 );      
      i = 0;
      c = getc ( fileptr );
      while ( c != EOF && c != '\n' ) {
        Bpim_num [ i ] = ( char ) c;
        i++;
        c = getc ( fileptr );
      }
      fclose ( fileptr );
      unlink ( ".Bini" );
      Bpim_num [ i++ ] = '\0';
      noBini = 0;
    }
    else {
      if ( loop_count ) {
        if ( loop_count == MAX_loop_count + 1 )
          printf ( "\n            $BKIT/BLIB/BMotif not yet running\n" );
        else if ( loop_count < MAX_loop_count + 1 )
	  printf ( "\n               Waiting for BMotif ... (%2d)", loop_count );
        fflush ( stdout );
        sleep ( seconds );
        loop_count--;
      }
    }
  }

  if ( noBini ) {
    printf ( "\n\n\n              $BKIT/BLIB/BMotif didn't start\n\n\n" );
    err_exit ( 11 );
  }
  else if ( loop_count < MAX_loop_count + 1 ) {
    printf ( " ... ok\n" );
  }

}

#define BT_PROB_BINARY        194

void
re_initialise ( BT_PROB_BINARY_flag )
int BT_PROB_BINARY_flag;
{
  strcpy ( BSes_num, "1" );

  fileptr = fopen ( ".Bver", "w" );
  if ( fileptr == NULL ) err_exit ( 6 );
  fprintf ( fileptr, "4.9" );
  fclose ( fileptr );

  fileptr = fopen ( ".Bjob", "w" );
  if ( fileptr == NULL ) err_exit ( 7 );
  if ( BT_PROB_BINARY_flag )
    fprintf ( fileptr, "?,(0:%d:0)|?|0|?|?|?|?|?|?|?|?|?|0|?|0|?|?|?|?", BT_PROB_BINARY );
  else
    fprintf ( fileptr, "?|?|0|?|?|?|?|?|?|?|?|?|0|?|0|?|?|?|?" );
  /***
  should agree with Utils.src - uj0:
  bprintf( "%\n",( a|b|c|e|f|g|h|i|j|k|l|m|n|o|p|q|d|r|s ) ) ...
  ***/
  fclose ( fileptr );
}


void
int_trap ()
{
  err_exit ( 11 );

}

void
get_new_dir_from_Bcom ()
{
  int c;
  int i = 0;
  fileptr = fopen ( ".Bcom", "r" );
  if ( fileptr == NULL ) err_exit ( 8 );
  c = getc ( fileptr );
  while ( c != EOF && c != '\n' ) {
    cur_dir [ i ] = ( char ) c;
    i++;
    c = getc ( fileptr );
  }
  fclose ( fileptr );
  cur_dir [ i ] = '\0';
}

void
wait_Bxok_and_get_tty_id ()
{
  /***
  wait for .Bxok from BMotif - then we know .Btty complete
  ***/
  Bxok_exists = 0;
  while ( ! Bxok_exists ) {
    if   ( stat ( ".Bxok", &statBxok ) == 0 ) Bxok_exists = 1;
    else                                      sleep ( 1 );
  }

  count = 0;
  done = 0;
  while ( ! done && count < 10 ) {
    fileptr = fopen ( ".Btty", "r" );
    if ( fileptr == NULL ) {
      count++;
      sleep ( 1 );
    }
    else {
      done = 1;
   }
  }

  if ( ! done ) err_exit ( 5 );

  i = 0;
  c = getc ( fileptr );
  while ( c != EOF && c != '\n' ) {
    tty_id [ i ] = c;
    i++;
    c = getc ( fileptr );
  }
  fclose ( fileptr );
  tty_id [ i ] = '\0';
}

int
get_pog_flag ()
{
  fileptr = fopen ( "TMP/.Bpog", "r" );
  if ( fileptr == NULL ) err_exit ( 13 );
  c = getc ( fileptr );
  fclose ( fileptr );
  return ( c - '0' );
}


int
main ( argc, argv )
     int argc;
     char *argv[];
{

/***
system ( "echo \"BSession originally running in:\n  `pwd`\"" );
***/

/***
printf ( "BSession argc: %d\n", argc );
for ( i = 0 ; i < argc ; i++ ) {
  printf ( "  %d: %s\n", i, argv [ i ] );
}
***/

/*
  0: /apps/btoolkit/TOOLKIT/MASTER_RELEASE/4.0/BKIT/BLIB/BSession
  1: /home/btoolkit/TOOLKIT/BDEMO/test
*/

  act1.sa_handler = int_trap;
  sigemptyset ( &( act1.sa_mask ) );
  sigaddset ( &( act1.sa_mask ), SIGINT );
  act1.sa_flags = ( int ) 0;
  sigaction ( SIGINT, &act1, NULL );

  banim = getenv ( "BANIMATOR" );
  if ( banim == NULL ) banim = getenv ( "BKIT" );

  bkit = getenv ( "BKIT" );
  if ( bkit == NULL ) err_exit ( 1 );

  strcpy ( cur_dir, argv [ 1 ] );

  sprintf ( bkit_tool, "%s/BPLAT/bplatform %s/BLIB/Toolkit", bkit, bkit );

  read_Bini_load_strings ();

#define FIFO_FILE_MODE ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )

  sprintf ( fifo_file_2m, "/tmp/.B_%s_2m", Bpim_num );
  if ( mkfifo ( fifo_file_2m, FIFO_FILE_MODE ) != 0 ) err_exit ( 37 );

  sprintf ( fifo_file_2b, "/tmp/.B_%s_2b", Bpim_num );
  if ( mkfifo ( fifo_file_2b, FIFO_FILE_MODE ) != 0 ) err_exit ( 38 );

  unlink ( ".Berr" );

  sprintf ( cmd, "%s%s.bin %s %s 040 0 %s %s 2> .Berr",
                 bkit_tool, "1", Bpim_num, cur_dir, fifo_file_2m, fifo_file_2b );
  system ( cmd );  /* calls Toolkit1 which initiates .Bses */

  fileptr = fopen ( ".Bses", "r" );
  if ( fileptr == NULL ) err_exit ( 2 );
  i = 0;
  c = getc ( fileptr );
  while ( c != EOF && c != '\n' ) {
    BSes_num [ i++ ] = c;
    c = getc ( fileptr );
  }
  fclose ( fileptr );
  unlink ( ".Bses" );
  BSes_num [ i ] = '\0';

  /***
  BSession Loop
  ***/

  while ( ( strcmp ( BSes_num, "0" )      != 0 ) &&
          ( strcmp ( BSes_num, "999999" ) != 0 )    ) {

/***
printf (  "                              BSession - .Bses : %s\n", BSes_num  );
***/

    unlink ( ".Berr" );

    if ( strcmp ( BSes_num, "1" ) == 0 ) {
      sprintf ( cmd, "%s%s.bin %s %s 040 0 %s %s 2> .Berr", bkit_tool, BSes_num, Bpim_num, cur_dir, fifo_file_2m, fifo_file_2b );
      system ( cmd );
    }

    else if ( strcmp ( BSes_num, "14" ) == 0 ) {
      sprintf ( cmd, "%s/BLIB/CheckAnnot 1 %s", bkit, bkit );
      system ( cmd );
    }

    else if ( strcmp ( BSes_num, "15" ) == 0 ) {
      sprintf ( cmd, "%s/BLIB/CheckAnnot %s", bkit, bkit );
      system ( cmd );
    }

    else if ( strcmp ( BSes_num, "999" ) == 0 ) {  /* bplatform out of mem */
      re_initialise ( 0 );
      sprintf ( cmd, "%s%s.bin %s %s 040 0 %s %s 2> .Berr", bkit_tool, "1", Bpim_num, cur_dir, fifo_file_2m, fifo_file_2b );
      system ( cmd );
    }

    else if ( strcmp ( BSes_num, "997" ) == 0 ) {  /* bplatform received ^C */
      re_initialise ( 0 );
      sprintf ( cmd, "%s%s.bin %s %s 040 0 %s %s 2> .Berr", bkit_tool, "1", Bpim_num, cur_dir, fifo_file_2m, fifo_file_2b );
      system ( cmd );
    }

    else if ( strcmp ( BSes_num, "998" ) == 0 ) {  /* chdir to Bcom */
      get_new_dir_from_Bcom ();
      rm_files ( 0 );  /* pipe_unlink_flag=0 */
      unlink ( ".BToolkitLock" );
      if ( chdir ( cur_dir ) == -1 ) err_exit ( 9 );

/***
system ( "echo \"BSession now running in:\n  `pwd`\"" );
***/

      re_initialise ( 0 );
      fileptr = fopen ( ".Benv", "w" );
      if ( fileptr == NULL ) err_exit ( 10 );
      fprintf ( fileptr, "0$toolkit$0$0" );
      fclose ( fileptr );
      sprintf ( cmd, "%s%s.bin %s %s 040 0 %s %s 2> .Berr", bkit_tool, "1", Bpim_num, cur_dir, fifo_file_2m, fifo_file_2b );
      system ( cmd );
    }

    else if ( ( strcmp ( BSes_num, "5" )  == 0 ) ||
              ( strcmp ( BSes_num, "20" ) == 0 ) ||
              ( strcmp ( BSes_num, "21" ) == 0 )    ) {
      int pog_flag = get_pog_flag ();
      wait_Bxok_and_get_tty_id ();
      sprintf ( cmd, "%s%s.bin %s %s 040 %d %s %s | tee -a .Blog > %s 2> .Berr", bkit_tool, BSes_num, Bpim_num, cur_dir, pog_flag, fifo_file_2m, fifo_file_2b, tty_id );
      system ( cmd );
    }

    else if ( strlen ( BSes_num ) == 4 ) {  /* toolkit, no xterm */
      if ( BSes_num [ 2 ] == '0' ) {
        i = 3;
        while ( BSes_num [ i ] != '\0' ) {
          BSes_num [ i - 3 ] = BSes_num [ i ];
          i++;
        }
        BSes_num [ i - 3 ] = '\0';        
      }
      else {
        i = 2;
        while ( BSes_num [ i ] != '\0' ) {
          BSes_num [ i - 2 ] = BSes_num [ i ];
          i++;
        }
        BSes_num [ i - 2 ] = '\0';
      }
      sprintf ( cmd, "%s%s.bin %s %s 040 0 %s %s 2> .Berr", bkit_tool, BSes_num, Bpim_num, cur_dir, fifo_file_2m, fifo_file_2b );
      system ( cmd );
    }

    else { /* toolkit in xterm */
      wait_Bxok_and_get_tty_id ();
      sprintf ( cmd, "%s%s.bin %s %s 040 0 %s %s | tee -a .Blog > %s 2> .Berr", bkit_tool, BSes_num, Bpim_num, cur_dir, fifo_file_2m, fifo_file_2b, tty_id );
      system ( cmd );
    } /* end toolkit in xterm */


    fileptr = fopen ( ".Bses", "r" );
    if ( fileptr == NULL ) {
      if ( cant_open_bses ) {
        err_exit ( 3 );
      }
      else {
        cant_open_bses = 1;
        print_warning ( 1 );
        re_initialise ( 1 );
        strcpy ( BSes_num, "1" );
      }
    }
    else {
      i = 0;
      cant_open_bses = 0;
      c = getc ( fileptr );
      while ( c != EOF && c != '\n' ) {
	BSes_num [ i++ ] = c;
	c = getc ( fileptr );
      }
      fclose ( fileptr );
      system ( "rm -f .Bses" );
      BSes_num [ i ] = '\0';
    }

  }   /* BSes_num is now "0" (quit) or "999999" (SaveYourself) */

  /***
  printf ( "BSes_num=%s\n",BSes_num ); 
  ***/

  if ( strcmp ( BSes_num, "999999" ) == 0 ) {
    printf ( "\n\n       THIS IS *NOT* THE WAY TO TERMINATE THE B-TOOLKIT;\n         DATA MAY BE LOST, OR THE SYSTEM LEFT HANGING\n\n            PLEASE USE THE UTILS->EXIT PULLDOWN\n\n\n\n" );
  }
  else {
    kill_BMotif ();
  }
  kill_XTerm ();
  kill_edit ();
  kill_bplatform ();
  rm_files ( 1 );
  chk_info_reqd ();
  unlink ( ".BToolkitLock" );
  return ( 0 );

}

void
print_warning ( warn_no )
int warn_no;
{
  switch ( warn_no ) {
  case 1:
    printf ( " \n\n               Problem with toolkit binary\n" );
    printf ( "\n  Contents of .Berr (may not be relevant):\n\n" );
    system ( "cat .Berr" );
    break;
  }
}

void
err_exit ( err_no )
int err_no;
{
  switch ( err_no ) {
  case 1:
    printf ( "\n  Environment variable BKIT not set\n\n" );
    break;
  case 2:
    printf ( " \n         Execution problem ( insufficient memory? )" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 3:
    system ( "cat .Berr" );
    printf ( " \n\n               Problem running B-Toolkit" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 12:
    printf ( " \n\n                 \".Bini\" format error" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 5:
    printf ( " \n\n             Can't open \".Btty\" for reading" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 6:
    printf ( " \n\n             Can't open \".Bver\" for writing" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 7:
    printf ( " \n\n             Can't open \".Bjob\" for writing" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 8:
    printf ( " \n\n             Can't open \".Bcom\" for reading" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 9:
    printf ( " \n\n  Can't \"chdir\" to %s", cur_dir );
    perror ( "\n\n              chdir" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 10:
    printf ( " \n\n             Can't open \".Benv\" for writing" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 11:
#ifdef VERBOSE_FLAG
    printf ( "\n  BSession: gave up on .Bini\n\n" );
#endif /* VERBOSE_FLAG */
    break;
  case 13:
    printf ( " \n\n             Can't open \"TMP/.Bpog\" for reading" );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 37: printf ( "\n\n              Can't mkfifo in %s", fifo_file_2m );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  case 38: printf ( "\n\n              Can't mkfifo in %s", fifo_file_2b );
    printf ( "\n\n              B-Toolkit Session Terminated" );
    printf ( "\n\n             ( all data has been preserved )\n\n\n" );
    break;
  }
  kill_BMotif ();
  kill_XTerm ();
  kill_edit ();
  kill_bplatform ();
  rm_files ( 1 );
  chk_info_reqd ();
  unlink ( ".BToolkitLock" );
  exit ( err_no );
}

void
kill_BMotif ()
{
  struct stat stat_Bpim;
  int count = 0;
  int Bpim_not_present = stat ( ".Bpim", &stat_Bpim );
  while ( ! Bpim_not_present && count < 10 ) {
    sleep ( 1 );
    Bpim_not_present = stat ( ".Bpim", &stat_Bpim );
    count++;
  }
  if ( Bpim_not_present ) return;
  fileptr = fopen ( ".Bpim", "r" );
  if ( fileptr != NULL ) {
    i = 0;
    c = getc ( fileptr );
    while ( c != EOF && c != '\n' ) {
      i = ( i * 10 ) + c - '0';
      c = getc ( fileptr );
    }
    fclose ( fileptr );
    system ( "rm -f .Bcom ; touch .Bcom" );
    kill ( i, SIGKILL );
  }
}

/*
   LEAVE COMMENTED OUT ...
   in case ServerSocket.c needs this again ...
   void
   kill_BMotif ()
   {
   char buf  [ 50 ];
   strcpy (  buf, "/tmp/.B"  );
   fileptr = fopen (  ".Bpim", "r"  );
   if (  fileptr != NULL  ) {
   i = 0;
   j = 7;
   c = getc (  fileptr  );
   while (  c != EOF && c != '\n'  ) {
   i = (  i * 10  ) + c - '0';
   buf [ j ] = (  char  ) c;
   c = getc (  fileptr  );
   j++;
   }
   fclose (  fileptr  );
   buf [ j ] = '\0';
   system (  "rm -f .Bcom ; touch .Bcom"  );
   kill (  i, SIGKILL  );
   unlink (  buf  );
   }
   }
 */

void
kill_XTerm ()
{
  fileptr = fopen ( ".Bpix", "r" );
  if ( fileptr != NULL ) {
    i = 0;
    c = getc ( fileptr );
    while ( c != EOF && c != '\n' ) {
      i = ( i * 10 ) + c - '0';
      c = getc ( fileptr );
    }
    fclose ( fileptr );
    system ( "rm -f .Bcom ; touch .Bcom" );
    kill ( i, SIGKILL );
  }
}


void
kill_bplatform ()
{
  fileptr = fopen ( ".Bpib", "r" );
  if ( fileptr != NULL ) {
    i = 0;
    c = getc ( fileptr );
    while ( c != EOF && c != '\n' ) {
    i = ( i * 10 ) + c - '0';
      c = getc ( fileptr );
    }
    fclose ( fileptr );
    if ( c == '\n' ) kill ( i, SIGKILL );     /* read the whole thing */
  }
}

void
kill_edit ()
{
  fileptr = fopen ( ".Bed", "r" );
  if ( fileptr != NULL ) {
    i = 0;
    c = getc ( fileptr );
    while ( c != EOF && c != '\n' ) {
      i = ( i * 10 ) + c - '0';
      c = getc ( fileptr );
    }
    fclose ( fileptr );
    system ( "rm -f .Bcom ; touch .Bcom" );
    kill ( i, SIGKILL );
  }
}

void 
chk_info_reqd ()
{
  fileptr = fopen ( ".Berc", "r" );
  if ( fileptr != NULL ) {
    i = 0;
    c = getc ( fileptr );
    while ( c != EOF && c != '\n' &&
	    c >= '0' && c <= '9'  &&
	    i < 99999999             ) {
      i = ( i * 10 ) + c - '0';
      c = getc ( fileptr );
    }
    fclose ( fileptr );
    unlink ( ".Berc" );
    if ( i >= 15280 && i <= 15289 && i != 15284 )
      printf ( "           Run the script: $BKIT/BProcessInfo\n\n\n" );
  }

}

void
rm_files ( pipe_unlink_flag )
int pipe_unlink_flag;
{
  system ( " rm -f .BBBcom .BBcom .Bsid .Bver .Bmen .Benv .Berr .Bnum .Bprf .Bini btl_proof.dump" );
  system ( " rm -f .Bls .Bipr .Bses .Bopn .Bcom .Bed .Bbw .Brec .Bplx B.tmp .Bbox" );
  system ( " rm -f  BB.tmp .Bhw .Bpw tmp.b tmp.c tmpb tmpc .Bprv .Bpob* .Bbbb" );
  system ( " rm -f .Bprs .Bpdu .Bsb1 .Bsb2 .Bsb3 .Btad .Bcjt .Btlb .Bxok" );
  system ( " rm -f .Bcjm .Bdmf .Bjob .Bplf .Bpla .Bpi* .Blog .Btty .Bhlp" );
  system ( " rm -f .Bini .Bcpp .Bler .Bter SRC/.BBcom SRC/.Bter .Bsta .Bpmd .Bipl" );
  system ( " rm -f .B.bin .Blws .Bplt .Bpof .Bspo .Btac .Bthy .Btot .Bmvr .Bhtx" );
  /*  system ( " rm -f TMP/.Bffi TMP/.Bffo" ); */
  system ( " rm -fr TMP" );
  if ( pipe_unlink_flag ) { unlink ( fifo_file_2m ); unlink ( fifo_file_2b ); }
}
