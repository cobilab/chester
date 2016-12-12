#!/bin/bash
GET_GOOSE=1;
GET_CHESTER=1;
GET_SAMTOOLS=1;
GET_NEANDERTHAL=1;
GET_HUMAN=1;
SAM2FASTA=1;
SPLIT_FASTA=1;
RUN_CHESTER=1;
#==============================================================================
# sudo apt-get install samtools
#==============================================================================
# GET CHESTER
if [[ "$GET_CHESTER" -eq "1" ]]; then
  rm -fr chester/
  git clone https://github.com/pratas/chester.git
  cd chester/src/
  cmake .
  make
  cp CHESTER-map ../../
  cp CHESTER-visual ../../
  cd ../../
fi
#==============================================================================
# GET GOOSE
if [[ "$GET_GOOSE" -eq "1" ]]; then
  rm -fr goose/ GetHumanParse.sh
  git clone https://github.com/pratas/goose.git
  cp goose/scripts/GetHumanParse.sh .
  cd goose/src/
  make
  cp goose-fastq2mfasta ../../
  cd ../../
fi
#==============================================================================
# GET SAMTOOLS 1.3.1
if [[ "$GET_SAMTOOLS" -eq "1" ]]; then
  wget https://github.com/samtools/samtools/releases/download/1.3.1/samtools-1.3.1.tar.bz2
  tar -xvf samtools-1.3.1.tar.bz2
  cd samtools-1.3.1/
  ./configure --without-curses
  make
  cp samtools ../
  cd ..
  rm -fr samtools-1.3.1.*
fi
#==============================================================================
# BUILD DB
if [[ "$GET_HUMAN" -eq "1" ]]; then
. GetHumanParse.sh
fi
#==============================================================================
if [[ "$GET_NEANDERTHAL" -eq "1" ]]; then
  # ===========---------------------------------------------
  # | WARNING | THIS REQUIRES AT LEAST 300 GB OF FREE DISK |
  # ===========---------------------------------------------
  #
  # DEPENDECY: SAMTOOLS;
  #
  EVANM="http://cdna.eva.mpg.de";
  EVAPT="$EVANM/neandertal/altai/AltaiNeandertal/bam";
  EVAPK="$EVANM/neandertal/altai/AltaiNeandertal/bam/unmapped_qualfail/";
  WGETO=" --trust-server-names -q ";
  echo "Downloading sequences ... (This may take a while!)";
  for((x=1 ; x<=22 ; ++x));  # GET NEANTHERTAL GENOME IN BAM FORMAT
    do
    wget $WGETO $EVAPT/AltaiNea.hg19_1000g.$x.dq.bam -O HN-C$x.bam;
    done
  wget $WGETO $EVAPT/AltaiNea.hg19_1000g.X.dq.bam -O HN-C23.bam;
  wget $WGETO $EVAPT/AltaiNea.hg19_1000g.Y.dq.bam -O HN-C24.bam;
  # UNMAPPED DATA:
  wget $WGETO $EVAPK/NIOBE_0139_A_D0B5GACXX_7_unmapped.bam -O HN-C25.bam;
  wget $WGETO $EVAPK/NIOBE_0139_A_D0B5GACXX_8_unmapped.bam -O HN-C26.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_1_unmapped.bam -O HN-C27.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_2_unmapped.bam -O HN-C28.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_3_unmapped.bam -O HN-C29.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_4_unmapped.bam -O HN-C30.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_5_unmapped.bam -O HN-C31.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_6_unmapped.bam -O HN-C32.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_7_unmapped.bam -O HN-C33.bam;
  wget $WGETO $EVAPK/SN928_0068_BB022WACXX_8_unmapped.bam -O HN-C34.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_1_unmapped.bam -O HN-C35.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_2_unmapped.bam -O HN-C36.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_3_unmapped.bam -O HN-C37.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_4_unmapped.bam -O HN-C38.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_5_unmapped.bam -O HN-C39.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_6_unmapped.bam -O HN-C40.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_7_unmapped.bam -O HN-C41.bam;
  wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_8_unmapped.bam -O HN-C42.bam;
  wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_1_unmapped.bam -O HN-C43.bam;
  wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_2_unmapped.bam -O HN-C44.bam;
  wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_3_unmapped.bam -O HN-C45.bam;
  wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_5_unmapped.bam -O HN-C46.bam;
  wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_6_unmapped.bam -O HN-C47.bam;
  wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_7_unmapped.bam -O HN-C48.bam;
  wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_8_unmapped.bam -O HN-C49.bam;
  wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_1_unmapped.bam -O HN-C50.bam;
  wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_2_unmapped.bam -O HN-C51.bam;
  wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_3_unmapped.bam -O HN-C52.bam;
  wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_5_unmapped.bam -O HN-C53.bam;
  wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_6_unmapped.bam -O HN-C54.bam;
  wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_7_unmapped.bam -O HN-C55.bam;
  wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_8_unmapped.bam -O HN-C56.bam;
fi
#==============================================================================
# FROM SAM 2 MFASTA
if [[ "$SAM2FASTA" -eq "1" ]]; then
  rm -f NEAN;
  for((xi=1 ; xi<=56 ; ++xi));
    do
    #samtools view HN-C$xi.bam | awk '{OFS="\t"; print ">"$1"\n"$10}' >> NEAN ;
    ./samtools bam2fq HN-C$xi.bam | ./goose-fastq2mfasta >> NEAN;
    done
fi
#==============================================================================
# FROM SAM 2 MFASTA
if [[ "$SPLIT_FASTA" -eq "1" ]]; then
  split --lines=1534662800 < NEAN; # SPLIT IN ~5 FILES, % PAR ENSURE HEADER ON
  rm -f NEAN;
fi
#==============================================================================
# RUN CHESTER
if [[ "$RUN_CHESTER" -eq "1" ]]; then # 223338299392=26 GB, 549755813888=64 GB
  (time ./CHESTER-map -p -v -i -s 549755813888 -t 0.6 -k 30 xaa:xab:xac:xad:xae HS1:HS2:HS3:HS4:HS5:HS6:HS7:HS8:HS9:HS10:HS11:HS12:HS13:HS14:HS15:HS16:HS17:HS18:HS19:HS20:HS21:HS22:HS23:HS24:HS25:HS26:HS27 ) &> REPORT_CHESTER_HUMAN_NEAN
  ./CHESTER-visual -v HS1.seg:HS2.seg:HS3.seg:HS4.seg:HS5.seg:HS6.seg:HS7.seg:HS8.seg:HS9.seg:HS10.seg:HS11.seg:HS12.seg:HS13.seg:HS14.seg:HS15.seg:HS16.seg:HS17.seg:HS18.seg:HS19.seg:HS20.seg:HS21.seg:HS22.seg:HS23.seg:HS24.seg:HS25.seg:HS26.seg:HS27.seg
fi
#==============================================================================
