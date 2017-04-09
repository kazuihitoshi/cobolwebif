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
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include "sstr.h"
#include "gurdio.h"

#define SJIS1(a)   ((a >=129 && a <= 159)||(a>=224 && a <= 239))
#define SJIS2(a)   ( a >= 64 && a <= 252 )
#define ISEUC(a)   ((a >=161 && a <= 254 ))
#define HANKATA(a) (a >= 161 && a <= 223)

int charset_flag = CHARSET_EUC_JP;

int strsrch_n( char *s,int s_len, char *d, int d_len){
  int i;
  for(i=0;i<(s_len-d_len+1);i++){
    if ( !strncmp(s+i,d,d_len) ){
      return(i);
    }
  }
  return(-1);
}

int strsrch( char *s, char *d ){
  return(strsrch_n(s,strlen(s),d,strlen(d)));
}

char *Malloc( int len ){
  int reallen = len + (MEMSTEPSIZE - (len % MEMSTEPSIZE));
  
  return( (char*)malloc( reallen ) );
}

/**********************************************************************/
/* improved type strtok                                               */
/*                                                                    */
/*  ret = estrtok ( src, fs, ns, next )                               */
/*                                                                    */
/*   src  .. 文字列を指定                                             */
/*   fs   .. フィールドセパレータ(省略時 SPACEとTAB)                  */
/*   ns   .. 省略時「"」および「'」                                   */
/*   next .. 次のフィールドのアドレス格納領域                         */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
char *estrtok_n (
      char *src,        int src_len, 
      const char *a_fs, int a_fs_len,
      const char *a_ns, int a_ns_len,
      char **next 
){
  int len;
  int i,n;
  int found;
  char *start;
  char *ret;
  int ns_flag;
  char *fs = " \t";
  char *ns = "\"\'";
  int fs_len = 2;
  int ns_len = 2;

  if (src == NULL )return(NULL);
  if (a_fs != NULL ) {
     fs = (char*)a_fs;
     fs_len = a_fs_len;
  }
  if (a_ns != NULL ) {
     ns = (char*)a_ns;
     ns_len = a_ns_len;
  }

  if (*next==NULL) *next = src;
  
  /* 区切り文字以外を探す */
  found = 0;
  start = NULL;
  for(i=0;i<src_len;i++){
    for (n=0;n<fs_len;n++){
      if ( src[i] != fs[n] ) found ++;
      else break;
    }
    if (found == fs_len ){
      start = &src[i];
      break;
    }
  }
  if ( start == NULL ) return NULL;

  src_len -= i;

  if (src_len<=0) return NULL;

  ns_flag = -1;
  for (i=0;i<ns_len;i++){
    if( start[0] == ns[i] ) {
      start ++;
      ns_flag = i;
      break;
    }
  }

  /* 区切り文字を探す */
  found = 0;
  for(i=0;i<src_len;i++){
    for (n=0;n<fs_len;n++){
      if (ns_flag != -1){
        if (start[i] == ns[ns_flag] ){
	  found = 1;
          break;
        }
      }else{
        if ( start[i] == fs[n] ){
          found = 1;
          break;
        }
      }
    }
    if ( found ) break;
  }
  ret = (char*)malloc( (sizeof(char)*(i+1))  );
  if (ret == NULL)return(NULL);

  memset(ret,0,(sizeof(char)*(i+1)) );
  if( i>0 ) memcpy(ret,start,i);

  if ( (src + src_len) <= (start+i) ){
    *next = NULL;
  }else{
    *next = (start + i + 1);
  }
  return(ret);
}

/**********************************************************************/
/* improved type strtok                                               */
/*                                                                    */
/*  ret = estrtok ( src, fs, ns, next )                               */
/*                                                                    */
/*   src  .. 文字列を指定                                             */
/*   fs   .. フィールドセパレータ(省略時 SPACEとTAB)                  */
/*   ns   .. 省略時「"」および「'」                                   */
/*   next .. 次のフィールドのアドレス格納領域                         */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
char *estrtok (char *src, const char *fs, const char *ns, char **next ){
  int src_len, fs_len, ns_len;

  if ( src == NULL )return(NULL);
  src_len = strlen(src);

  if ( fs == NULL ) fs_len = 0;
  else              fs_len = strlen(fs);

  if ( ns == NULL ) ns_len = 0;
  else              ns_len = strlen(ns);

  return(estrtok_n(src,src_len,fs,fs_len,ns,ns_len,next));

}

int querystringtoarray( char ***des, int ***des_n, char *src, const char *fs, const *ns ){
  char **d_des; char **pp_des;
  char *d,*p,*next;
  int  des_count = 0;
  if ( src == NULL ) return(-1);
  d_des = NULL;

  p = src;
  next = NULL;
  while (1){
    d = estrtok(p,fs,(const char*)ns,&next);
    if ( d == NULL )break;

    pp_des = d_des;
    d_des = (char**)Malloc( sizeof(char**)*(des_count+1)  );
    if ( d_des == NULL )return(-1);
    memset(d_des,0,(sizeof(char**)*(des_count+1)));
    
    if ( pp_des != NULL )
      memcpy(d_des,pp_des,(sizeof(char**)*(des_count+1))  );
    d_des[des_count] = d;
    des_count++;

    p = next;
  }

  *des = d_des;
}

