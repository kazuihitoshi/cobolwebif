#!/bin/sh
T="0"
if [ "$1" = "-tabular" ];then
  T="1"
  shift
fi

texttotex(){
  if [ "$T" = "0" ];then
    awk '{
      s=$0; 
      p = "";
      if(p==""&&index(s,"!")==0)p="!";
      if(p==""&&index(s,"@")==0)p="@";
      if(p==""&&index(s,"$")==0)p="$";
      if(p==""&&index(s,"&")==0)p="&";
      if(p==""&&index(s,"+")==0)p="+";
      if(p==""&&index(s,"-")==0)p="-";
      if(p==""&&index(s,"/")==0)p="/";
      if(p==""&&index(s,"*")==0)p="*";
      #print "\\vspace{-0.4em}\\verb" p s p
      print "\\vspace{-0.3em}\\verb" p s p
      #print "\\verb" p s p
    }' $1
  fi
  if [ "$T" = "1" ];then
    awk '{
      s=$0; 
      p = "";
      if(p==""&&index(s,"!")==0)p="!";
      if(p==""&&index(s,"@")==0)p="@";
      if(p==""&&index(s,"$")==0)p="$";
      if(p==""&&index(s,"&")==0)p="&";
      if(p==""&&index(s,"+")==0)p="+";
      if(p==""&&index(s,"-")==0)p="-";
      if(p==""&&index(s,"/")==0)p="/";
      if(p==""&&index(s,"*")==0)p="*";
      print "\\vspace{-0.4em}\\verb" p s p "\\\\"
    }' $1
  fi
}

for i in $*
do
  if [ "$T" = "1" ];then
    echo "\begin{tabular}{|l|}"
    echo "\hline"
  fi
  texttotex $i
  if [ "$T" = "1" ];then
    echo "\\\\"
    echo "\hline"
    echo "\end{tabular}"
  fi
done





