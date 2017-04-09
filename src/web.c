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
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "sstr.h"
#include "gurdio.h"
#define NO_ALLOC
#ifdef __MINGW_H 
#include <winx/fcntlx.h>
#endif
#include <fcntl.h>
#include "web.h"

T_DATA_LIST *datalist_d = NULL;
T_DATA_LIST *http_cookie_d = NULL;
T_WEB_COOKIE_LIST *cookie_list = (T_WEB_COOKIE_LIST*)NULL;

typedef struct {
  char *programname;
  char *pwd;
  char *arg;
} T_WEB_GLOBAL;

T_WEB_GLOBAL webglobal;
static int initialize_flag = 1;

int charlength_n ( char *d , int len,  char c ){
  int i; int n;
  n = 0;
  for (i=0;i<len;i++){
    if ( d[i] != (char)c )break;
    n ++;
  }
  return( n );
}

int charlength ( char *d, char c ){
  return( charlength_n ( d, strlen(d), c ) );
}

/* コマンド起動オプションをQUERY_STRINGルーチンへ設定 */

char *query_string_programname = (char*)NULL;
char *query_string_arg         = (char*)NULL;

int   query_string_get_f       = 1;

static int arealength ( char *p ){
  int i,p_len;
  int f = 0;
  p_len = 0;
  for(i=0;;i++){
     if(p[i] == 0)f++;
     else f = 0;
     if ( f > 1 ) {
       p_len = (i - 1);
       break;
     }
  }
  return(p_len);
}

static int initialize(){
  char *d, *dd;

#ifdef GET_CURRENT_DIR_NAME
  webglobal.pwd = (char*)get_current_dir_name();
#else
  { char *w = NULL; char *wret;int wlen=81;
   while(1){
     w = (char*)malloc(wlen);
     if (w==NULL) {
       return(-1);
     }
     memset(w,0,wlen);
     wret = getcwd(w,(wlen - 1));
     if (wret!=NULL)break;
     wlen+=80;
     free(w);
   }
   webglobal.pwd = allocstrcat(w,"/");
   free(w);
  }
#endif
  if ( (char*)getenv("SCRIPT_FILENAME") == (char*)NULL ) return(-1);

  d = allocstr((char*)getenv("SCRIPT_FILENAME"));
  if (d==NULL)return(-1);

#ifdef DEBUG
  fprintf(stderr,"%s(%d) SCRIPT_FILENAME=(%s)\n",__FILE__,__LINE__,d);
#endif

  /*
  webglobal.programname = allocstrcat(webglobal.pwd,d);
  */
  webglobal.programname = d;
  if(webglobal.programname == NULL)return(-1);

#ifdef DEBUG
  logprintf("%s(%d) webglobal.programname = %s\n         webglobal.pwd = %s\n",
  __FILE__,__LINE__,webglobal.programname,webglobal.pwd);
#endif
  inifileread( webglobal.pwd, NULL );

  return(0);
}