int split( char ***des, char *src, const char *fs, const char *ns ){
  char **d_des; char **pp_des;
  char *d,*p,*next;
  int  des_count = 0;
  if ( src == NULL ) return(-1);
  d_des = NULL;

  p = src;
  next = NULL;
  while (1){
    d = estrtok(p,fs,ns,&next);
    if ( d == NULL )break;

    pp_des = d_des;
    d_des = (char**)Malloc( sizeof(char**)*(des_count+1)  );
    if ( d_des == NULL )return(-1);
    memset(d_des,0,(sizeof(char**)*(des_count+1)));
    
    if ( pp_des != NULL )
      memcpy(d_des,pp_des,(sizeof(char**)*(des_count+1))  );
    d_des[des_count] = d;
    des_count++;

    p = next;
  }

  *des = d_des;
  return(des_count);
}
/*******************************************************************/

/*int strnlen( char *d, int len ){
  int i;
  if (d == NULL ) return(-1);
  if ( len <= 0 ) return(0);
  for(i=0;i<len;i++){
    if(d[i]==0)return(i);
  }
  return(0);
}*/

/*******************************************************************/
char *allocstr_n( char *s , int len){
  char *ret; 
  if (s == NULL )return(NULL);
  ret = (char*)Malloc( len + 1 );
  if (ret == NULL )return(NULL);
  memset(ret,0,len+1);
  memcpy(ret,s,len);
  return(ret);
}

char *allocstr ( char *s ){
  if (s == NULL)return(NULL);
  return( allocstr_n(s,strlen(s)));
}

char *allocstr_upper_n( char *d , int len ){
  char *ret; int i;

  if ( d == NULL ) return(NULL);

  ret = (char*) Malloc( len + 1 );
  if (ret == NULL) return(NULL);

  memset(ret,0,len+1);
  if (len == 0 ) return(ret);

  for (i=0;i<len;i++)
    ret[i] = toupper(d[i]);

  return(ret);
}

char *allocstr_lower_n( char *d , int len ){
  char *ret; int i;

  if ( d == NULL ) return(NULL);

  ret = (char*)Malloc( len + 1 );
  if ( ret == NULL ) return(NULL);

  memset(ret,0,len+1);

  for (i=0;i<len;i++)
    ret[i] = tolower(d[i]);

  return(ret);
}

char *allocstr_upper( char *d ){
  return( allocstr_upper_n(d,strlen(d)) );
}

char *allocstr_lower( char *d ){
  return( allocstr_lower_n(d,strlen(d)) );
}

char *allocstrcat_n( char *s1, int s1_len, char *s2, int s2_len ){
  char *ret;
  int ret_len;
  if (s1 == NULL) return(NULL);
  if (s2 == NULL) return(NULL);
  if ( s1_len<0 ) return(NULL);
  if ( s2_len<0 ) return(NULL);
  ret_len = s1_len + s2_len + 1;
  ret = (char*)Malloc( ret_len );
  if(ret == NULL) return(NULL);
  ret[ret_len-1] = 0;
  if(s1_len) memcpy(ret,s1,s1_len);
  if(s2_len) memcpy(ret+s1_len,s2,s2_len);
  return(ret);
}

char *allocstrcat( char *s1, char *s2 ){
  if (s1==NULL)return(NULL);
  if (s2==NULL)return(NULL);
  return(allocstrcat_n(s1,strlen(s1),s2,strlen(s2)));
}

int efread( char *d, int size , FILE *fp ){
  int  i;
  int  idx;
  int  getsize;
  int  totalsize;
  i   = 0;
  idx = 0;
  totalsize = 0;
  while (1){
    getsize = fread(d+idx,1,size-totalsize,fp);
    if (getsize<=0)return(-1);
    totalsize += getsize;
#ifdef DEBUG
    logprintf("%s(%d) fread getsize=%d totalsize=%d\n",__FILE__,__LINE__,getsize,totalsize);
#endif
    if (totalsize >= size)break;
    idx+=getsize;
  }
  return(0);
}

/****************************************************************************

      d にはefgetsでmallocされる。初回はNULLをセットしたポインタのアドレスを
　　　指定する。
      d_size には実際に読み込まれたバイト数ではなくバッファサイズが入る   
****************************************************************************/
int  efgets( char **d, int *d_buf_size, FILE *fp ){

  char *p;
  char *np;
  int strleni;
  int eof_f;
  if ( d_buf_size == NULL )return(-2);
  if ( fp == NULL ) return(-2);

  if ( *d_buf_size == 0 ) {
    *d_buf_size = efgets_buffer_step_size;
    *d=(char*)malloc( *d_buf_size );
    memset(*d,0,*d_buf_size);
  }

  eof_f = 0;
  if ( *d == NULL ) return(-2);
  p=*d;
  memset(p,0,efgets_buffer_step_size);
  while (1){
    if ( fgets ( p, efgets_buffer_step_size , fp ) == NULL )eof_f=1;
    strleni=strlen(p);
    if(strleni==0&&eof_f)break;
    /*printf("strleni=%d eof_f=%d\n",strleni,eof_f);*/
    if ( strleni ){
      if ( p[strleni-1] == 0x0a )break;
    }
    *d_buf_size += efgets_buffer_step_size;
    np = (char*)realloc( *d, *d_buf_size );
    if (np==NULL){
      logprintf("%s(%d) error malloc error exist\n",__FILE__,__LINE__);
      return(-2);
    }
    p = np;
    p += strlen(p);
    *d = np;
    if(eof_f)break;
  }
  if ( *d == NULL ) return(-2);
  if ( eof_f ) return(-1);

  return( strlen(*d) );
}
/**********************************************************************************


 **********************************************************************************/
