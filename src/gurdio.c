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
/****************************************************************************


    sc.conf の ROOTDIR 設定解釈付きファイルIO処理


 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sstr.h"
#include "ini.h"

extern T_INI INI;

char * gfilename ( char *filename ){
  FILE *fp;
  char *fn = NULL;
  char *fnw= NULL;
  int   i;
  if ( filename == NULL ) return( NULL );

  if ( INI.rootdir != NULL ){
    i = strlen(filename) + 2 + strlen(INI.rootdir) ;
    fn = (char*)malloc( i );
    if ( fn == NULL )return ( NULL );
    memset ( fn , 0, i );
    strcpy ( fn, INI.rootdir );
    if ( strlen (fn) > 0 ) {
      if ( fn[strlen(fn)-1] != '/' )
        if ( strlen( filename ) > 0 ){
          if ( filename[0] != '/' ) strcat(fn,"/");
        }
    }

    fnw = (char*)malloc(strlen(filename)+1);
    if ( fnw == (char*)NULL ) return( (char*)NULL );
    memset( fnw , 0, strlen(filename)+1 );
    strcpy( fnw , filename );
    if ( strlen(filename) >= 3 ){
     for (i=0;i<strlen(filename);i++ ){
       if (!strncmp(fnw+i,"../",3)){
         memcpy(fnw+i,fnw+i+3,strlen(fnw+i+3));
         fnw[strlen(fnw)-3]=0;
         i--;
       }
     }
    }
    strcat(fn,fnw);
    free(fnw);
  }else{
    fn = filename;
  }
  return( fn );
}

FILE * gfopen ( char *filename, char *mode ){
  FILE *fp;
  char *fn = (char*)NULL;
  char *fnw= (char*)NULL;
  int   i;
  if ( filename == (char*)NULL ) return( (FILE*)NULL );
  if ( mode     == (char*)NULL ) return( (FILE*)NULL );

  fn = gfilename( filename );
  
  fp = fopen ( fn , mode );

  if ( fn != filename ) free(fn);
  return( (FILE*)fp );
}

#ifdef DEBUG_MAIN 
int main( int argc, char **argv ){
  FILE *fp;
  char  *filename="/ggggg.txt";

  inifileread ( argv[0], (char*)NULL );

  if (argc > 1){
    filename = argv[1];
  }

  printf("gfilename[%s]\n",gfilename(argv[1]));

  printf("INI.charset<%s>\n",INI.charset);

  fp = gfopen( filename, (char*)"r" );
}
#endif







