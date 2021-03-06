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
#include "String_TYPE.h"

#include "Bool_TYPE.h"

#include "Rename_SharedSocketServer.h"

#define Get4byteNum \
    ( ( ( unsigned char ) _in_buf [ _in_buf_ptr     ] * 16777216 ) + \
      ( ( unsigned char ) _in_buf [ _in_buf_ptr + 1 ] *    65536 ) + \
      ( ( unsigned char ) _in_buf [ _in_buf_ptr + 2 ] *      256 ) + \
      ( ( unsigned char ) _in_buf [ _in_buf_ptr + 3 ] ) )

#define Get3byteNum \
    ( ( ( unsigned char ) _in_buf [ _in_buf_ptr    ] *    65536 ) + \
      ( ( unsigned char ) _in_buf [ _in_buf_ptr + 1 ] *      256 ) + \
      ( ( unsigned char ) _in_buf [ _in_buf_ptr + 2 ] ) )

#define Get2byteNum \
    ( ( ( unsigned char ) _in_buf [ _in_buf_ptr     ] * 256 ) + \
      ( ( unsigned char ) _in_buf [ _in_buf_ptr + 1 ] ) )

#define Get1byteNum \
    ( ( unsigned char ) _in_buf [ _in_buf_ptr ] )



#define Put4byteNum(n) \
    _out_buf [ _out_buf_ptr     ] = ( unsigned char ) ( (n) / 16777216 ); \
    _out_buf [ _out_buf_ptr + 1 ] = ( unsigned char ) ( ((n) % 16777216) / 65536 ); \
    _out_buf [ _out_buf_ptr + 2 ] = ( unsigned char ) ( ((n) % 65536)    / 256 ); \
    _out_buf [ _out_buf_ptr + 3 ] = ( unsigned char ) ( (n) % 256 )

#define Put3byteNum(n) \
    _out_buf [ _out_buf_ptr     ] = ( unsigned char ) ( ((n) % 16777216) / 65536 ); \
    _out_buf [ _out_buf_ptr + 1 ] = ( unsigned char ) ( ((n) % 65536)    / 256 ); \
    _out_buf [ _out_buf_ptr + 2 ] = ( unsigned char ) ( (n) % 256 )

#define Put2byteNum(n) \
    _out_buf [ _out_buf_ptr     ] = ( unsigned char ) ( ((n) % 65536)    / 256 ); \
    _out_buf [ _out_buf_ptr + 1 ] = ( unsigned char ) ( (n) % 256 )

#define Put1byteNum(n) \
    _out_buf [ _out_buf_ptr     ] = ( unsigned char ) ( (n) % 256 )

int master_sock;
int slave_sock;

#ifdef SERVER_STATS_FLAG
struct tms          Rename_tms_buf_0,
                    Rename_tms_buf_a, Rename_tms_buf_d, Rename_tms_buf_e, Rename_tms_buf_h;
struct timeval      Rename_timeval_buf_0,
                    Rename_timeval_buf_a, Rename_timeval_buf_d, Rename_timeval_buf_e, Rename_timeval_buf_h;
long                Rename_tz = ( long ) NULL;
clock_t             Rename_clock_0,
                    Rename_clock_a, Rename_clock_d, Rename_clock_e, Rename_clock_h;
long                Rename_cycleReadWriteLenThis;
#define             Rename_statsArrMAX  50
unsigned long       Rename_statsArr [ 9 ] [ Rename_statsArrMAX + 1 ];
int                 Rename_statsArrPtr;
#define             Rename_file_name "Rename_ServerStats.txt"
FILE *              Rename_file;
#define             Rename_LargeVal	(10*1000000)
#define             Rename_AverageDim 50
#endif

#define doOutRealloc(n,s) \
 \
{ \
  char * _prev_out_buf   = _out_buf; \
  long _prev_out_buf_size = _out_buf_size; \
  _out_buf_size = (n); \
  _out_buf = ( char * ) realloc ( ( char * ) _out_buf, \
                                              ( _out_buf_size * sizeof ( char ) ) ); \
  if ( _out_buf == NULL ) { \
    printf ( "\n  %s: out_buf malloc (%d) failure\n", s, _out_buf_size ); \
    _out_buf = _prev_out_buf; \
    _out_buf_size = _prev_out_buf_size; \
    * _rep = FALSE; \
    return; \
  } \
}

#define doInRealloc(n,s) \
 \
{ \
  char * _prev_in_buf   = _in_buf; \
  long _prev_in_buf_size = _in_buf_size; \
  _in_buf_size = (n); \
  _in_buf = ( char * ) realloc ( ( char * ) _in_buf, \
                                              ( _in_buf_size * sizeof ( char ) ) ); \
  if ( _in_buf == NULL ) { \
    printf ( "\n  %s: in_buf malloc (%d) failure\n", s, _in_buf_size ); \
    _in_buf = _prev_in_buf; \
    _in_buf_size = _prev_in_buf_size; \
    * _rep = FALSE; \
    return; \
  } \
}