char * cgiunperse_n ( char *d , int len ){
  int   f,i;
  char  bb[3], b[10];
  char  *o;
  int   o_ind;

  o_ind=0;

  if ( d == NULL )return(NULL);

  if ( len < 0 ) return(NULL);

  o=(char*)Malloc( len + 1 );
  if ( o == (char*)NULL ) return( NULL );
  memset((char*)o,0,len + 1);

  if ( len == 0 ) return(o);

  f = 0;
  for(i=0;i<len;i++){
    if ( f ){
      bb[2 - f] = (unsigned char)tolower((unsigned char)d[i] );

      f--;
      if(f==0){
	memset(b,0,sizeof(b));
	strcpy(b,"0x");
	strncat(b,bb,2);
	o[o_ind]=(unsigned char)strtol( b, (char**)NULL,16);
	o_ind++;
	continue;
      }
    }
    if(d[i]=='%') f=2;
    if ( f == 0 ) {
      if ( d[i] == '+' ) {
	o[o_ind]=(char)' ';
        o_ind ++;
      }else{
	o[o_ind]=(char)d[i];
	o_ind ++;
      }
    }
  }
  return( o );
}

int codecount_n( char *d , int len ,char c ){
  int ret; int i;
  if (d == NULL)return(-1);
  if (len <= 0 )return(-1);
  for(i=0;i<len;i++){
    if ( d[i] != (char)c ) break;
  }
  ret = i;
  return(ret);
}

int codecount( char *d ,char c ){
  return(codecount_n(d,strlen(d),c));
}

char * cgiunperse ( char *d ){
  return(cgiunperse_n(d,strlen(d)));
}

char * trim_n( char * d , int len ){
  char *ret; int i; int n; int start; int cp_len;

  if ( d == NULL )return(NULL);

  if ( len < 0 ) return(NULL);

  if (len == 0 ) {
    ret = (char*)Malloc(1);
    if ( ret == NULL ) return(NULL);
    ret [0] = 0;
    return(ret);
  }

  n = 0;
  for(i=0;i<len;i++){
    if ( d[i] != ' ' && d[i] != '\t' )break;
  }
  start = 0;
  if (i<len)start = i;
  if (i == len ) {
    ret = (char*)Malloc(len+1);
    if (ret == NULL)return(NULL);
    memset(ret,0,len+1);
    if ( len ) memcpy(ret,d,len);
    return (ret);
  }

  cp_len = len;
  for(i=len-1;i>=0;i--){
    if ( d[i] != ' ' && d[i] != '\t' ){
      cp_len = len - start - (len - i) + 1;
      break;
    }
  }
 
  if ( cp_len ){
    ret = (char*)Malloc(cp_len+1);
    if(ret==NULL)return(NULL);
    memset(ret,0,cp_len+1);
    if(cp_len)memcpy(ret,d+start,cp_len);
  }else {
    { char *ret = (char*)Malloc(1);
    if (ret == NULL)return(NULL);
    ret[0]=0;
    return(ret);
    }
  }

  return(ret);
}

char * trim( char *d ){
  if ( d == NULL )return(NULL);
  return(trim_n(d,strlen(d)));
}

char * rtrim_n(char *d, int len){
  char *ret;
  int i;
  if ( d == NULL )return(NULL);

  if ( len <= 0 ) {
    ret=allocstr("");
    return(ret);
  }

  for(i=len-1;i>0;i--){
    if(d[i]!=' ' && d[i]!='\t')break;
  }
#ifdef DEBUG
  printf("%s(%d) i = %d\n",__FILE__,__LINE__,i);
#endif
  if ( i == 0 && ( d[i] == ' ' || d[i] == '\t')){
    ret=allocstr("");
    return(ret);
  }
  if (i<=0) i=(len - 1);
  ret = allocstr_n(d,i+1);
  return(ret);
}

char *rtrim(char *d){
  return(rtrim_n(d,strlen(d)));
}

char * cgiparse_n ( char *d, int len ){
  char *codeset = "*-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
  int i; int n; char *upper_str;
  int n_len; int found;
  char *ret_string = NULL;
  char encoding[5];
  if ( d == NULL ){ logprintf("%s(%d) error d is null\n",__FILE__,__LINE__);return(NULL);}
  if ( len <  0 ) { logprintf("%s(%d) error len <= 0\n",__FILE__,__LINE__);return(NULL);}

  for(i=len-1;i>=0;i--){
    if ( d[i] != ' ' )break;
  }
  len = i + 1;

  ret_string = (char*)Malloc(len * 3 + 1);
  if ( ret_string == NULL ){ logprintf("%s(%d) error ret_string is null\n",__FILE__,__LINE__);return(NULL);}
  memset(ret_string,0,len * 3 + 1);
  n_len = strlen(codeset);
  for(i=0;i<len;i++){
    found = 0;
    for(n=0;n<n_len;n++){
      if(d[i]==' '){
        strcat(ret_string,"+");
        found = 1;
        break;
      }
      if(d[i]==codeset[n]){
        strncat(ret_string,d+i,1);
        found = 1;
        break;
      }
    }
    if (found == 0){
      memset(encoding,0,sizeof(encoding));
      sprintf(encoding,"%%%02x",(unsigned char)d[i]);
      upper_str = allocstr_upper ( encoding );
      if (upper_str==NULL)return(NULL);
      strcat(ret_string,upper_str);
      if (upper_str!=NULL)free(upper_str);
    }
  }
  
  return(ret_string);
}

char * cgiparse ( char *d ){
  return(cgiparse_n(d,strlen(d)));
}

T_DATA_LIST *datalist_create( int hash_count ){
  T_DATA_LIST *ret;
  int hc;
  hc = hash_count;
  if ( hc == 0 ) hc = 101;
  ret = (T_DATA_LIST*)Malloc( sizeof(T_DATA_LIST) * hc );
  if ( ret == NULL )return(NULL);
  memset((char*)ret,0,sizeof(T_DATA_LIST)* hc);
  ret->hashcount=hc;
  return(ret);
}

