/*
 * Copyright (C) 2003 Hitoshi Kazui hitoshi@pulse.homeip.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pipe_exe.h"
#include "sstr.h"


T_PIPE_EXE_TYPE * pipe_execute( char *p , char *opt )
{

  T_PIPE_EXE_TYPE *ret_pipe;

  int pret;

  logprintf("%s(%d) pipe_execute (%s,%s)\n",__FILE__,__LINE__,p,opt);

  ret_pipe = (T_PIPE_EXE_TYPE *) malloc ( sizeof( T_PIPE_EXE_TYPE ) );
  if ( ret_pipe == (T_PIPE_EXE_TYPE *)NULL )
    return( (T_PIPE_EXE_TYPE*)NULL );
  memset(ret_pipe->out_data,0,sizeof(ret_pipe->out_data));
  memset(ret_pipe->in_data, 0,sizeof(ret_pipe->in_data ));
  ret_pipe->out_datalength = 0;
  ret_pipe->in_datalength  = 0;

  pipe( ret_pipe->out );
  pipe( ret_pipe->in  );

  pret = vfork();

  switch( pret ){
  case -1:
    free ( ret_pipe );
    return( (T_PIPE_EXE_TYPE*)NULL );
  case  0:
    close(0);
    if ( dup( ret_pipe->out[0] ) < 0 ){
      free ( ret_pipe );
      return( (T_PIPE_EXE_TYPE*)NULL );
    }
    close(1);
    if ( dup( ret_pipe->in[1]  ) < 0 ){
      free ( ret_pipe );
      return( (T_PIPE_EXE_TYPE*)NULL );
    }
    close( ret_pipe->out[0] );
    close( ret_pipe->out[1] );
    close( ret_pipe->in [0] );
    close( ret_pipe->in [1] );
    execlp( p, "scg(subthered)", NULL );
    exit(-1);
  default:
    close(ret_pipe->out[0] );
    close(ret_pipe->in [1] );
    ret_pipe->pid = pret;
    ret_pipe->d_out[1] = fdopen( ret_pipe->out[1], "w" );
    if ( ret_pipe->d_out[1] == (FILE*)NULL )  return( (T_PIPE_EXE_TYPE*)NULL );
    ret_pipe->d_in [0] = fdopen( ret_pipe->in [0], "r" );
    if ( ret_pipe->d_in[0] == (FILE*)NULL )   return( (T_PIPE_EXE_TYPE*)NULL );
  }
  return( ret_pipe );
}

int pipe_execlose( T_PIPE_EXE_TYPE *p )
{
  int ret;
  if( p == (T_PIPE_EXE_TYPE*)NULL ){
     return(-1);
  }
  wait( &ret );
  close(p->out[0]);
  close(p->out[1]);
  close(p->in[0]);
  close(p->in[1]);
  free( p );
  return( ret );
}

int pipe_exesend( T_PIPE_EXE_TYPE *p, char *d , int sz ){
  int io_length;
  int io_ret; int i;
  int ret; int write_size;
  if ( sz <= 0 )return(-1);
  io_length = sz;
  write_size = 0;
  ret = write( p->out[1],d,io_length );

#ifdef  GG                                          /* 本来は以下で行うべきとおもわれる */

    ret = fwrite( d + write_size, 1,io_length, p->d_out[1]);
    logprintf("ret = %d fwrite (%s) %d\n",ret,d,io_length);
  while ( 1 ) {
    ret = fwrite( d + write_size, 1,io_length, p->d_out[1]);
    if ( ret <= 0 )break;
    write_size += ret;
    if (write_size >= sz)break;
  }

  if (ret < 0 ) return(ret);
#endif
  return(0);
}

void pipe_exesend_end( T_PIPE_EXE_TYPE *p ){
  close(p->out[1]);
}

/* 
   It changes into the same specification as fgets.    Even a return code is read.  

   fgetsと同じ様な仕様に変更　　リターンコードまでを読み込む　
*/
int pipe_exegets( T_PIPE_EXE_TYPE *p, char *d, int sz ){
  char *str_ret;
  if ( d == (char*)NULL )             return(-1);
  if ( p == (T_PIPE_EXE_TYPE *)NULL ) return(-1);
  if ( sz<= 0 )                       return(-1);

  memset(d,0,sz);
  str_ret = fgets (  d , sz , p->d_in[0] );
  if ( str_ret == (char*)NULL ) return (-1);
  return(strlen(d));

}

int pipe_exeefgets( T_PIPE_EXE_TYPE *p, char **d, int *d_buf_size ){
  if ( d_buf_size == NULL )return(-1);
  if ( p == NULL ) return(-1);
  logprintf("%s(%d) pipe_exeefgets\n",__FILE__,__LINE__);
  return( efgets ( d, d_buf_size, p->d_in[0] ) );
}

int pipe_exerecv( T_PIPE_EXE_TYPE *p, char *d , int sz ){
  int io_length;
  int io_ret;

  if ( sz <= 0 )return(-1);
  io_length = sz;
  return( read(p->in[0],d,io_length ) );
}

#ifdef DEBUG_MAIN
int main( ){
  T_PIPE_EXE_TYPE *p;
  FILE *fp;
  char buf[8192];
  fp = fopen("data.dat","r");
  if ( fp == (FILE*)NULL )return(-1);
  p = pipe_execute( "sort", "" );
  while(1){
    memset(buf,0,sizeof(buf));
    if( fgets(buf,sizeof(buf),fp ) == 0 ){
      break;
    }
    pipe_exesend( p, buf, strlen(buf) );
  }
  pipe_exesend_end( p );
  printf("=----------------------------------------------\n");
  while(1){
    memset(buf,0,sizeof(buf));
    if ( pipe_exerecv( p, buf, sizeof(buf) ) <= 0 ){
      break;
    }
    fwrite(buf,strlen(buf),1,stdout);
  }
  printf("/----------------------------------------------\n");
  printf( "pipe_execlose=%d\n",pipe_execlose( p ));
  return(0);
}
#endif