#ifdef SERVER_STATS_FLAG

unsigned long
_Rename_clockStatsTot ( struct tms * tmsstart, struct tms * tmsend )
{
  return ( ( tmsend->tms_utime  - tmsstart->tms_utime )  +
           ( tmsend->tms_stime  - tmsstart->tms_stime )  +
           ( tmsend->tms_cutime - tmsstart->tms_cutime ) +
           ( tmsend->tms_cstime - tmsstart->tms_cstime )   );
}

unsigned long
_Rename_GetTimeStatsTot ( struct timeval * tmsstart, struct timeval * tmsend )
{
  long sec, usec;

  sec  = tmsend->tv_sec  - tmsstart->tv_sec;
  usec = tmsend->tv_usec - tmsstart->tv_usec;

  if ( usec < 0 ) {
    usec += 1000000;
    sec--;
  }

  if ( ( sec + 1 ) > ( Rename_LargeVal / 1000000 ) )
    return ( Rename_LargeVal );
  else
    return ( ( 1000000 * sec ) + usec );
}

void
Rename_PrintStats ( int * rep, char * filename )
{
  static long    clktick = 0;

  int            i, Rename_statsArrPtr_;

  if ( clktick == 0 ) clktick = sysconf ( _SC_CLK_TCK );

  strcpy ( filename, Rename_file_name );
  Rename_file = fopen ( Rename_file_name, "w" );

  if ( Rename_file != NULL ) {

    Rename_statsArrPtr_ = Rename_statsArrPtr + 1;
    if ( Rename_statsArrPtr_ == Rename_statsArrMAX ) Rename_statsArrPtr_ = 0;

    /* header */
    fprintf ( Rename_file, "                              Database Server Statistics Per Transaction\n" );
    fprintf ( Rename_file, "                        Timings in milliseconds (imposed wait ceiling of %dms)\n\n",(Rename_LargeVal/1000));
    fprintf ( Rename_file, "|--------------------------- This cycle ---------------------------| |----- Average of last 50 cycles -----|\n" );
    fprintf ( Rename_file, "   wait        cycle     non-io as   Kb     Kb/   cycle/  cycle as %%    wait       Kb/   cycle/   cycle as %%\n" );
    fprintf ( Rename_file, "            (cpu)   tot   %% cycle           sec    sec    wait+cycle               sec    sec     wait+cycle\n" );

    for ( i = 0 ; i < Rename_statsArrMAX ; i++ ) {
      if ( Rename_statsArr [ 2 ] [ Rename_statsArrPtr_ ] +
           Rename_statsArr [ 3 ] [ Rename_statsArrPtr_ ] +
           Rename_statsArr [ 0 ] [ Rename_statsArrPtr_ ]   ) {
        /* wait */
        fprintf ( Rename_file, "%9.3f  ", ( Rename_statsArr [ 0 ] [ Rename_statsArrPtr_ ] / ( double ) 1000 ) );
        /* cycle (cpu) */
        fprintf ( Rename_file, " (%3d)", ( ( 1000 * Rename_statsArr [ 1 ] [ Rename_statsArrPtr_ ] ) / clktick ) );
        /* cycle tot */
        fprintf ( Rename_file, "%7.3f ", ( Rename_statsArr [ 2 ] [ Rename_statsArrPtr_ ] / ( double ) 1000 ) );
        /* proc as % tot */
        fprintf ( Rename_file, "%5d%% ", ( ( 100 * Rename_statsArr [ 3 ] [ Rename_statsArrPtr_ ] ) /
					                            Rename_statsArr [ 2 ] [ Rename_statsArrPtr_ ] ) );
        /* Kb */
        fprintf ( Rename_file, "%8.3f ", ( Rename_statsArr [ 4 ] [ Rename_statsArrPtr_ ] / ( double ) 1000 ) );

        /* Kb/sec */
        fprintf ( Rename_file, "%6d ", ( ( Rename_statsArr [ 4 ] [ Rename_statsArrPtr_ ] * 1000 ) /
					                            Rename_statsArr [ 2 ] [ Rename_statsArrPtr_ ] ) );
        /* cycle/sec */
        fprintf ( Rename_file, "%7d ", ( 1000000 / ( Rename_statsArr [ 2 ] [ Rename_statsArrPtr_ ] ) ) );

        /* cycle as % wait+cycle */
        fprintf ( Rename_file, "%8.1f%%", ( ( 100 * Rename_statsArr [ 2 ] [ Rename_statsArrPtr_ ] ) /
	   ( double ) ( Rename_statsArr [ 2 ] [ Rename_statsArrPtr_ ] +
                           Rename_statsArr [ 0 ] [ Rename_statsArrPtr_ ] ) ) );

	/* avge wait */
        fprintf ( Rename_file, "%13.3f  ", ( Rename_statsArr [ 5 ] [ Rename_statsArrPtr_ ] / ( double ) 1000 ) );

	/* avge Kb/sec */
        fprintf ( Rename_file, "%6d ", ( ( Rename_statsArr [ 7 ] [ Rename_statsArrPtr_ ] * 1000 ) /
					                          Rename_statsArr [ 6 ] [ Rename_statsArrPtr_ ] ) );
	/* avge cycles/sec */
        fprintf ( Rename_file, "%7d ", ( 1000000 / ( Rename_statsArr [ 6 ] [ Rename_statsArrPtr_ ] ) ) );

        /* avge cycle as % wait+cycle */
        fprintf ( Rename_file, "%8.1f%%\n", ( ( 100 * Rename_statsArr [ 6 ] [ Rename_statsArrPtr_ ] ) /
	   ( double ) ( Rename_statsArr [ 6 ] [ Rename_statsArrPtr_ ] +
                           Rename_statsArr [ 5 ] [ Rename_statsArrPtr_ ] ) ) );
      }
      Rename_statsArrPtr_++;
      if ( Rename_statsArrPtr_ == Rename_statsArrMAX ) Rename_statsArrPtr_ = 0;
    }
    fclose ( Rename_file );
    * rep = 1;
  }
  else {
    * rep = 0;
  }
}

