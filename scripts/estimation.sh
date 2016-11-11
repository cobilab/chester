#!/bin/bash
#
# usage: . estimation.sh <file.mfa> <k-mer>
#
K=`echo "scale=2;$2-1"  | bc -l`; #K-1
N=`cat $1 | grep -v ">" | wc -c`;
L=`cat $1 | grep -v ">" | wc -l`;
echo "scale=2;$N-($L*$K)" | bc
echo "$N";