int datalist_show( T_DATA_LIST *d, char *screen_filename, FILE *ffp ){

  FILE *fp; char *data; int data_size; int len; char *p; 
  int lno; int print_flag;
#ifdef SHOW_FAST_FLAG
  int show_fast=1;
#endif

  extern int charset_flag;

  int form_nest_count = 0;

  typedef struct s_sc {
    char *d;
    struct s_sc *next;
  }t_sc;

  t_sc *sc;
  t_sc *sc_top;
  t_sc *sc_p;

  if ( d == NULL )return(-1);
  if ( screen_filename ==NULL ){
    fputs("Content-Type: text/html\n\n"                                                 ,ffp);
    fputs("<HTML LANG=\"ja\" DIR=\"LTR\">\n"                                            ,ffp);
    fputs("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=X-EUC-JP\">\n",ffp);
    fputs("ページファイルが指定されていません。<br>"                                ,ffp);
    fputs("</HTML>\n", ffp);
    return(0);
  }
  fp = gfopen (screen_filename,"r");
  if (fp==NULL){
    fputs("Content-Type: text/html\n\n"                                                 ,ffp);
    fputs("<HTML LANG=\"ja\" DIR=\"LTR\">\n"                                            ,ffp);
    fputs("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=X-EUC-JP\">\n",ffp);
    fputs("指定されたページファイルが存在しません。<br>"                                ,ffp);
    fprintf(ffp,"FileName (%s)<br>\n",screen_filename);
    fputs("</HTML>\n", ffp);
    return(0);
  }
  data_size = 0;
  data = NULL;
  lno=0;

  charset_flag = CHARSET_EUC_JP;

  sc     = (t_sc*)NULL;
  sc_top = (t_sc*)NULL;
  
  /* 読込 */
  while(1){
    len = efgets(&data,&data_size,fp);
    if ( len <0 ) {
      if ( sc != NULL){
        free(sc_p->next);
        sc_p->next = NULL;
      }
      break;
    }
    if ( sc == NULL ) {
       sc = (t_sc*) malloc(sizeof(t_sc));
       sc_top = sc;
       if ( sc == NULL ) return(-1);
    }
    sc->d = (char*)malloc( len + 1 );
    if ( sc->d == NULL ) return(-1);
    memset(sc->d,0,len +1 );
    memcpy(sc->d,data,len);
    sc->next = (t_sc*)malloc( sizeof(t_sc) );
    if ( sc->next == NULL ) return(-1);
    sc_p    = sc;
    sc      = sc->next;
    sc->next= NULL;
  }
  fclose(fp);

  /* 文字コードセット検索 */

  sc = sc_top;
  {int found_flag = 0; char *r;
   while(1) {
    if ( sc == (t_sc*)NULL ) break;
    { char *d; char *r;
     d = allocstr_upper(sc->d);
     if (d==NULL)return(-1);
     if(strstr(d,"<META")) found_flag = 1;                 /*かなり手抜きなタグ解析*/
     
     if ( found_flag ){
      r = (char*)strstr(d,"CHARSET");
      if(r!=NULL){
        if(strstr(r,"EUC-JP")){
	  charset_flag = CHARSET_EUC_JP;
        }else if ( strstr(r,"SHIFT_JIS") ){
          charset_flag = CHARSET_SHIFT_JIS;
        }
      }
     }
     if(strstr(d,">")) found_flag = 0;
     free(d);
    }
    sc = sc->next;
   }
  }

  sc = sc_top;
  while(1){
    print_flag=1;
    if ( sc ==NULL ) break;
    p = replace_cgiunperse (sc->d,d);
    if ( p == NULL ){
      logprintf(
       "%s(%d) replace_cgiunperse() return null probably a memory is insufficient\n",__FILE__,__LINE__ );
      return(-1);
    }
    if (lno==0){
      { char *dd; int ret;
       dd = allocstr_upper(p);
       if (!strstr(dd,"CONTENT-TYPE:"))fputs("Content-Type: text/html\n\n",ffp);
       free(dd);
      }
    }

#ifdef SHOW_FAST_FLAG
    if (show_fast){
#endif
     {char *dd; T_DATA_LIST *l;int ii; /* 最後まで出力されなかった変数はすべて hiddenで出力 */
      dd = allocstr_upper(p);
      if (strstr(dd,"<FORM")){  /* フォーム開始時にフォームデータ一括出力しておく */
	/*form_nest_count ++;*/
	form_nest_count=1;
       if(form_nest_count==1){ 
        {char *w=NULL;int wlen=0;int wslen=0;T_DATA_LIST *lst_p = d;int i; int ret;int fast_flag = 1; char *q;
         
         for(i=0;i<d->hashcount;i++){
           lst_p = &d[i];
           while(1){
             if(lst_p==NULL)break;
             if(lst_p->name!=NULL){;
               if(fast_flag==0){
                 ret =reallocstrcat(&w,&wlen,&wslen,"&");
                 if(ret!=0)return(ret);
               }
               if (lst_p->name!=NULL){
                 ret=reallocstrcat(&w,&wlen,&wslen,lst_p->name);
                 if ( ret != 0 ) return(ret);
                 ret=reallocstrcat(&w,&wlen,&wslen,"=");
                 if ( ret != 0 ) return(ret);
                 fast_flag = 0;
 	      }
               if (lst_p->d != NULL){
                 ret=reallocstrcat(&w,&wlen,&wslen,lst_p->d);
                 if ( ret != 0 ) return(ret);
               }
 	    }
             lst_p = lst_p->next;  
           }
         }
 	 fputs(p,ffp);
         fprintf(ffp,"<input type=\"hidden\" name=\"SAVE_QUERY_STRING\" value=\"%s\">\n",w);
         free(w);
 	 print_flag=0;
        }
       }
#ifdef SHOW_FAST_FLAG
        show_fast = 1;
#endif
      }
      if (strstr(dd,"</FORM>")){ 
       form_nest_count =0;
       if (form_nest_count<0)form_nest_count=0;
      }
      free(dd);
     }
#ifdef SHOW_FAST_FLAG
    }
#endif
    if(print_flag)fputs(p,ffp);
    if (p!=NULL){
      free(p);
      p = NULL;
    }
    lno ++;
    sc = sc->next;
  }
  free(data);
  sc = sc_top;
  while(1){ 
    if (sc==NULL)break;
    sc_p = sc;
    sc   = sc->next;
    free(sc_p->d);
    free(sc_p);
  }
  return(0);
}

