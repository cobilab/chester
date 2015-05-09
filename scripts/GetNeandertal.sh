#!/bin/bash
#=============================================================================
INITALS="HN";
MAX=22;
WGETOP=" --trust-server-names -q ";
ONWAY="http://cdna.eva.mpg.de/neandertal/altai/AltaiNeandertal/bam/AltaiNea.hg19_1000g.";
#-----------------------------------------------------------------------------
function downloadEach
  {
  PATTERN="unexpected";
  for((;;));
    do
    sleep 2;
    xtmp="`wget $1 $2 -O $4-X$3`"
    if [[ "$xtmp" == *"$PATTERN"* ]];
      then
      echo "Unexpected EOF found, re-downloading C$3 ...";
      continue;
    else
      echo "wget stderr:$xtmp";
      echo "Downloaded $4 C$3 with success!";
      break;
    fi
    done
  }

echo "Downloading and filtering $INITALS sequences ...";
for((x=1 ; x <= $MAX ; ++x));
  do
  ZPATH="$ONWAY$x.dq.bam";
  downloadEach "$WGETOP" "$ZPATH" "$x" "$INITALS";
  samtools view $INITALS-X$x | awk '{OFS="\t"; print ">"$1"\n"$10}' > $INITALS$x;
  echo "$INITALS C$x filtered!";
  done

CHR=23;
FIELD="X";
ZPATH="$ONWAY$FIELD.dq.bam";
downloadEach "$WGETOP" "$ZPATH" "$CHR" "$INITALS";
samtools view $INITALS-X$CHR | awk '{OFS="\t"; print ">"$1"\n"$10}' > $INITALS$CHR;
echo "$INITALS CX filtered";

CHR=24;
FIELD="Y";
ZPATH="$ONWAY$FIELD.dq.bam";
downloadEach "$WGETOP" "$ZPATH" "$CHR" "$INITALS";
samtools view $INITALS-X$CHR | awk '{OFS="\t"; print ">"$1"\n"$10}' > $INITALS$CHR;
echo "$INITALS CY filtered";


