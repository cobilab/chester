#ifndef FILTERS_H_INCLUDED
#define FILTERS_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void     WindowSizeAndDrop  (Param *, uint64_t);
float    *InitWinWeights    (int64_t, int32_t);
void     EndWinWeights      (float *);
void     FilterSequence     (char *, Param *, float *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