int WEB_LOCK( T_WEB_LOCK *lock_id , int *rc ){

  char *l;
  struct flock lock;
  int fd;
  int lock_mode;
  
  if ( lock_id == NULL ) {
    *rc = -1;
    return(-1);
  }

  l = rtrim( lock_id->lockname );
  if ( l == NULL ) {
    *rc = -1;
    return(-1);
  }

  switch ( lock_id->mode ) {
  case LOCK_MODE_EXCLUSIVE:
    lock_mode = F_WRLCK;
    break;
  case LOCK_MODE_SHARE:
    lock_mode = F_WRLCK;
    break;
  default:
    *rc = -1;
    return(-1);
  }

  lock_id->id = open( l, O_RDWR | O_CREAT, 0666 );
  free ( l );
  if ( lock_id->id < 0 ) {
    *rc = -1;
    return(-1);
  }

  lock.l_type = lock_mode;
  lock.l_whence = SEEK_SET;
  lock.l_start = (off_t)0;
  lock.l_len = (off_t)0;
  if (fcntl(lock_id->id,F_SETLKW,&lock)<0){
    *rc = -1;
    return(-1);
  }

  *rc = 0;
  return(0);
}

int WEB_UNLOCK ( T_WEB_LOCK *lock_id , int *rc ){
  struct flock lock;
  lock.l_type  = F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = (off_t)0;
  lock.l_len = (off_t)0;
  *rc = 0;
  if (fcntl(lock_id->id,F_SETLKW,&lock)<0){
    *rc = -1;
    return(-1);
  }
  *rc = 0;
  return(0);
}

int WEB_GETENV_N( char *dist, int *dist_len, int *ret_len,char *src , int *src_len){
  char *d;
  char *d_src; int copy_len;

  d_src = rtrim_n(src,*src_len);
  if ( d_src == NULL ) return(-1);

  memset(dist,' ',*dist_len);

  d = (char*)getenv(d_src);

  if ( d == NULL ){
    free(d_src);
    return(0);
  }

  copy_len = strlen(d);
  if ( copy_len > *dist_len ){
    copy_len = *dist_len;
  }
  
  memcpy ( dist,d,copy_len );

  *ret_len = copy_len;

  return(0);
}

int WEB_GETENV( char *dist,int *ret_len,char *src ){
  char *d;
  char *d_src; int copy_len;
  int dist_len;
  int src_len;

  dist_len = strlen(src);
  src_len  = strlen(dist);

  d_src = rtrim_n(src,src_len);
  if ( d_src == NULL ) return(-1);

  memset(dist,' ',dist_len);

  d = (char*)getenv(d_src);

  if ( d == NULL ){
    free(d_src);
    return(0);
  }

  copy_len = strlen(d);
  if ( copy_len > dist_len ){
    copy_len = dist_len;
  }
  
  memcpy ( dist,d,copy_len );

  *ret_len = copy_len;

  return(0);
}