void datalist_free( T_DATA_LIST *d ){  /* 修正の必要あり */
  T_DATA_LIST *p, *pp; int i;
  if (d == NULL)return;
  for(i=0;i<d->hashcount;i++){
    p = d+i;
    if ( p != NULL ){
    while(1){
      if (p == NULL)break;
      if (p->name == NULL) break;
      if (p->name != NULL) free(p->name);
      p->name = NULL;
      if (p->d != NULL ) free(p->d);
      p->d = NULL;
      pp = p;
      p = p->next;
      /*free(pp); */
    }}
  }
  free(d);
}

int hash_n( T_DATA_LIST *lst,char *s, int s_len ){
   unsigned v;
   int i;
   for(v=0,i=0;i<s_len;s++,i++)
     v = ((v << CHAR_BIT) + *s) % lst->hashcount;
   return (int)v;  
}

int hash( T_DATA_LIST *lst, char *s){
  return( hash_n(lst,s,strlen(s)) );
}

int datalist_set_noalloc( T_DATA_LIST *alst, char *name, int name_len, char *d, int d_len ){
  T_DATA_LIST *lst_st;
  T_DATA_LIST *lst_n;
  T_DATA_LIST *lst_p;
  int ret;
  T_DATA_LIST *lst;

  if (name==NULL   )return(-1);
  if (d   ==NULL   )return(-1);
  if (alst == NULL )return(-1);

  lst = &alst[ hash_n(alst,name,name_len) ];

  if (lst->name == NULL ){
    lst->name = name;
    lst->name_len = name_len;
    lst->d = d;
    lst->d_len = d_len;
    lst->next = NULL;
    return(0);
  }
  lst_st = lst;
  lst_n = lst;
  lst_p = lst;
  while(1){
    if (lst_n==NULL){
      lst_n = malloc( sizeof(T_DATA_LIST) );
      if (lst_n==NULL)return(-1);
      memset((char*)lst_n,0,sizeof(T_DATA_LIST));
      lst_n->hashcount = lst_p->hashcount;
      lst_n->name  = name;
      lst_n->name_len = name_len;
      lst_n->d     = d;
      lst_n->d_len = d_len;
      lst_n->flag  = 0;
      break;
    }
    if ( !strncmp(lst_n->name,name,name_len) ){
      /*free(lst_n->name);
	free(lst_n->d);*/
      lst_n->name  = name;
      lst_n->name_len = name_len;
      lst_n->d     = d;
      lst_n->d_len = d_len;
      lst_n->flag  = 0;
      break;
    }
    lst_p = lst_n;
    lst_n = lst_n->next;
  }
  return(0);
}

int datalist_set( T_DATA_LIST *alst, char *name, char *d ){
  return( datalist_set_n(alst,name,strlen(name),d,strlen(d)) );
}

int datalist_set_n( T_DATA_LIST *alst, char *name, int name_len, char *d , int d_len ){
  T_DATA_LIST *lst_st;
  T_DATA_LIST *lst_n;
  T_DATA_LIST *lst_p;
  int ret;
  T_DATA_LIST *lst;
  int start_f;

  if (name==NULL  )return(-1);
  if (d   ==NULL  )return(-1);
  if (alst == NULL )return(-1);
  lst = &alst[ hash_n(alst,name,name_len) ];

  if (lst->name == NULL ){
    lst->name = allocstr_n(name,name_len);
    lst->d = allocstr_n(d,d_len);
    lst->next = NULL;
    return(0);
  }
  start_f = 1;
  lst_st= lst;
  lst_n = lst;
  lst_p = lst;
  while(1){
    if (lst_n==NULL){
      lst_n = malloc( sizeof(T_DATA_LIST) );
      if (lst_n==NULL)return(-1);
      memset((char*)lst_n,0,sizeof(T_DATA_LIST));
      lst_n->hashcount = lst_p->hashcount;
      lst_p->next=lst_n;
      lst_n->name = allocstr_n(name,name_len);
      lst_n->d = allocstr_n(d,d_len);
      lst_n->flag = 0;
      break;
    }
  
    if ( !strncmp(lst_n->name,name,name_len) ){
      free(lst_n->name);
      free(lst_n->d);
      lst_n->name = allocstr_n(name,name_len);
      lst_n->d = allocstr_n(d,d_len);
      lst_n->flag = 0;
      break;
    }

    lst_p = lst_n;
    lst_n = lst_n->next;
    start_f = 0;
  }
  return(0);
}

int datalist_set_flag( T_DATA_LIST *alst, char *name, int flag ){
  T_DATA_LIST *lst_n;
  int ret;
  T_DATA_LIST *lst;

  if (name==NULL  )return(-1);
  if (alst == NULL )return(-1);

  lst = &alst[ hash(alst,name) ];

  lst_n = lst;
  while(1){
    if (lst_n==NULL)break;
    if (lst_n->name != NULL ){
      if(!strcmp(lst_n->name,name)){
        lst_n->flag = flag;
        break;
      }
    }
    lst_n = lst_n->next;
  }
  return(0);
}

