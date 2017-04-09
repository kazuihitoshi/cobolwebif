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
/***********************************************************************/
/*                                                                     */
/*                                                                     */
/* スタートアップファイルリードルーチン                                */
/*                                                                     */
/*                                                                     */
/***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include "sstr.h"
#include "ini.h"

T_INI INI;

int inifileread ( char *prgn, char *opt_fullfile ){
  char     *inifile = (char*)NULL;
  char     *dir;
  char     *fullfile; int fullfile_f = 0;
  int       i,n;
  FILE     *fp;
  char      buf[8192];
  char    **sbuf; int sbuf_count;
  int       comment_f;
  char     *wp;

  INI.rootdir = NULL;
  INI.charset = NULL;
  INI.logfilename = NULL;
  INI.log_flag = 0;

  INI.rootdir = NULL;

  fullfile=(char*)NULL;
  if ( opt_fullfile != NULL ){
    if ( strlen(opt_fullfile) )fullfile = opt_fullfile;
  }
  if ( fullfile == NULL ){
    dir = NULL;
    if ( prgn != NULL ){
      n = strlen(prgn);
      if ( n > 0 ){
	for(i=n-1;i>=0;i--){
          if (prgn[i] == DIRCHAR){
            if ( prgn[0] == DIRCHAR ){
              dir = allocstr_n(prgn,i+1);
              if (dir == NULL) { fprintf(stderr,"%s(%d) allocstr_n return null\n",__FILE__,__LINE__);return(-1);}
              break;
            }else{
              dir = (char*)getcwd((char*)NULL,0);
              if (dir == NULL) { fprintf(stderr,"%s(%d) getcwd return null\n",__FILE__,__LINE__);return(-1);}
              wp = dir;
              dir = allocstrcat(dir,DIRSTR);
              if (dir == NULL) { fprintf(stderr,"%s(%d) allocstrcat_n return null\n",__FILE__,__LINE__);return(-1);}
              free(wp);
              wp = dir;
              dir = allocstrcat_n(dir,strlen(dir),prgn,i+1);
              if (dir == NULL) { fprintf(stderr,"%s(%d) allocstrcat_n return null\n",__FILE__,__LINE__);return(-1);}
              free(wp);
              break;
            }
          }
        }/* for */
      }/* if */
    }else{
      fprintf(stderr,"%s(%d) It cannot do my load module name get\n",__FILE__,__LINE__);
    }
    if ( dir == NULL ){
      dir = (char*)getcwd(NULL,0);
      if (dir == NULL) { fprintf(stderr,"%s(%d) getcwd return null\n",__FILE__,__LINE__);return(-1);}
    }
    wp = allocstrcat(dir,DIRSTR);
    if (wp ==NULL){ fprintf(stderr,"%s(%d) allocstr_cat return null\n",__FILE__,__LINE__);return(-1);}
    free(dir);dir = (char*)NULL;
    dir= wp;
    wp = allocstrcat(dir,INIFILE);
    if (wp ==NULL){ fprintf(stderr,"%s(%d) allocstr_cat return null\n",__FILE__,__LINE__);return(-1);}
    free(dir);dir = (char*)NULL;
    fullfile = wp;
  }

  fp = fopen( fullfile , "r" );
  if ( fp == (FILE*)NULL ) {
    /*fprintf(stderr,"%s(%d) config filename (%s) open error\n",__FILE__,__LINE__,fullfile);*/
    return (0);
  }
  while(1){
    memset ( buf,0, sizeof(buf));
    if ( fgets ( buf,sizeof(buf),fp ) == NULL )break;
    if ( strlen(buf) ){
     if ( buf[strlen(buf)-1]== 0x0a ) buf[strlen(buf)-1]=0;
     for(i=0;i<strlen(buf);i++){
      if(buf[i]=='#') {
	buf[i]=0;
        break;
      }
    }}
    sbuf_count = split(&sbuf,buf,NULL,NULL);
    if (sbuf_count > 0){

      wp = allocstr_upper(sbuf[0]);
      if (wp==NULL)return(-1);
      free(sbuf[0]);
      sbuf[0] = wp;

      if ( !strcmp(sbuf[0],"ROOTDIR") ){
        if (sbuf_count > 1){
         INI.rootdir=allocstr(sbuf[1]);
         if(INI.rootdir ==NULL )return(-1);
        }
      }

      if ( !strcmp(sbuf[0],"CHARSET") ){
        if (sbuf_count > 1){
         INI.charset = allocstr(sbuf[1]);
         if(INI.charset ==NULL )return(-1);
        }
      }

      if ( !strcmp(sbuf[0],"LOG_FLAG") ){
        if (sbuf_count>1)INI.log_flag = atoi(sbuf[1]);
      }
    }
    for(i=0;i<sbuf_count;i++) free(sbuf[i]);
    free(sbuf);
  }
  fclose(fp);

  if ( fullfile_f ) free(fullfile);
  return(0);
}