int WEB_SET_COOKIE( T_WEB_COOKIE *c, int *rc ){

  T_WEB_COOKIE_LIST *now;
  T_WEB_COOKIE_LIST *prev;

  c->name_stop    = 0;
  c->value_stop   = 0;
  c->expires_stop = 0;
  c->path_stop    = 0;
  c->domain_stop  = 0;
  c->secure_stop  = 0;

  now  = cookie_list;
  prev = cookie_list;

  for(;;){
    if (now==NULL) break;
    prev = now;
    now  = now->next;
  }

  if (now==NULL){
    now = (T_WEB_COOKIE_LIST *)malloc(sizeof(T_WEB_COOKIE_LIST));
    if ( now == NULL ) return(-1);
    memset(now,0,sizeof(T_WEB_COOKIE_LIST));
  }
  now->name    = rtrim(c->name);   if(now->name   ==NULL ) return(-1);
  now->value   = rtrim(c->value);  if(now->value  ==NULL ) return(-1);
  now->expires = rtrim(c->expires);if(now->expires==NULL ) return(-1);
  now->path    = rtrim(c->path);   if(now->path   ==NULL ) return(-1);
  now->domain  = rtrim(c->domain); if(now->domain ==NULL ) return(-1);
  now->secure  = rtrim(c->secure); if(now->secure ==NULL ) return(-1);
  now->next = (T_WEB_COOKIE_LIST *)NULL;

  if(prev==NULL){
    cookie_list = now;
  }else{
    prev->next = now;
  }

  return(0);
}

int WEB_SEND_COOKIE(){
  T_WEB_COOKIE_LIST *now;
  T_WEB_COOKIE_LIST *prev;
  char *d;
  int fast = 1;
  now = cookie_list;
  prev=now;
  for (;;){
    if ( now == NULL ) break;
    d = cgiparse(now->value);
#ifdef DEBUG
    logprintf("cgiunperse(%s)\n",d);
#endif
    if ( d == NULL ) {
      logprintf("%s(%d)cgiunperse error\n",__FILE__,__LINE__);
      return(-1);
    }
    printf("Set-Cookie: %s=%s;",now->name,d);
    free(d);
    if(strlen(now->expires))printf(" expires=%s;",now->expires);
    if(strlen(now->domain))printf(" domain=%s;",now->domain);
    if(strlen(now->path))printf(" path=%s;",now->path);
    if(strlen(now->secure))printf(" %s;",now->secure);
    printf("\n");
    fast = 0;
    prev = now;
    now=now->next;
    if(prev->name)   free(prev->name);
    if(prev->value)  free(prev->value);
    if(prev->expires)free(prev->expires);
    if(prev->domain) free(prev->domain);
    if(prev->path)   free(prev->path);
    if(prev->secure) free(prev->secure);
    if(prev) free(prev);
    prev = (T_WEB_COOKIE_LIST*)NULL;
  }
  cookie_list = (T_WEB_COOKIE_LIST*)NULL;
  return(0);
}

