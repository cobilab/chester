#!/bin/bash
GET_GOOSE=1;
GET_CHESTER=1;
GET_NEANDERTHAL=1;
GET_HUMAN=1;
SAM2FASTA=1;
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
  cp CHESTER ../../
  cd ../../
fi
#==============================================================================
# GET GOOSE
if [[ "$GET_GOOSE" -eq "1" ]]; then
  rm -fr goose/ GetHumanParse.sh
  git clone https://github.com/pratas/goose.git
  cp goose/scripts/GetHumanParse.sh .
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
  for((xi=1 ; xi<=56 ; ++xi));
    do
    samtools view HN-C$xi.bam | awk '{OFS="\t"; print ">"$1"\n"$10}' > HN-XC$xi ;
    done
fi
#==============================================================================
# RUN FALCON
if [[ "$RUN_CHESTER" -eq "1" ]]; then # 223338299392 = 26 GB
  (time ./CHESTER -p -v -i -s 223338299392 -t 0.6 -k 30 HN-XC1:HN-XC2:HN-XC3:HN-XC4:HN-XC5:HN-XC6:HN-XC7:HN-XC8:HN-XC9:HN-XC10:HN-XC11:HN-XC12:HN-XC13:HN-XC14:HN-XC15:HN-XC16:HN-XC17:HN-XC18:HN-XC19:HN-XC20:HN-XC21:HN-XC22:HN-XC23:HN-XC24:HN-XC25:HN-XC26:HN-XC27:HN-XC28:HN-XC29:HN-XC30:HN-XC31:HN-XC32:HN-XC33:HN-XC34:HN-XC35:HN-XC36:HN-XC37:HN-XC38:HN-XC39:HN-XC40:HN-XC41:HN-XC42:HN-XC43:HN-XC44:HN-XC45:HN-XC46:HN-XC47:HN-XC48:HN-XC49:HN-XC50:HN-XC51:HN-XC52:HN-XC53:HN-XC54:HN-XC55:HN-XC56 HS1:HS2:HS3:HS4:HS5:HS6:HS7:HS8:HS9:HS10:HS11:HS12:HS13:HS14:HS15:HS16:HS17:HS18:HS19:HS20:HS21:HS22:HS23:HS24:HS25:HS26:HS27 ) &> REPORT_CHESTER_HUMAN_NEAN
fi
#==============================================================================
