#!/bin/bash
(time ./CHESTER-map -p -v -i -s 35813888 -k 30 A.fa B.fa ) &> REPORT_CHESTER_MAP # 1GB OF MEMORY
(time ./CHESTER-filter -v -t 0.5 B.fa.oxch ) &> REPORT_CHESTER_FILTER
./CHESTER-visual -v B.fa.seg