int WEB_POP_COOKIE ( char *p ){
  char *s,*str,*d,*upper_str;
  char *name,*attrib,*data;
  int  p_len,p_cpy_len,len;
  int  name_len,attrib_len,data_len;
  if ( p == NULL ) return(-1);

  p_len = arealength(p);
  s = p;
  p_cpy_len = p_len;
  while(1){
    if ( (p + p_len) <= s ) break;

    name_len   = strnlen(s,           p_cpy_len);             p_cpy_len -= name_len;
    attrib_len = strnlen(s+name_len+1,p_cpy_len);             p_cpy_len -= attrib_len;
    data_len   = strnlen(s+name_len+1+attrib_len+1,p_cpy_len);p_cpy_len -= data_len;

    d    = trim_n(s,name_len); 
    if (d == NULL) return(-1);
    upper_str = allocstr_upper(d);
    if (upper_str == NULL)return(-1);
    free(d);
    name = upper_str;

    d    = trim_n(s+name_len+1,attrib_len);
    if (d == NULL)return(-1);
    attrib = d;
    
    d = datalist_get(http_cookie_d,name );
    if (d!=NULL){
      data = cgiunperse(d);
    }
    memset(s+name_len+1+attrib_len+1,(char)' ',data_len);
    if(data!=NULL){
      len=strlen(data);
      if (len>data_len )len=data_len;
      memcpy(s+name_len+1+attrib_len+1,data,len);
    }

    if(name  !=NULL){ free(name);   name   = NULL; }
    if(attrib!=NULL){ free(attrib); attrib = NULL; }
    if(data  !=NULL){ free(data);   data   = NULL; }

    s += name_len +1+attrib_len+1+data_len+1;
  }
  
  return(0);
}

int WEB_PUSH ( char *p ){
  char *s,*str,*d,*upper_str;
  char *name,*attrib,*data;
  int  p_len,p_cpy_len;
  int  name_len,attrib_len,data_len;

  if ( initialize_flag ){ initialize(); initialize_flag = 0;}

  if ( p == NULL ) return(-1);

  p_len = arealength(p);
  s = p;
  p_cpy_len = p_len;
  while(1){
    if ( (p + p_len) <= s ) break;

    name_len   = strnlen(s,           p_cpy_len);             p_cpy_len -= name_len;
    attrib_len = strnlen(s+name_len+1,p_cpy_len);             p_cpy_len -= attrib_len;
    data_len   = strnlen(s+name_len+1+attrib_len+1,p_cpy_len);p_cpy_len -= data_len;

    d    = trim_n(s,name_len); 
    if (d == NULL) return(-1);
    upper_str = allocstr_upper(d);
    if (upper_str == NULL)return(-1);
    free(d);
    name = upper_str;

    d    = trim_n(s+name_len+1,attrib_len); 
    if (d == NULL)return(-1);
    attrib = d;

    /* これを修正しないといけない　前スペースが消去されてしまう */
    d   = rtrim_n(s+name_len+1+attrib_len+1,data_len);
    /*d     = allocstr_n(s+name_len+1+attrib_len+1,data_len);*/
    if (d == NULL)return(-1);
#ifdef DEBUG
    fprintf(stderr,"%s(%d) datalist_set(%s,%s)\n",__FILE__,__LINE__,name,d);
#endif
    data = cgiparse(d);
    
    if(data==NULL)return(-1);
    free(d);

#ifdef DEBUG
    fprintf(stderr,"%s(%d) datalist_set(%s,%s)\n",__FILE__,__LINE__,name,data);
#endif

    datalist_set( datalist_d,name,data);

    if(name  !=NULL){ free(name);   name   = NULL; }
    if(attrib!=NULL){ free(attrib); attrib = NULL; }
    if(data  !=NULL){ free(data);   data   = NULL; }

    s += name_len +1+attrib_len+1+data_len+1;
  }
  
  return(0);
}

