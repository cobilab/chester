#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "segment.h"
#include "common.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SegmentSequence(char *fName, Param *P, uint32_t tar){
  FILE     *Reader = NULL , *Writter = NULL;
  float    val, threshold;
  uint64_t pos, initPosition, lastPosition;
  int32_t  region;
  char     fNameOut[4096];

  threshold    = (float) P->threshold; 
  Reader       = Fopen(fName, "r");
  sprintf(fNameOut, "%s.seg", P->tar->names[tar]);
  Writter      = Fopen(fNameOut, "w");

  fprintf(Writter, "#%"PRIu64"#%"PRIu64"\n", P->max, P->size[0][tar]); // WRITE MAX AT THE HEAD

  if(fscanf(Reader, "%"PRIu64"\t%f", &pos, &val) == 2)
    region = val < threshold ? LOW_REGION : HIGH_REGION;
  else{
    fprintf(stderr, "Error: unknown format\n");
    exit(1);
    }
  initPosition = 1;
  lastPosition = pos;

  while(fscanf(Reader, "%"PRIu64"\t%f", &pos, &val) == 2){
    if(val >= threshold){ 
      if(region == LOW_REGION){
        region = HIGH_REGION;
        fprintf(Writter, "%"PRIu64":%"PRIu64"\n", initPosition, pos);
        }
      }
    else{ // val < threshold ====> LOW_REGION
      if(region == HIGH_REGION){
        region = LOW_REGION;
        initPosition = pos;
        }
      }
    lastPosition = pos;
    }

  if(region == LOW_REGION)
    fprintf(Writter, "%"PRIu64":%"PRIu64"\n", initPosition, lastPosition);

  fclose(Reader);
  fclose(Writter);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