int datalist_del( T_DATA_LIST *alst, char *name ){
  T_DATA_LIST *lst_p;
  T_DATA_LIST *lst_n; int i;
  T_DATA_LIST *lst;
  if (alst== NULL)return(-1);
  if (name==NULL)return(-1);
  lst = &alst[ hash(alst,name) ];
  if (lst->name !=NULL){ 
    if(!strcmp(lst->name,name)){
      lst_n = lst->next;
      if (lst->name!=NULL)free(lst->name);
      if (lst->d   !=NULL)free(lst->d);
      if ( lst->next != NULL ){
        lst->name = lst->next->name;
        lst->d    = lst->next->d;
        lst->flag = lst->next->flag;
        lst->next = lst->next->next;
        free(lst_n);
      }else{
        lst->name = NULL;
        lst->d = NULL;
      }
      return(0);
    }
  }

  lst_p = lst;
  lst_n = lst;
  i = 0 ;
  while(1){
    if ( lst_n == NULL )break;
    if ( lst_n->name == NULL )break;
    if ( !strcmp(lst_n->name,name) ){
      if ( lst_p != NULL ) lst_p->next = lst_n->next;
      if ( lst_n->name != NULL )free(lst_n->name);
      if ( lst_n->d != NULL )free(lst_n->d);
      free(lst_n);
      return(0);
    }
    lst_p = lst_n;
    lst_n = lst_n->next;
  }
  return(0);
}

void datalist_flag_clear( T_DATA_LIST *lst ){
  T_DATA_LIST *p; int i;
  if ( lst == NULL )return;
  for(i=0;i<lst->hashcount;i++){
    p = &lst[i];
    while(1){
      if (p==NULL)break;
      p->flag = 0;
      p = p->next;
    }
  }
}

char * datalist_get_n( T_DATA_LIST *alst, char *name, int len ){
  char *ret;
  static char null_char[] = {""};
  T_DATA_LIST *lst_n;
  T_DATA_LIST *lst;

  if (alst==NULL)return(NULL);
  if (name==NULL)return(NULL);

  if ( len < 0 )return(NULL);
  lst = &alst[ hash_n(alst,name,len) ];
  lst_n = lst;
  ret = null_char;
  while(1){
    if ( lst_n == NULL )break;
    if (lst_n->name==NULL )break;
    if ( !strncmp(lst_n->name,name,len) ){
      ret = lst_n->d;
      lst_n->flag = 1;
      break;
    }
    lst_n = lst_n->next;
  }

  return(ret);
}

char * datalist_get( T_DATA_LIST *lst, char *name ){
  if (name == NULL)return(NULL);
  return(datalist_get_n( lst, name , strlen(name) ));
}


int reallocstrcat_n( char **s, int *s_len, int *w_s_len, char *a, int a_len ){
  char *r;
  int r_len;
  int new_len;
  if (*s==NULL ) {
    *s_len = REALLOCSTEPSIZE;
    *s = (char*)malloc( REALLOCSTEPSIZE );
    if ( *s == NULL ) return (-1);
    *s[0]=0;
  }
  r_len     = *w_s_len + a_len + 1;

  if ( r_len > (*s_len + 1)){
    new_len = r_len + ( r_len % REALLOCSTEPSIZE);
    r = (char*)realloc( *s, new_len );
    if ( r == NULL ) return(-1);
    *s = r;
    memcpy(*s+(*w_s_len),a,a_len);
    *s_len = new_len;
  }else{
    memcpy(*s+ (*w_s_len),a,a_len);
  }
  *(*s+ (*w_s_len)+a_len) = 0;
  *w_s_len +=a_len;

  return(0);
}

int reallocstrcat ( char **s, int *s_len, int *w_s_len, char *a ){
  return(  reallocstrcat_n(s,s_len,w_s_len,a,strlen(a))  );
}

