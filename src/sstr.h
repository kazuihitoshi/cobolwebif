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
extern char *estrtok_n (
      char *src,        int src_len, 
      const char *a_fs, int a_fs_len,
      const char *a_ns, int a_ns_len,
      char **next 
      );
extern int strsrch_n( char *, int , char *,int );
extern int strsrch( char *, char * );
extern char *estrtok (char *src, const char *fs, const char *ns, char **next );
extern int split( char ***des, char *src, const char *fs, const char *ns );
extern char *allocstr_n( char *s , int len);
extern char *allocstr( char *d );
extern char *allocstr_upper( char *d );
extern char *allocstr_lower( char *d );
extern char *allocstr_upper_n( char *d, int len );
extern char *allocstr_lower_n( char *d, int len );
extern char *allocstrcat_n( char *s1, int s1_len, char *s2, int s2_len );
extern char *allocstrcat( char *s1, char *s2 );
extern int  efgets( char **d, int *d_buf_size, FILE *fp );
extern char * cgiunperse_n ( char *d , int len );
extern char * cgiunperse ( char *d );
extern char * cgiparse_n ( char *d, int len );
extern char * cgiparse ( char *d );
extern char * trim ( char *d );
extern char * trim_n( char *d, int len);
extern char * rtrim ( char *d );
extern char * rtrim_n( char *d, int len);
extern int codecount( char *d, char c );
extern int codecount_n( char *d, int len, char c );
/*extern int strnlen( char *d , int len );*/
typedef struct DATA_LIST {
  char *name;
  int  name_len;
  char *d;
  int  d_len;
  int flag;
  int hashcount;
  struct DATA_LIST *next;
}T_DATA_LIST;

extern T_DATA_LIST *datalist_create();
extern void datalist_free( T_DATA_LIST *d );
extern int datalist_set( T_DATA_LIST *lst, char *name, char *d );
extern int datalist_del( T_DATA_LIST *lst, char *name );
extern char * datalist_get_n( T_DATA_LIST *lst, char *name, int len );
extern char * datalist_get( T_DATA_LIST *lst, char *name );
extern char * replace_n ( char *s, int s_len, T_DATA_LIST *dl );
extern char * replace ( char *s, T_DATA_LIST *dl );
extern char * replace_cgiunperse ( char *s, T_DATA_LIST *dl );
extern char * tagconvert_n(char *d,int len);
extern char * tagconvert (char*d);
extern int  efread( char *d, int size, FILE *fp);
int QUERY_STRING_to_datalist( T_DATA_LIST **dl, char *d );
int datalist_to_QUERY_STRING( T_DATA_LIST *dl, char **d, int *d_len);

#define efgets_buffer_step_size 1024
#define MEMSTEPSIZE 10
#define REALLOCSTEPSIZE 81920
#define CHARSET_EUC_JP    0
#define CHARSET_SHIFT_JIS 1

