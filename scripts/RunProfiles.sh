#!/bin/bash
# RUNNING:
# . RunProfiles $chromosome $SEQUENCE_NCBI_ID
#
# WARNING: 
# REQUIRES: EXTRACT (GOOSE), CHARTOLINE (GOOSE), FILTER (GOOSE), GECO & GNUPLOT
#
MAXMATCH='1';
k=30;
y=1;
old_IFS=$IFS
IFS=$'\n';
printf "\n";
for line in $(cat "HS$1-k$k.seg");
  do
  iPos=`echo $line | awk '{print $1;}'`;
  fPos=`echo $line | awk '{print $2;}'`;
  if [[ $((fPos-iPos)) -ge $MAXMATCH ]]
    then
    echo "Running motif $y [$iPos:$fPos]";
    echo "Run: http://www.ncbi.nlm.nih.gov/projects/sviewer/?id=$2&v=$iPos:$fPos";
    (./Extract -i HS$1-k$k.oxch -o HS$1-$iPos-$fPos -pS $iPos -pL $fPos ) &> GARBAGE;
    (./Extract -i HS$1 -o HS$1-seq-$iPos-$fPos -pS $iPos -pL $fPos ) &> GARBAGE;
    ./GeCo -tm 6:1:0:0/0 -tm 13:50:1:0/0 -tm 18:100:1:3/20 -c 20 HS$1-seq-$iPos-$fPos ;
    rm -f HS$1-seq-$iPos-$fPos GARBAGE;
    ./CharToLine < HS$1-$iPos-$fPos > TMP-MM ;
    ./Filter -p1 -w 5000 -d 100 -1 TMP-MM > TMP-MM.fil
    echo 'set style line 11 lc rgb "#808080" lt 1
    set border 3 back ls 11
    set tics nomirror
    unset key
    plot "TMP-MM.fil" u 1 with lines' | gnuplot -persist
    #rm -f TMP-MM.fil TMP-MM;
    printf "\n";
    ((y++));
    fi
  done
IFS=$old_IFS