int WEB_POP ( char *p ){
  char *s,*str,*d,*upper_str;
  char *name,*attrib,*data;
  int  p_len,p_cpy_len,len;
  int  name_len,attrib_len,data_len;

  if ( initialize_flag ){ initialize(); initialize_flag = 0;}

  if ( p == NULL ) return(-1);

  p_len = arealength(p);
  s = p;
  p_cpy_len = p_len;
  while(1){
    if ( (p + p_len) <= s ) break;

    name_len   = strnlen(s,           p_cpy_len);             p_cpy_len -= name_len;
    attrib_len = strnlen(s+name_len+1,p_cpy_len);             p_cpy_len -= attrib_len;
    data_len   = strnlen(s+name_len+1+attrib_len+1,p_cpy_len);p_cpy_len -= data_len;

    d    = trim_n(s,name_len); 
    if (d == NULL) return(-1);
    upper_str = allocstr_upper(d);
    if (upper_str == NULL)return(-1);
    free(d);
    name = upper_str;

    d    = trim_n(s+name_len+1,attrib_len);
    if (d == NULL)return(-1);
    attrib = d;
    
    d = datalist_get(datalist_d,name );
    if (d!=NULL){
      data = cgiunperse(d);
    }
    memset(s+name_len+1+attrib_len+1,(char)' ',data_len);
    if(data!=NULL){
      len=strlen(data);
      if (len>data_len )len=data_len;
      memcpy(s+name_len+1+attrib_len+1,data,len);
    }

    if(name  !=NULL){ free(name);   name   = NULL; }
    if(attrib!=NULL){ free(attrib); attrib = NULL; }
    if(data  !=NULL){ free(data);   data   = NULL; }

    s += name_len +1+attrib_len+1+data_len+1;
  }
  
  return(0);
}

int WEB_SHOW(){
  char *s,*str,*d,*upper_str;
  char *execute, *screen_filename,*programname,*query_string;
  int query_string_len;
  int  ret;
  int  p_len,p_cpy_len,len;

  if ( initialize_flag ){ initialize(); initialize_flag = 0;}

  programname = datalist_get( datalist_d,"PROGRAMNAME");

  if(programname!=NULL){
    if(strlen(programname)==0)programname=NULL;    
  }
  if(programname!=NULL){
    { char *d;
#ifdef DEBUG
    fprintf(stderr,"%s(%d) programname=(%s)\n",__FILE__,__LINE__,programname);
#endif
    d = allocstrcat(webglobal.pwd,programname);
    if(d==NULL)return(-1);
    /*free(programname);*/
    programname = d;
    }
  }

#ifdef DEBUG
  fprintf(stderr,"%s(%d) programname=(%s) webglobal.programname(%s) webglobal.pwd=(%s)\n",__FILE__,__LINE__,programname,webglobal.programname,webglobal.pwd);
#endif
  if (programname!=NULL && webglobal.programname != NULL){
#ifdef DEBUG
    printf("%s(%d) programname=(%s)(%s)\n",__FILE__,__LINE__,programname,webglobal.programname);
#endif
    if(strcmp(programname,webglobal.programname)){
      query_string = NULL; query_string_len = 0;
      ret = datalist_to_QUERY_STRING(datalist_d, &query_string, &query_string_len);
      if (ret != 0 )return(-1);
#ifdef DEBUG
  fprintf(stderr,"%s(%d) query_string=(%s)\n",__FILE__,__LINE__,query_string);
  fprintf(stderr,"%s(%d) programname=(%s)\n",__FILE__,__LINE__,programname);
  fprintf(stderr,"%s(%d) programname=(%s)\n",__FILE__,__LINE__,programname);
#endif
      { char *d; char *p;
        p = "QUERY_STRING=";
        d = (char*)malloc( query_string_len + 14 );
        if(d==NULL)return(-1); 
        memset(d,0,query_string_len+14);
        strcpy(d,p);
        strcat(d,query_string);
        free(query_string);
        putenv(d);

        { char*d;
	 d = allocstrcat("SCRIPT_FILENAME=",programname);
         if(d==NULL)return(-1);
	 putenv(d);
        }

        datalist_free(datalist_d);
#ifdef DEBUG
	printf("%s(%d) execlp(%s) QUERY_STRING=%s SCRIPT_FILENAME=",__FILE__,__LINE__,programname,getenv("QUERY_STRING"),getenv("SCRIPT_FILENAME"));
#endif
        /*chdir(webglobal.pwd);*/
        execlp(programname,programname,NULL);
      }
    }
  }

  if ( cookie_list != NULL){
    ret = WEB_SEND_COOKIE();
    if ( ret != 0 ){
      logprintf("%s(%d) WEB_SEND_COOKIE() = %d\n",__FILE__,__LINE__,ret);
    }
  }

  screen_filename = datalist_get( datalist_d,"SCREENNAME" );
  if ( screen_filename != NULL ){
    ret = datalist_show( datalist_d,screen_filename,stdout );
    if ( ret < 0 ) return(-1);
    return(0);
  }

  return(0);
}