char * replace_cgiunperse_n ( char *s, int s_len, T_DATA_LIST *dl ){
  int i;
  int len;
  int d_start;
  int d_len;
  char *ret; int ret_len;
  char *g; char *gg;
  char *w;
  char *S;
  int  s_idx; int w_s_idx;
  int n;
  char *stock; int stock_len;
  int d_i;
  int paste_f = 0;
  int convert_f = 0;

  if (  s == NULL )return(NULL);
  if ( dl == NULL )return(NULL);
  if ( s_len < 0  )return(NULL);  

  ret_len = 0;
  ret = NULL;
  S = allocstr_upper_n(s,s_len);
  /*S=s;*/
  if ( s == NULL ) return(NULL);

  len = s_len;
  d_start = -1;
  s_idx=0; w_s_idx = 0;
  stock = NULL; stock_len = 0;
  
  d_i = -1;
  for (i=0;i<len;i++){
    if (s[i] =='\\' ){
      memcpy(s+i,s+i+1,len-i-1); s[len-1] = 0;
      len --;
      continue;
    }
    if (i>0){
#ifdef DEBUG
      if ( (unsigned char)s[i] & 0x80 ){
        printf("%02x(%c)",(unsigned char)s[i],'.');
      }else{
        printf("%02x(%c)",(unsigned char)s[i],s[i]);
      }
#endif
      if ( charset_flag == CHARSET_SHIFT_JIS ){
        if (SJIS1((unsigned char)s[i-1]) && SJIS2((unsigned char)s[i])){
#ifdef DEBUG
          printf("-");
#endif
          i++;
          continue;
        }
      }
    }
    if ( d_start < 0 ){
     if (s[i]=='@'){
       if ( (i + 1)<len ){
         if(s[i+1]=='@'){
           convert_f = 1;
           i++;
         }
       }
       d_start = i;
       paste_f = 1;
       continue;
     }
    }
#ifdef DELETE
    if (s[i]=='@'){
      if ( d_start > 0 ) {
        convert_f = 1; /* @@となった */
      }else{
        d_start = i;
        paste_f = 1;
      }
      continue;
    }
#endif
    if (s[i] == '$' ){
      d_start = i;
      paste_f = 0;
      continue;
    }
    if (d_start>=0 && (!isalnum(s[i]) && s[i]!='_') ){
      d_len = i - d_start;
      g = datalist_get_n( dl, S + d_start + 1 , d_len -1 );
      stock = g;
      if ( g == NULL ){
        if ( reallocstrcat_n( &ret,&ret_len,&w_s_idx,s+s_idx,(d_start+d_len-s_idx)) != 0 ) return(NULL);
        s_idx=d_start + d_len;
      }else{
        g = cgiunperse(g);
        if(g==NULL)return(NULL);
        stock = g; 
        stock_len = strlen(g);
        if ( reallocstrcat_n( &ret,&ret_len,&w_s_idx,s+s_idx,(d_start-convert_f-s_idx)) != 0 )return(NULL);
        if ( paste_f ){
          if (convert_f){
	   {char *wg;
            wg = tagconvert(g);
            if (wg == NULL)return(NULL);
            if ( reallocstrcat( &ret,&ret_len,&w_s_idx,wg) != 0 )return(NULL);
            free(wg);
           }
          }else{
            if ( reallocstrcat( &ret,&ret_len,&w_s_idx,g) != 0 ) return(NULL);
          }
	}
        /*free(g);*/
        s_idx=d_start + d_len;
      }
      d_start = -1;
      convert_f = 0;
      d_i = -1;
      if (s[i]=='@'){
        d_start = i;
        paste_f = 1;
        continue;
      }
    }
    if (stock && s[i]==':' && d_i == -1){
      d_i = i + 1;
      continue;
    }
    if (stock &&d_i>0 && s[i]==':'){
      if(!strncmp(stock,s+d_i,i-d_i)){
        for(n=i+1;n<s_len;n++){
          if ( s[n] ==':' ) {
            break;
	  }
        }
        w_s_idx-=stock_len;
        if( reallocstrcat_n(&ret,&ret_len,&w_s_idx,(char*)(s+i+1),(n - i - 1)) != 0 ){
	  return(NULL);
        }
        s_idx += ((n - i)+2+i-d_i);
        i = n -1;
      }else{
        w_s_idx-=stock_len;
        for(n=i+1;n<s_len;n++){
          if ( s[n] ==':' ) break;
        }
        s_idx += ((n - i)+2+i-d_i);
        i = n;
	}
       d_i = -1;
       if ( stock != NULL ){
         free(stock);
         stock= NULL;
       }
    }
    
  }
  if ( reallocstrcat(&ret,&ret_len,&w_s_idx,(s+s_idx)) != 0 )return(NULL);
  free(S);
  return(ret);
}

char * replace_cgiunperse ( char *s, T_DATA_LIST *dl ){
  return(replace_cgiunperse_n(s,strlen(s),dl));
}

char *tagconvert_n( char *d ,int n ){
  char *ret;int i;
  char *w;
  if (d==NULL)return(NULL);
  if (n<0)return(NULL);
  ret = allocstr("");
  if (ret == NULL)return(NULL);
  for (i=0;i<n;i++){
    if(d[i]=='<'){
      w = ret;
      ret = allocstrcat(ret,"&lt;");
      if (ret == NULL)return(NULL);
      free(w);
    }else{
      w = ret;
      ret = allocstrcat_n(ret,strlen(ret),d+i,1);
      if (ret == NULL)return(NULL);
      free(w);
    }
  }
  return(ret);
}

char *tagconvert(char *d){
  if (d == NULL)return(NULL);
  return(tagconvert_n(d,strlen(d)));
}

/* 単なるQUERY_STRING を T_DATA_LIST型リストへの登録 */

int query_string_to_datalist_n( T_DATA_LIST **dl, char *d, int dlen ){
  char *dstr;
  int i; int len; 
  int n; int nlen;
  int start_idx, end_idx;
  char *cstr[2];


#ifdef DEBUG
  int count;
#endif

  if ( d   == NULL ) return(-1);
  if ( *dl == NULL ) *dl = datalist_create(0);
  if ( *dl == NULL ) return(-1);

  dstr=d;

  nlen=dlen;
  start_idx = 0;
  for(i=0;i<nlen;i++){
    if (dstr[i]=='&' || i==(nlen-1)){
      end_idx = i;
      if (i==(nlen-1))end_idx++;
      for (n=start_idx;n<end_idx;n++){
        if(dstr[n]=='='){

#ifdef DEBUG
	 {
          count++;
          printf("%d datalist_set(",count);
          fwrite(dstr+start_idx,n-start_idx,1,stdout);
          printf(")(");
          fwrite(dstr+n+1,end_idx-n-1,1,stdout);
          printf(") name len=%d data len=%d\n",n-start_idx,end_idx-n-1);
         }
#endif

	  cstr[0] = (char*)allocstr_upper_n(dstr+start_idx,(n - start_idx));
          cstr[1] = cgiunperse_n(dstr+n+1,end_idx - n -1 );
          /*datalist_set_n(*dl,dstr+start_idx,n-start_idx,dstr+n+1,end_idx-n-1);*/

          datalist_set(*dl,cstr[0],cstr[1]);
          break;
        }
      }
      start_idx = i + 1;
    }
  }
  return(0); 
}

/* SAVE_QUERY_STRING までの解釈を行う */

