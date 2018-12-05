#!/bin/bash
#
# usage: . estimation.sh <file.mfa> <k-mer>
#
K=`echo "scale=2;$2-1" | bc -l`; #K-1
C=`grep -v ">" $1 | wc -c`;
L=`grep -v ">" $1 | wc -l`;
N=`echo "scale=2;($C-($L*$K))*2" | bc`;
#
echo "N=$N"; #130942622992 * 2 // IR

