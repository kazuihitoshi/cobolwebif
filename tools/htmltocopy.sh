#!/bin/sh
for i in $*
do
 awk 'BEGIN{RS="<|>"}{
   dat=toupper($0);
   start_i = index(dat,"NAME=");
   if (start_i){
     d = substr(dat,start_i);
     start_i = index(d,"\"");
     d = substr(d,start_i+1);
     start_i = index(d,"\"");
     name = substr(d,1,start_i-1);
     #
     len = 1;
     start_i = index(dat,"MAXLENGTH=");
     if (start_i){
       d = substr(dat,start_i+10);
       split(d,darr," ");
       d = darr[1];
       gsub("\"","",d);
       len = d;
     }
     ll=length(name);
     print "        03 filler pic x(" ll ") value \"" name "\"."
     print "        03 filler pic x value low-value."
     print "        03 filler pic x value space."
     print "        03 filler pic x value low-value."
     print "        03 " name " pic x(" len ") value space."
     print "        03 filler pic x value low-value."
   }
   
 }
 END{
     print "        03 filler pic x value low-value."
 }' $i
done



