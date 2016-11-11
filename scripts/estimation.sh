#!/bin/bash
#
# usage: . estimation.sh <file.mfa> <k-mer>
#
K=`echo "scale=2;$2-1"  | bc -l`; #K-1
C=`cat $1 | grep -v ">" | wc -c`;
L=`cat $1 | grep -v ">" | wc -l`;
N=`echo "scale=2;$C-($L*$K)" | bc`;
#
echo "N=$N";