long
_Rename_UpdateStatsArray_arrAvg ( int index, int tot )
{
  long    nn;
  int     Rename_statsArrPtr_, i;

  nn = 0;
  Rename_statsArrPtr_ = Rename_statsArrPtr;
  for ( i = 0 ; i < tot ; i++ ) {
    nn += Rename_statsArr [ index ] [ Rename_statsArrPtr_ ];
    Rename_statsArrPtr_--;
    if ( Rename_statsArrPtr_ < 0 ) Rename_statsArrPtr_ = Rename_statsArrMAX - 1;
  }
  return ( nn / tot );
}

void
_Rename_UpdateStatsArray ()
{
  int    cycleTot;
  
  if   ( Rename_statsArrPtr == 0 ) cycleTot = Rename_statsArr [ 8 ] [ Rename_statsArrMAX - 1 ];
  else                                 cycleTot = Rename_statsArr [ 8 ] [ Rename_statsArrPtr - 1 ];
  if ( cycleTot < Rename_AverageDim ) cycleTot++;

  /* wait */
  Rename_statsArr [ 0 ] [ Rename_statsArrPtr ] =
                                     _Rename_GetTimeStatsTot ( &Rename_timeval_buf_0, &Rename_timeval_buf_a );

  /* cycle (cpu) */
  Rename_statsArr [ 1 ] [ Rename_statsArrPtr ] =
                                     _Rename_clockStatsTot ( &Rename_tms_buf_a, &Rename_tms_buf_h );

  /* cycle tot */
  Rename_statsArr [ 2 ] [ Rename_statsArrPtr ] =
                                     _Rename_GetTimeStatsTot ( &Rename_timeval_buf_a, &Rename_timeval_buf_h );

  /* proc tot */
  Rename_statsArr [ 3 ] [ Rename_statsArrPtr ] =
                                     _Rename_GetTimeStatsTot ( &Rename_timeval_buf_d, &Rename_timeval_buf_e );

  /* Kb * 1000 */
  Rename_statsArr [ 4 ] [ Rename_statsArrPtr ] = Rename_cycleReadWriteLenThis;

  /* avge wait */
  Rename_statsArr [ 5 ] [ Rename_statsArrPtr ] = _Rename_UpdateStatsArray_arrAvg ( 0, cycleTot );

  /* avge cycle tot */
  Rename_statsArr [ 6 ] [ Rename_statsArrPtr ] = _Rename_UpdateStatsArray_arrAvg ( 2, cycleTot );

  /* avge Kb * 1000 */
  Rename_statsArr [ 7 ] [ Rename_statsArrPtr ] = _Rename_UpdateStatsArray_arrAvg ( 4, cycleTot );

  /* cycles */
  Rename_statsArr [ 8 ] [ Rename_statsArrPtr ] = cycleTot;

/***
{
  int i, j;
  for ( i = 0 ; i < Rename_statsArrMAX ; i++ ) {
    for ( j = 0 ; j < 9 ; j++ ) {
      printf ( "%9d ", Rename_statsArr [ j ] [ i ] );
    }
    if ( i == Rename_statsArrPtr ) printf ( "***" );
    printf ( "\n" );
  }
  printf ( "--------------------------------------------------------------------\n" );
}
***/

  Rename_statsArrPtr++;
  if ( Rename_statsArrPtr == Rename_statsArrMAX ) { Rename_statsArrPtr = 0; }
}

#endif


