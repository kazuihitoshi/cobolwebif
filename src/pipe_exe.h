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
typedef struct {
  int out[2];
  int in [2];
  FILE *d_out[2];
  FILE *d_in [2];
  char out_data[ 100 ];
  int  out_datalength;
  char in_data [ 100 ];
  int  in_datalength;
  int pid;
} T_PIPE_EXE_TYPE;

extern T_PIPE_EXE_TYPE * pipe_execute( char *p , char *opt );
extern int pipe_execlose( T_PIPE_EXE_TYPE *p );
extern int pipe_exesend( T_PIPE_EXE_TYPE *p, char *d , int sz );
extern void pipe_exesend_end( T_PIPE_EXE_TYPE *p );
extern int pipe_exegets( T_PIPE_EXE_TYPE *p, char *d, int sz );
extern int pipe_exeefgets( T_PIPE_EXE_TYPE *p, char **d, int *d_buf_size );
extern int pipe_exerecv( T_PIPE_EXE_TYPE *p, char *d , int sz );





