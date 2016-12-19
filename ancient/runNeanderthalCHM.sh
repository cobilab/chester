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
  cp goose/scripts/GetHumanCHMParse.sh .
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
. GetHumanCHMParse.sh
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
# RUN FALCON
if [[ "$RUN_CHESTER" -eq "1" ]]; then # 549755813888 = 64 GB
  (time ./CHESTER-map -p -v -i -s 549755813888 -k 30 xaa:xab:xac:xad:xae CHM1:CHM2:CHM3:CHM4:CHM5:CHM6:CHM7:CHM8:CHM9:CHM10:CHM11:CHM12:CHM13:CHM14:CHM15:CHM16:CHM17:CHM18:CHM19:CHM20:CHM21:CHM22:CHM23 ) &> REPORT_CHESTER_HUMAN_CHM_NEAN
  ./CHESTER-filter -v -t 0.95 CHM1.oxch:CHM2.oxch:CHM3.oxch:CHM4.oxch:CHM5.oxch:CHM6.oxch:CHM7.oxch:CHM8.oxch:CHM9.oxch:CHM10.oxch:CHM11.oxch:CHM12.oxch:CHM13.oxch:CHM14.oxch:CHM15.oxch:CHM16.oxch:CHM17.oxch:CHM18.oxch:CHM19.oxch:CHM20.oxch:CHM21.oxch:CHM22.oxch:CHM23.oxch:CHM24.oxch:CHM25.oxch:CHM26.oxch:CHM27.oxch
  ./CHESTER-visual -v -e 50000 CHM1.oxch.seg:CHM2.oxch.seg:CHM3.oxch.seg:CHM4.oxch.seg:CHM5.oxch.seg:CHM6.oxch.seg:CHM7.oxch.seg:CHM8.oxch.seg:CHM9.oxch.seg:CHM10.oxch.seg:CHM11.oxch.seg:CHM12.oxch.seg:CHM13.oxch.seg:CHM14.oxch.seg:CHM15.oxch.seg:CHM16.oxch.seg:CHM17.oxch.seg:CHM18.oxch.seg:CHM19.oxch.seg:CHM20.oxch.seg:CHM21.oxch.seg:CHM22.oxch.seg:CHM23.oxch.seg:CHM24.oxch.seg:CHM25.oxch.seg:CHM26.oxch.seg:CHM27.oxch.seg
fi
#==============================================================================
~                                                                                                                                                                                                                                                                                                                                                               
~                                                                                                     