int WEB_LOGWRITE( char *d ){
  logprintf("%s\n",d);
  return(0);
}

int WEB_GET_QUERY_STRING (){
  char *s;
  char *name;
  char *d; char *dd; char *upper_str;
  char *query_string; int query_string_size;int query_string_length;
  char **array;int array_count;
  char *attrib;
  char *wp; char *wp2;
  int i; int n; int iret,ret;
  int s_idx, s_len;
  int name_len; int attrib_len;int data_len; int area_len;
  int query_array_count;
  char *LENGTH; int length;
  int p_cpy_length; int p_len;
  char *screen_filename; char *programname;
  char *execute;
  char *http_cookie;
#ifdef DEBUG
  logprintf("%s(%d) WEB_GET_QUERY_STRING\n",__FILE__,__LINE__);
#endif

  if ( initialize_flag ){ initialize(); initialize_flag = 0;}

  query_string_size = 0;

#ifdef DEBUG
  logprintf("%s(%d) query_string_get_f = %d\n",__FILE__,__LINE__,query_string_get_f);
#endif
  /*-------------  QUERY_STRING READ------------------- */
  if ( query_string_get_f ){
      /* -------------------------*/
      /* input QUERY_STRING  data */
      /* -------------------------*/
      query_string = (char*)getenv("QUERY_STRING");
#ifdef DEBUG
      printf("%s(%d) QUERY_STRING=%s\n",__FILE__,__LINE__,query_string);
#endif
      if ( query_string != NULL ){
        if (strlen(query_string)==0) query_string = NULL;
      }
      if ( query_string != NULL ){ 
        if (strlen(query_string ) == 0 ) query_string = NULL;
      }

      if ( query_string == NULL ){
        LENGTH = (char*)getenv("CONTENT_LENGTH");
        if ( LENGTH != NULL ) {
          if(strlen(LENGTH) == 0 )LENGTH = NULL;
        }
        if ( LENGTH != NULL ){
          length = atoi(LENGTH);
          if (length>0){
            query_string = (char*)malloc(length+1);
            memset(query_string,0,length+1);
#ifdef DEBUG
            logprintf("%s(%d) efread start\n",__FILE__,__LINE__);
#endif
            efread(query_string,length,stdin);
#ifdef DEBUG
	    logprintf("%s(%d) efread end\n",__FILE__,__LINE__);
#endif
          }
        }
      }
  
      if (query_string == NULL){
        query_string_length = efgets( &query_string, &query_string_size, stdin );
#ifdef DEBUG
        logprintf("%s(%d) query_string=(%s)\n",__FILE__,__LINE__,query_string);
#endif
        if (query_string_length == -2 ){
          return(-1);
        }
        if ( query_string_length ){
          if (query_string[query_string_length - 1] == 0x0a)
            query_string[query_string_length - 1] = 0;
        }
      }else{
        query_string_length = strlen(query_string);
      }

      logprintf("%s(%d) QUERY_STRING=(%s)\n",__FILE__,__LINE__,query_string);

      QUERY_STRING_to_datalist( &datalist_d, query_string );
      query_string_get_f = 0;
  }
  /*-------------  QUERY_STRING READ------------------- */
  if(query_string_size){
    free(query_string);
    query_string = NULL;
  }
  /*-------------  COOKIE READ ------------*/
  http_cookie = (char*)getenv("HTTP_COOKIE");
  if ( http_cookie != NULL ){
    HTTP_COOKIE_to_datalist ( &http_cookie_d, http_cookie );
#ifdef DEBUG
    logprintf("%s(%d) http_cookie=%s\n",__FILE__,__LINE__,http_cookie);
#endif
  }

  return(0);
}