int QUERY_STRING_to_datalist_n( T_DATA_LIST **dl, char *d, int dlen ){
  char *dstr;
  int i; int len; 
  int n; int nlen;
  int start_idx, end_idx;
  char *cstr[2]; int ret;
  int save_query_string_flag  = 1;

#ifdef DEBUG
  int count;
#endif

  if ( d   == NULL ) return(-1);
  if ( *dl == NULL ) *dl = datalist_create(0);
  if ( *dl == NULL ) return(-1);

  dstr=d;

#ifdef DEBUG
  count=0;
#endif
  nlen=dlen;
  start_idx=0;
  cstr[0]=NULL;
  cstr[1]=NULL;
  for(i=0;i<nlen;i++){
    if (dstr[i]=='&' || i==(nlen-1)){
      end_idx = i;
      if (i==(nlen-1))end_idx++;
      for (n=start_idx;n<end_idx;n++){
        if(dstr[n]=='='){
	  cstr[0] = (char*)allocstr_upper_n(dstr+start_idx,(n - start_idx));
	  /*printf("%s(%d):(%s)",__FILE__,__LINE__,cstr[0]);*/
          if(!strcmp(cstr[0],"SAVE_QUERY_STRING")){
            /*SAVE_QUERY_STRING に対する処理 */
#ifdef DEBUG
            {char *g;
	    g=allocstr_n(dstr+n+1,end_idx-n-1);
	    logprintf("%s(%d):(%s):\n",__FILE__,__LINE__,g);
            free(g);
            }
#endif
            if (save_query_string_flag){
              save_query_string_flag = 0;
	      cstr[1]=cgiunperse_n(dstr+n+1,end_idx - n -1);
              if(cstr[1]==NULL){
	        logprintf("%s(%d) cgiunperse return = NULL\n",__FILE__,__LINE__);
	        return(-1);
              }
              ret = query_string_to_datalist_n(dl,cstr[1],strlen(cstr[1]));
              if (ret) return(ret);
            }
            free(cstr[0]);free(cstr[1]);
            i = nlen;
	    break;
	  }
          free(cstr[0]);
          break;
        }
      }
      start_idx = i + 1;
    }
  }

  nlen=dlen;
  start_idx = 0;
  for(i=0;i<nlen;i++){
    if (dstr[i]=='&' || i==(nlen-1)){
      end_idx = i;
      if (i==(nlen-1))end_idx++;
      for (n=start_idx;n<end_idx;n++){
        if(dstr[n]=='='){
	  cstr[0] = (char*)allocstr_upper_n(dstr+start_idx,(n - start_idx));
          if ( !strcmp(cstr[0],"SAVE_QUERY_STRING")){
	    free(cstr[0]);
	  }else{
            cstr[1] = cgiunperse_n(dstr+n+1,end_idx - n -1 );
            /*datalist_set_n(*dl,dstr+start_idx,n-start_idx,dstr+n+1,end_idx-n-1);*/
            datalist_set(*dl,cstr[0],cstr[1]);
	  }
          break;
        }
      }
      start_idx = i + 1;
    }
  }
  return(0); 
}

int QUERY_STRING_to_datalist( T_DATA_LIST **dl, char *d){
  return(QUERY_STRING_to_datalist_n(dl,d,strlen(d)));
}

int HTTP_COOKIE_to_datalist_n( T_DATA_LIST **dl, char *d, int dlen ){
  char *dstr;
  int i; int len; 
  int n; int nlen;
  int start_idx, end_idx; int skip_idx;
  char *cstr[2]; int ret;

  if ( d   == NULL ) return(-1);
  if ( *dl == NULL ) *dl = datalist_create(0);
  if ( *dl == NULL ) return(-1);

  dstr=d;

  nlen=dlen;
  start_idx=0;
  cstr[0]=NULL;
  cstr[1]=NULL;

  nlen=dlen;
  start_idx = 0;
#ifdef DEBUG
  logprintf("%s(%d)\n",__FILE__,__LINE__);
#endif
  for(i=0;i<nlen;i++){
    if (dstr[i]==';' || i==(nlen-1)){
      end_idx = i;
      if (i==(nlen-1))end_idx++;
      for (n=start_idx;n<end_idx;n++){
        if(dstr[n]=='='){
          {int i;
	   skip_idx = 0;
	   for(i=0;i<(n-start_idx);i++){
             if (dstr[start_idx+i]!=' ') {
               skip_idx = i;
               break;
             }
	   }
          }
	  cstr[0] = (char*)allocstr_upper_n(dstr+start_idx+skip_idx,(n - start_idx - skip_idx));
          cstr[1] = cgiunperse_n(dstr+n+1,end_idx - n -1 );
          datalist_set(*dl,cstr[0],cstr[1]);
#ifdef DEBUG
          logprintf("%s(%d) http_cookie(%s=%s)\n",__FILE__,__LINE__,cstr[0],cstr[1]);
#endif
          break;
        }
      }
      start_idx = i + 1;
    }
  }
  return(0); 
}

int HTTP_COOKIE_to_datalist( T_DATA_LIST **dl, char *d){
  return(HTTP_COOKIE_to_datalist_n(dl,d,strlen(d)));
}

