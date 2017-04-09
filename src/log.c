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
#include <stdarg.h>
#include <ctype.h>
#include "log.h"

static char *logfilename_string = NULL;
int logfilename( char *d ){
#ifdef NOLOG
  return(0);
#else
  char *a; int a_len;
  a_len = strlen (d);

  if ( d == NULL ) return(-1);

  if ( logfilename_string != NULL ) free( logfilename_string );

  a = (char*)malloc( a_len + 1 );
  if ( a == NULL ) return (-1);
  
  memset(a,(char)NULL,a_len + 1);

  memcpy(a,d,a_len);

  logfilename_string = a;

  return(0);
#endif
}

int logprintf(char *fmt,...){
  FILE *fp;
  va_list ap;
  int ret;
  char *d;
  if ( logfilename_string == NULL ){
    ret = logfilename ( DEFAULT_LOGFILENAME );
    if ( ret ) return (ret);
  }

  va_start(ap, fmt);

  fp = fopen (logfilename_string,"a");
  if (fp == (FILE*)NULL) return(-1);

  vfprintf(fp, fmt, ap );
  d  = (char*)getenv("DEBUG");
  if ( d!=NULL){
    if (atoi(d)>0){
      vfprintf(stderr, fmt, ap );
    }
  }

  fclose(fp);

  va_end(ap);
  return(0);
}

#ifdef DEBUG_MAIN
int main( int argc, char **argv ){
  int i; int ret;
  for (i=1;i<argc;i++){
    if(!strcmp(argv[i],"-l")){
      if ( (i+1) < argc ){
        i++;
	ret = logfilename_string(argv[i]);
      }
    }
  }
}
#endif