int WEB_PUT_QUERY_STRING ( char *d ){
  char *dd; char *p;int len; char *upper_str; char *s; char *s2;
  int name_len;
  int attrib_len;
  int data_len;
  char *wstring;
  char *ss; int p_len; int i;
  int p_cpy_length; 
  int fast_flag = 1;
  int ret;
  char *query_string; int query_string_len; int query_string_s_len;

  if ( initialize_flag ){ initialize(); initialize_flag = 0;}

  query_string       = NULL;
  query_string_len   = 0;
  query_string_s_len = 0;
  

  if ( d == NULL )return (-1);

  p_len = arealength(p);

  p = d;
  p_cpy_length = p_len;
  while(1){
    if ( p[0] == 0 )break;

    name_len   = strnlen ( p, p_cpy_length );
                 p_cpy_length -= name_len;
    attrib_len = strnlen ( p + name_len + 1,p_cpy_length ); 
                 p_cpy_length -= attrib_len;
    data_len   = strnlen ( p + name_len + 1 + attrib_len + 1,p_cpy_length );
                 p_cpy_length -= data_len;

    s = trim_n( p, name_len );
    if ( s == NULL ){return(-1);}

    if ( datalist_d != NULL )datalist_del(datalist_d,s);

    if(fast_flag){
      fast_flag = 0;
    }else{
      ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,"&");
      if ( ret != 0 ) return(ret);
    }

    ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,s);
    if ( ret != 0 ) return(ret);

    ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,"=");
    if ( ret != 0 ) return(ret);

    s2 = cgiparse( (p + name_len + 1 + attrib_len + 1) );
    if(s2==NULL){ return(-1);}

    ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,s);
    if ( ret != 0 ) return(ret);

    datalist_set(datalist_d,s,s2);

    free(s);
    free(s2);

    p += name_len + 1 + attrib_len + 1 + data_len + 1 ;
  }

  if ( datalist_d != NULL ){
    { char *w; T_DATA_LIST *lst_p = datalist_d; int i;
    for (i=0;i<datalist_d->hashcount;i++){
      /*if ( datalist_d->name == NULL ) break;*/
      lst_p = &datalist_d[i];
      while(1){
        if (lst_p == NULL)break;
        if (lst_p->name != NULL){
          if (fast_flag ==0){
            ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,"&");
            if ( ret != 0 ) return(ret);
          }
          if (lst_p->name != NULL) { 
            ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,lst_p->name);
            if ( ret != 0 ) return(ret);
            ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,"=");
            if ( ret != 0 ) return(ret);
          }
          if (lst_p->d != NULL){
            ret = reallocstrcat( &query_string,&query_string_len,&query_string_s_len,lst_p->d);
            if ( ret != 0 ) return(ret);
          }
        }
        lst_p = lst_p->next;  
      }
    }
   }
  }
  fputs(query_string,stdout);
  fputs("\n",stdout);

  fclose(stdout);
  free(query_string);
  return(0);
}

#ifdef DEBUG_MAIN
int main(){
  char *a = "abcdef    ";
  char *ret;
  int iret;
  int point;
  char *d; char *g;
  ret = trim(a);
  printf("<%s>\n",ret); free(ret);
  d  = (char*)Malloc( 8192 );
  if ( d == NULL ){
    perror("");exit(-1);
  }
  putenv ( "QUERY_STRING=abc=123&ggg=4567.12");
  memset(d,0,8192);
  g = "abc";
  strcat ( d, g); point = strlen(g) + 1;
  g = "NUMERIC:10:0";
  strcat ( d + point,g); point += strlen(g)+1;
  point += 1+ 10 + 1;
  g = "ggg";
  strcat ( d+point, g); point += strlen(g) + 1;
  g = "NUMERIC:10:2";
  strcat ( d+point, g); point += strlen(g) + 1;
  point += 1+ 10 + 1 + 2;
  g = "@@END@@";
  strcat ( d+point, g); point += strlen(g) + 1;
  QUERY_STRING(d,&iret);
  printf("%s\n",d+4+1+12 );
  printf("%s\n",d+4+1+12+11+1+4+13 );
}
#endif
