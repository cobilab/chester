#!/bin/bash
(time ./CHESTER-map -p -v -i -s 9955813888 -k 30 $1 $2 ) &> REPORT_CHESTER_MAP # 1GB OF MEMORY
(time ./CHESTER-filter -v -t 0.5 $2 ) &> REPORT_CHESTER_FILTER
./CHESTER-visual -v $2.seg
