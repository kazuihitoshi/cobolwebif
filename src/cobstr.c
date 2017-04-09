#include <stdio.h>
void DSP( char *d, int * len ){
  if (d == NULL) return;
  if ( *len <=0 ) return;
  fwrite( d,*len,1,stdout);
  return;
}
void dsp( char *d, int  len ){
  if (d == NULL) return;
  if ( len <=0 ) return;
  fwrite( d,len,1,stdout);
  return;
}
void dspret( char *d, int * len ){
  if (d == NULL) return;
  if ( *len <=0 ) return;
  fwrite( d,*len,1,stdout);
  printf("\n");
  return;
}