void
Rename_INIT ( int * _rep, void * _lockfile, int _port, void * _bufsavefile )
{
  static int          init_done = 0;
  int                 on = 1;
  struct sockaddr_in  s_server;
  int                 lf;              /* file descriptor for pid/lock file */
  char                pid_buf [ 10 ];  /* buffer for pid                    */
  pid_t               f;               /* for fork                          */

  /***
  check not already initialised
  ***/
  if ( init_done ) {
    printf ( "\n  Rename_INIT: already initialised\n" );
    * _rep = FALSE;
    return;
  }

  init_done = 1;

  /***
  create master_sock
  ***/
  if ( ( master_sock = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == -1 ) {
    printf ( "\n  Rename_INIT: socket failed - errno %d\n", errno );
    * _rep = FALSE;
    return;
  }

  /***
  setsocketopt
  ***/
  if ( ( setsockopt ( master_sock, SOL_SOCKET, SO_REUSEADDR,
		      ( char * ) &on, sizeof ( on ) ) ) == -1 ) {
    printf ( "\n  Rename_INIT: setsocketopt failed - errno %d\n", errno );
    * _rep = FALSE;
    return;
  }

  /***
  bind to port
  ***/
  memset ( ( char * ) &s_server, 0, sizeof ( s_server ) );
  s_server.sin_family = AF_INET;
  s_server.sin_addr.s_addr = htonl ( INADDR_ANY );
  s_server.sin_port = htons ( _port );
  if ( bind ( master_sock, ( struct sockaddr * ) &s_server,
                                           sizeof ( s_server ) ) == -1 ) {
    printf ( "\n  Rename_INIT: can't bind to port %d - errno %d\n", _port, errno );  
    * _rep = FALSE;
    return;
  }

  /***
  listen, with maximum backlog
  ***/
  if ( listen ( master_sock, SOMAXCONN ) == -1 ) {
    printf ( "\n  Rename_INIT: listen failed - errno %d\n", errno );
    * _rep = FALSE;
    return;
  }

  /***
  operate in background
  ***/

#ifndef TEST_FLAG

  f = fork ();
  if ( f < 0 ) {        /* error */
    printf ( "\n  Rename_INIT: fork failed - errno %d\n", errno );
    * _rep = FALSE;
    return;
  }
  else if ( f ) {       /* parent */
    exit ( 0 );
  }
                        /* child continues */
  /***
  detach from controlling tty
  ***/
/*  TIOCNOTT not in the Solaris 7 operating environment
  fd = open ( "/dev/tty", O_RDWR );
  ioctl ( fd, TIOCNOTTY, 0 );
  close ( fd );
*/
  
#else

 printf ( "\n  Rename_SharedSocketServer: TEST_FLAG set - fork/detach suppressed ...\n" );

#endif


  /***
  change directory to /
  ***/
  /*
  chdir ( "/" );
  */
  /*
  printf ( "Rename_SharedSocketServer: chdir / suppressed ...\n" );
  */

  /***
  set umask
  ***/
  umask ( 027 );
  

  /***
  set parent process group
  ***/
  setpgid ( 0, getpid () );


  /***
  acquire exclusive lock
  ***/
  lf = open ( ( char * ) _lockfile, O_RDWR | O_CREAT, 0640 );
  if ( lf < 0 ) {                              /* error */
    printf ( "\n  Rename_INIT: unable to open %s O_RDWR | O_CREAT - errno %d\n",
                                                  ( char * ) _lockfile, errno );
    * _rep = FALSE;
    return;
  }

/*
  if ( flock ( lf, LOCK_EX | LOCK_NB ) ) {     /? can't obtain exclusive lock ?/
    printf ( "\n  Rename_INIT: unable to obtain lock on %s - errno %d\n",
                                                 ( char * ) _lockfile, errno  );
    * _rep = FALSE;
    return;
  }
*/

  /***
  perform malloc for _out_buf & _in_buf
  ***/
#ifdef TEST_FLAG
    printf ( "Rename_INIT: _in_buf malloc (%d)\n", _in_buf_size );
#endif
  _in_buf = ( char * ) malloc ( _in_buf_size * sizeof ( char ) );
  if ( _in_buf == NULL ) {
    printf ( "\n  Rename_INIT: in_buf malloc (%d) failure\n", _in_buf_size );
    * _rep = FALSE;
    return;    
  }
#ifdef TEST_FLAG
    printf ( "Rename_INIT: _out_buf malloc (%d)\n", _out_buf_size );
#endif
  _out_buf = ( char * ) malloc ( _out_buf_size * sizeof ( char ) );
  if ( _out_buf == NULL ) {
    printf ( "\n  Rename_INIT: in_buf malloc (%d) failure\n", _out_buf_size );
    * _rep = FALSE;
    return;    
  }

  /***
  set _sav_buf;
  zero _num_sav, reset malloc_done
  ***/
  if ( strlen ( ( char * ) _bufsavefile ) < 256 ) {
    sprintf ( _sav_buf, ( char * ) _bufsavefile );
  }
  else {
    printf ( "\n  Rename_INIT: _bufsavefile too long (%d >= 256)\n",
                                             strlen ( ( char * ) _bufsavefile ) );
    * _rep = FALSE;
    return;    
  }
  _num_sav = 0;
  malloc_done = 0;

  /***
  write pid to file & close
  ***/
  sprintf ( pid_buf, "%d", getpid () );
  write ( lf, pid_buf, strlen ( pid_buf ) );
  close ( lf );

  /***
  report server started via syslog
  ***/
  openlog ( "Rename_SharedSocketServer", LOG_PID, LOG_DAEMON );
  setlogmask ( LOG_UPTO (  LOG_WARNING ) );
  syslog ( LOG_ERR, "Listening at port %d\n", _port );
  closelog ();

  printf ( "\n  Rename_SharedSocketServer: listening at port %d\n\n", _port );

#ifdef SERVER_STATS_FLAG
  Rename_statsArrPtr = 0;
  Rename_statsArr [ 5 ] [ 0 ] = 0; /* avge wait */
  Rename_statsArr [ 6 ] [ 0 ] = 0; /* avge cycle tot */
  Rename_statsArr [ 7 ] [ 0 ] = 0; /* avge Kb * 1000  */
  Rename_statsArr [ 8 ] [ 0 ] = 0; /* cycles */
  Rename_clock_h = times ( &Rename_tms_buf_h );
  gettimeofday ( &Rename_timeval_buf_h, ( void * ) &Rename_tz );
#endif

  * _rep = TRUE;
}

void
Rename_ACCEPT ( int * _rep )
{
  struct sockaddr  addr;      /* address for connecting enity       */
  int              addr_len;  /* amount of space pointed to by addr */

  addr_len = sizeof ( addr );

#ifdef SERVER_STATS_FLAG
  Rename_clock_0 =  times ( &Rename_tms_buf_0 );
  gettimeofday ( &Rename_timeval_buf_0, ( void * ) &Rename_tz );
#endif

  slave_sock = accept ( master_sock, ( struct sockaddr * ) &addr, &addr_len );

#ifdef SERVER_STATS_FLAG
  Rename_clock_a =  times ( &Rename_tms_buf_a );
  gettimeofday ( &Rename_timeval_buf_a, ( void * ) &Rename_tz );
#endif

  if ( slave_sock < 0 ) {
    if ( errno == EINTR ) {
      Rename_ACCEPT ( _rep ); /* redo */
      return;
    }
    printf ( "\n  Rename_ACCEPT: accept failed - errno %d\n", errno );
    * _rep = FALSE;
    return;
  } else {
    * _rep = TRUE;
  }

}

void
Rename_READ ( int * _rep, int * _pp )
{
  int read_len, tot_read_len, in_buf_len_available;
  char * _in_buf_sock_ptr;

  * _pp = 0;
  * _rep = FALSE;

  read_len = read ( slave_sock, _in_buf, 4 );

#ifdef SERVER_STATS_FLAG
  Rename_cycleReadWriteLenThis = read_len;
#endif

  if ( read_len != 4 ) {
    printf ( "\n  Rename_READ: read error 1\n  `read' returned %d not 4\n", read_len);
    return;
  }
  _in_buf_ptr = 0;
  _in_buf_len = Get4byteNum;

  /***
  perform realloc if necessary
  ***/
  if ( _in_buf_len > _in_buf_size ) {
#ifdef TEST_FLAG
  printf ( "Rename_READ: _in_buf realloc (%d -> %d)\n", _in_buf_size, _in_buf_len );
#endif
    doInRealloc ( _in_buf_len, "Rename_READ" );
  }

  /***
  read may come in many pieces
  ***/
  _in_buf_sock_ptr =_in_buf;
  in_buf_len_available = _in_buf_size;
  tot_read_len = 0;
  while ( ( tot_read_len < _in_buf_len ) && ( in_buf_len_available > 0 ) ) {
    read_len =  read ( slave_sock, _in_buf_sock_ptr, in_buf_len_available );
    if ( read_len == 0 ) break;
    if ( read_len < 0 ) {
      if ( errno != EINTR ) break;
    }
    tot_read_len += read_len;
    _in_buf_sock_ptr += read_len;
    in_buf_len_available -= read_len;
  }

  if ( tot_read_len != _in_buf_len ) {
     printf ( "\n  Rename_READ: read error 3\n  `read' returned %d not %d\n",
                                                       tot_read_len, _in_buf_len );
     return;
  }


  * _pp = tot_read_len;
  * _rep = TRUE;

#ifdef TEST_FLAG
{
  int n; printf ( "_in_buf (%d):\n    `", _in_buf_len );
  for ( n = 0 ; n < _in_buf_len ; n++ ) { printf ( "%3d ", ( unsigned char ) _in_buf [ n ] ); } printf ( "'\n" );
}
#endif

#ifdef SERVER_STATS_FLAG
  Rename_clock_d =  times ( &Rename_tms_buf_d );
  gettimeofday ( &Rename_timeval_buf_d, ( void * ) &Rename_tz );
#endif

}

void
Rename_WRITE ( int * _rep )
{
  int write_len;
  int out_buf_ptr0 = _out_buf_ptr - 4;

#ifdef SERVER_STATS_FLAG
  Rename_clock_e =  times ( &Rename_tms_buf_e );
  gettimeofday ( &Rename_timeval_buf_e, ( void * ) &Rename_tz );
#endif

  _out_buf_ptr = 0;
  Put4byteNum(out_buf_ptr0);

  write_len = write ( slave_sock, _out_buf, out_buf_ptr0 + 4 );

#ifdef SERVER_STATS_FLAG
  Rename_cycleReadWriteLenThis += write_len;
#endif

  if ( write_len != out_buf_ptr0 + 4 ) {
   printf ( "\n  Rename_WRITE :write error\n  `write' returned %d not %d\n",
                                                                    write_len, out_buf_ptr0 );
   * _rep = FALSE;
  }
  else {
   * _rep = TRUE;
  }
  _out_buf_ptr = 4;

#ifdef TEST_FLAG
{
  int n; printf ( "Rename_WRITE (%d including 1st four length bytes):\n    `", write_len );
  for ( n = 0 ; n < write_len ; n++ ) { printf ( "%3d ", ( unsigned char ) _out_buf [ n ] ); } printf ( "'\n" );
}
#endif

  /***
  check _in_buf_size/_out_buf_size are default
  ***/
  if ( _in_buf_size != Rename_SharedSocketServerP2 ) {
#ifdef TEST_FLAG
  printf ( "Rename_WRITE: _in_buf realloc (%d -> %d)\n", _in_buf_size, Rename_SharedSocketServerP2 );
#endif
    doInRealloc ( Rename_SharedSocketServerP2, "Rename_WRITE" );
  }
  if ( _out_buf_size != Rename_SharedSocketServerP3 ) {
#ifdef TEST_FLAG
  printf ( "Rename_WRITE: _out_buf realloc (%d -> %d)\n", _out_buf_size, Rename_SharedSocketServerP3 );
#endif
    doOutRealloc ( Rename_SharedSocketServerP3, "Rename_WRITE" );
  }
}

void
Rename_CLOSE ( int * _rep )
{
  if ( close ( slave_sock ) == -1 ) {
    printf ( "\n  Rename_CLOSE: close failed - errno %d\n", errno );
    * _rep = FALSE;
  }
  else {
    * _rep = TRUE;
  }

#ifdef SERVER_STATS_FLAG
  Rename_clock_h =  times ( &Rename_tms_buf_h );
  gettimeofday ( &Rename_timeval_buf_h, ( void * ) &Rename_tz );
  _Rename_UpdateStatsArray ();
#endif

}

void
Rename_GET_TOK ( int * _tok, int _toksize )
{
  if ( _in_buf_ptr + _toksize <= _in_buf_size ) {
    switch ( _toksize ) {
    case 1:
      * _tok = Get1byteNum;
      break;
    case 2:
      * _tok = Get2byteNum;
      break;
    case 3:
      * _tok = Get3byteNum;
      break;
    case 4:
      * _tok = Get4byteNum;
      break;
    }
    _in_buf_ptr += _toksize;
  }
  else {
    * _tok = 1;
  }
}

void
Rename_GET_STR ( int * _ss )
{
  char *s = ( char * ) _ss;

  if ( _in_buf_ptr + 2 <= _in_buf_size ) {
    int ss_len = Get2byteNum;
    _in_buf_ptr += 2;
    if ( _in_buf_ptr + ss_len <= _in_buf_size ) {
      int i = 0;
      while ( ss_len ) {
        if ( i < 2000 )  { s [ i++ ] = _in_buf [ _in_buf_ptr ]; }
        if ( i == 2000 ) { 
          printf ( "\n  Rename_GET_STR: truncating to 2000 characters\n" );
          s [ i++ ] = '\0';
        }
        ss_len--;
        _in_buf_ptr++;
      }
    }
    else {
      printf ( "\n  Rename_GET_STR: failed - length exceeds input buffer\n" );
      s [ 0 ] = '\0';
    }
  }
  else {
    printf ( "\n  Rename_GET_STR: failed - length exceeds input buffer\n" );
    s [ 0 ] = '\0';
  }
}


/* could even be inlined  
void
Rename_VAL_STR ( int * _ss )
{
  int ss_len = Get2byteNum;

  _in_buf_ptr += 2 ;
  _ss  = (int *) &(_in_buf [ _in_buf_ptr ]) ;
  _in_buf_ptr += ss_len;
}
*/

void
Rename_PUT_TOK ( int _tok, int _toksize )
{
  int * _rep;

  /***
  perform realloc if necessary
  ***/
  if ( _out_buf_ptr + _toksize >= _out_buf_size ) {
#ifdef TEST_FLAG
  printf ( "Rename_PUT_TOK: _out_buf realloc (%d -> %d)\n", _out_buf_size, _out_buf_ptr + _toksize + Rename_SharedSocketServerP3 );
#endif
     doOutRealloc ( _out_buf_ptr + _toksize + Rename_SharedSocketServerP3, "Rename_PUT_TOK" );
  }

  if ( _out_buf_ptr + _toksize < _out_buf_size ) {
    switch ( _toksize ) {
    case 1:
      Put1byteNum ( _tok );
      break;
    case 2:
      Put2byteNum ( _tok );
      break;
    case 3:
      Put3byteNum ( _tok );
      break;
    case 4:
      Put4byteNum ( _tok );
      break;
    }
    _out_buf_ptr += _toksize;
  }
}

void
Rename_PUT_STR ( int * _ss )
{
  char *s = (char *)_ss;
  int ss_len;
  int i = 0;
  int * _rep;

  ss_len = strlen ( s );

  /***
  perform realloc if necessary
  ***/
  if ( _out_buf_ptr + ss_len + 3 >= _out_buf_size ) {
#ifdef TEST_FLAG
  printf ( "Rename_PUT_STR: _out_buf realloc (%d -> %d)\n", _out_buf_size, _out_buf_ptr + ss_len + 3 + Rename_SharedSocketServerP3 );
#endif
     doOutRealloc ( _out_buf_ptr + ss_len + 3 + Rename_SharedSocketServerP3, "Rename_PUT_STR" );
  }

  if ( ( _out_buf_ptr + ss_len + 3 ) < _out_buf_size ) {
    Put2byteNum ( ss_len + 1 );
    _out_buf_ptr += 2;
    while ( ss_len ) {
       _out_buf [ _out_buf_ptr++ ] = s [ i++ ];
       ss_len--;
     }
    _out_buf [ _out_buf_ptr++ ] = '\0';
  }
}

void
Rename_SET_IN_PTR ( int _ptr )
{
  _in_buf_ptr = _ptr;
}

void
Rename_GET_IN_PTR ( int * _ptr )
{
  * _ptr = _in_buf_ptr;
}

void
Rename_SET_OUT_PTR ( int _ptr )
{
  _out_buf_ptr = _ptr + 4;
}

void
Rename_GET_OUT_PTR ( int * _ptr )
{
  * _ptr = _out_buf_ptr - 4;
}

void
Rename_GET_OUT_SIZE ( int * _sz )
{
  * _sz = _out_buf_size;
}

void
Rename_PUT_FILE ( int * _rep, void * _filename )
{
  struct stat stat_buf;
  FILE * filename_ptr;
  int c;
  
  * _rep = FALSE;

  if ( stat ( ( char * ) _filename, & stat_buf ) != 0 ) {
    printf ( "\n  Rename_PUT_FILE: %s does not exist\"r\" - errno %d\n",
                                               ( char * ) _filename, errno );
    return;
  }

  if ( ( filename_ptr = fopen ( ( char * ) _filename, "r" ) ) == NULL ) {
    printf ( "\n  Rename_PUT_FILE: unable to fopen %s \"r\" - errno %d\n",
                                               ( char * ) _filename, errno );
    return;
  }

  /***
  perform realloc if necessary
  ***/
  if ( _out_buf_ptr + stat_buf.st_size + 4 >= _out_buf_size ) {
#ifdef TEST_FLAG
  printf ( "Rename_PUT_FILE: _out_buf realloc (%d -> %d)\n", _out_buf_size, _out_buf_ptr + stat_buf.st_size + 4 + 100 );
#endif
     doOutRealloc ( _out_buf_ptr + stat_buf.st_size + 4 + 100, "Rename_PUT_FILE" );
  }

  Put4byteNum ( stat_buf.st_size );
  _out_buf_ptr += 4;
  c = getc ( filename_ptr );
  while ( c != EOF ) {
    _out_buf [ _out_buf_ptr++ ] = ( unsigned char ) c;
    c = getc ( filename_ptr );
  }
  if ( ( fclose ( filename_ptr ) ) ) {
    printf ( "\n  Rename_PUT_FILE: fclose failed - errno %d\n", errno );
    return;
  }
  * _rep = TRUE;
}

void
Rename_SAV_BUF ( int * _rep, int * _num_saves )
{
  int write_len;
  char len_buf [ 2 ];

  if ( ( _sav_buf_ptr = open ( _sav_buf, O_RDWR | O_APPEND | O_CREAT, 0640 ) ) == -1 ) {
    printf ( "\n  Rename_SAV_BUF: unable to open %s O_RDWR | O_APPEND | O_CREAT - errno %d\n",
                                                   _sav_buf, errno );
    * _num_saves = 0;
    * _rep = FALSE;
    return;
  }
  len_buf [ 0 ] = ( unsigned char ) ( _in_buf_len  / 256 );
  len_buf [ 1 ] = ( unsigned char ) ( _in_buf_len  % 256 );
  write_len = write ( _sav_buf_ptr, len_buf, 2 );
  if ( write_len != 2 ) {
   printf ( "\n  Rename_SAV_BUF: write error\n  `write' returned %d not %d\n",
                                                          write_len, 2 );
    * _num_saves = 0;
    * _rep = FALSE;
    return;
  }
  write_len = write ( _sav_buf_ptr, _in_buf, _in_buf_len );
  if ( write_len != _in_buf_len ) {
   printf ( "\n  Rename_SAV_BUF: write error\n  `write' returned %d not %d\n",
                                                          write_len, _in_buf_len );
    * _num_saves = 0;
    * _rep = FALSE;
    return;
  }
  if ( close ( _sav_buf_ptr ) == -1 ) {
    printf ( "\n  Rename_SAV_BUF: close failed - errno %d\n", errno );
    * _num_saves = 0;
    * _rep = FALSE;
    return;
  }
  _num_sav++;
  * _num_saves = _num_sav;
  * _rep = TRUE;
}

void
Rename_SAV_RMV ()
{
  unlink ( _sav_buf );
  _num_sav = 0;
  if ( malloc_done ) {
    free ( malloc_buf );
    malloc_done = 0;
  }
}

void
Rename_RST_BUF ( int * _rep )
{
  struct stat stat_buf;
  int read_len;
  
  malloc_buf_len = 0;
  if ( stat ( _sav_buf, & stat_buf ) != 0 ) {
    * _rep = FALSE;
    return;
  }
  malloc_buf_len = stat_buf.st_size;
  if ( ( _sav_buf_ptr = open ( _sav_buf, O_RDONLY ) ) == -1 ) {
    printf ( "\n  Rename_RST_BUF: unable to open %s O_RDONLY - errno %d\n",
                                                   _sav_buf, errno );
    * _rep = FALSE;
    return;
  }
  if ( ! ( malloc_buf = malloc ( ( unsigned char ) stat_buf.st_size ) ) ) {
    printf ( "\n  Rename_RST_BUF: unable to malloc - errno %d\n", errno );
    * _rep = FALSE;
    return;
  }
  read_len = read ( _sav_buf_ptr, malloc_buf, stat_buf.st_size );
  if ( read_len != stat_buf.st_size ) {
    printf ( "\n  Rename_RST_BUF: read error 3\n  `read' returned %d not %d\n",
                                              read_len, ( int ) stat_buf.st_size );
    free ( malloc_buf );
    malloc_done = 0;
    * _rep = FALSE;
    return;
  }
  if ( close ( _sav_buf_ptr ) == -1 ) {
    printf ( "\n  Rename_RST_BUF: close failed - errno %d\n", errno );
    free ( malloc_buf );
    malloc_done = 0;
    * _rep = FALSE;
    return;
  }
  malloc_buf_len = stat_buf.st_size;
  malloc_buf_ptr = 0;
  malloc_done = 1;
  * _rep = TRUE;

#ifdef TEST_FLAG
{ int n; printf ( "Rename_RST_BUF (%d):\n    `", malloc_buf_len );
         for ( n = 0 ; n < malloc_buf_len ; n++ ) { printf ( "%3d ", ( unsigned char ) malloc_buf [ n ] ); } printf ( "'\n    `" );
         for ( n = 0 ; n < malloc_buf_len ; n++ ) { printf ( "%3c ", malloc_buf [ n ] ); } printf ( "'\n" ); }
#endif

}

void
Rename_NXT_BUF ( int * _rep )
{
  int next_rst_len;
#ifdef TEST_FLAG
  int tot_len;
#endif

  if ( malloc_buf_ptr + 2 > malloc_buf_len ) {
    free ( malloc_buf );
    malloc_done = 0;
    malloc_buf_len = 0;
    * _rep = FALSE;
    return;
  }
  _in_buf [ 0 ] = ( unsigned char ) malloc_buf [ malloc_buf_ptr++ ];
  _in_buf [ 1 ] = ( unsigned char ) malloc_buf [ malloc_buf_ptr++ ];
  _in_buf_ptr = 0;
  next_rst_len = Get2byteNum;
  if ( malloc_buf_ptr + next_rst_len > malloc_buf_len ) {
    free ( malloc_buf );
    malloc_done = 0;
    malloc_buf_len = 0;
    * _rep = FALSE;
    return;
  }

#ifdef TEST_FLAG
  tot_len = next_rst_len ;
#endif

  _in_buf_ptr = 0;
  while ( next_rst_len ) {
    _in_buf [ _in_buf_ptr++ ] = ( unsigned char ) malloc_buf [ malloc_buf_ptr++ ];
    next_rst_len--;
  }

  _in_buf_ptr = 0;
  * _rep = TRUE;

#ifdef TEST_FLAG
{ int n; printf ( "Rename_NXT_BUF (%d):\n    `", tot_len  );
         for ( n = 0 ; n < tot_len ; n++ ) { printf ( "%3d ", ( unsigned char ) _in_buf [ n ] ); } printf ( "'\n    `" );
         for ( n = 0 ; n < tot_len ; n++ ) { printf ( "%3c ", _in_buf [ n ] ); } printf ( "'\n" ); }
#endif
}