int datalist_to_QUERY_STRING( T_DATA_LIST *dl, char **d, int *d_len){
  T_DATA_LIST *p;
  char *w;
  int i, sep; int d_s_len;
  d_s_len = 0;
  sep=0;
  for(i=0;i<dl->hashcount;i++){
   p= &dl[i];
   while(1){
    if (p ==  NULL)break;
    if ( p->d    == NULL )break;
    if ( p->name == NULL )break;

    /* separator                                */
    if ( sep )
      if ( reallocstrcat( d,d_len,&d_s_len,"&") != 0 )return(-1);

    sep = 1;

    /* name                                     */
    if ( reallocstrcat( d,d_len,&d_s_len,p->name ) != 0 )return(-1);
#ifdef DEBUG
    fprintf(stderr,"%s(%d) p->name=(%s)",__FILE__,__LINE__,p->name);
#endif
    /* =                                        */
    if ( reallocstrcat( d,d_len,&d_s_len,"=" ) != 0 )return(-1);

    /* data                                     */
#ifdef DEBUG
    fprintf(stderr," p->d=(%s) ::: ",p->d);
    printf("%s(%d)p->name=(%s)p->d=(%s) ::: ",__FILE__,__LINE__,p->name,p->d);
#endif
    w = cgiparse( p->d );
    if ( w == NULL ) return(-1);
    if ( reallocstrcat( d,d_len,&d_s_len,w ) != 0 )return(-1);
#ifdef DEBUG
    fprintf(stderr," p->d=(%s)\n",p->d);
#endif
    free (w);
           
    p = p->next;  
   }
  }  
  return(0);
}

int edit_number_n( char *d, int d_len, char *s, int s_len, char *fmt, int fmt_len ){
  char *m;
  char *upper;
  char **arr;
  int arr_cnt;
  int i;
  int mode;

  int number_hlen;
  int number_llen;

#define MODE_NUMBER     1
#define MODE_NUMBER_STR "NUMBER"
#define MODE_DATE       2
#define MODE_DATE_STR   "DATE(YYYY/MM/DD)"
 
  upper = allocstr_upper_n( fmt, fmt_len );
  if(upper==(char*)NULL)return(-1);
  arr_cnt = split(&arr,upper,"().",NULL);

  if ( arr_cnt > 0 ){
    if (!strcmp(arr[0],MODE_NUMBER_STR)) mode = MODE_NUMBER;
    if (!strcmp(arr[0],MODE_DATE_STR))   mode = MODE_DATE;
  }
  switch(mode){
  case MODE_NUMBER:
    number_hlen=0;
    number_llen=0;
    if ( arr_cnt > 1 ) number_hlen = atoi(arr[1]);
    if ( arr_cnt > 2 ) number_llen = atoi(arr[2]);
    {char *mm; int mm_len; int i_p; int n;
     mm_len = number_hlen + number_llen + 1 + (number_llen > 0 ? 1 : 0);
     mm = (char*)malloc( mm_len );
     if (mm==NULL)return(-1);
     memset(mm,0,mm_len);
     memset(mm,'\0',mm_len-1);
     if ( number_llen ) mm[number_hlen] = '.';
     i_p = strsrch_n(s,s_len,".",1);
     if ( i_p < 0 ){
       n = number_hlen;
       for(i=(s_len-1);i>=0;i--){
         if (s[i]>='0' && s[i]<='9'){
           if ( n == 0 )break;
           mm[n-1]=s[i];
	   n --;
	 }
       }
     }else{
       n = number_hlen;
       for(i=(i_p-1);i>=0;i--){
         if (s[i]>='0' && s[i]<='9'){
           if ( n == 0 )break;
           mm[n-1]=s[i];
	   n --;
	 }
       }
       n = number_hlen;
       /*if (number_llen<s_len )s_len = number_llen;*/
#ifdef DEBUG
       printf("%s(%d) s_len=%d\n",__FILE__,__LINE__,s_len);
#endif
       for(i=(i_p+1);i<(s_len);i++){
         if(s[i]>='0' && s[i]<'9'){
           mm[n+1]=s[i];
           n ++;
         }
       }
     }       
#ifdef DEBUG
     printf("%s(%d) mm=(%s)\n",__FILE__,__LINE__,mm);
#endif

     { int start_idx;
      memset(d,'\0',d_len);
      if (d_len < mm_len){
	start_idx = mm_len - d_len;
        memcpy(d,mm+start_idx-1,d_len);
      }else{
        if ( mm_len < d_len ){
          start_idx = d_len - mm_len;
          memcpy(d+start_idx-1,mm,mm_len);
        }else{
          memcpy(d,mm,d_len);
        }
      }
     }
     free(mm);
    }

    break;
  case MODE_DATE:
    break;
  default:
    return(-1);
  }

  for (i=0;i<arr_cnt;i++) free(arr[i]);
  free(arr);
  free(upper);
  return(0);
}

int edit_number( char *d, char *s, char *fmt ){
  return(edit_number_n( d, strlen(d), s, strlen(s), fmt, strlen(fmt) ));
}

#ifdef DEBUG_MAIN
int main( int argc, char **argv ){

  char d[13];
  char *dd;int dd_len; char *f;
 

  char *ggg = "     ";
  printf("%s(%d):%s:\n",__FILE__,__LINE__,rtrim(ggg));
  exit(-1);

  memset(d,0,sizeof(d));
  memset(d,' ',sizeof(d)-1);
  f="number(14)";edit_number(d,"123.431",f);
  printf("edit_number[%s](%s)\n",f,d);

  memset(d,0,sizeof(d));
  memset(d,' ',sizeof(d)-1);
  f="number(14.2)";edit_number(d,"123.431",f);
  printf("edit_number[%s](%s)\n",f,d);
  
  memset(d,0,sizeof(d));
  memset(d,(char)' ',sizeof(d)-1);
  f="number(14.4)";edit_number(d,"123.431",f);
  printf("edit_number[%s](%s)\n",f,d);

  dd_len = 20; dd = (char*) malloc( dd_len+1 );
  memset(dd,0,dd_len+1);
  memset(dd,(char)' ',dd_len);
  f="number(5.2)";edit_number(dd,"123.431",f);
  printf("edit_number[%s](%s)\n",dd);
  free(dd);

  return(0);

}
#endif
