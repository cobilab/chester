#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <ctype.h>
#include "mem.h"
#include "msg.h"
#include "defs.h"
#include "common.h"
#include "model.h"
#include "filters.h"
#include "segment.h"
#include "parser.h"
#include "buffer.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - F I L T E R - - - - - - - - - - - - - - -
void FilterStreams(Param *P){
  float *winWeights;
  uint32_t tar;

  //WindowSizeAndDrop(P, P->max);
  winWeights = InitWinWeights(P->window, W_HAMMING);

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
    char *name = (char *) Calloc(4096, sizeof(char));
    sprintf(name, "%s", P->tar->names[tar]);
    FilterSequence(name, P, winWeights);
    Free(name);
    }

  EndWinWeights(winWeights);
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - S E G M E N T - - - - - - - - - - - - - - -
void SegmentStreams(Param *P){
  uint32_t tar;

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
    char *name = (char *) Calloc(4096, sizeof(char));
    sprintf(name, "%s.fil", P->tar->names[tar]);
    SegmentSequence(name, P, tar);
    Free(name);
    }
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - C H E S T E R   M A I N - - - - - - - - - - - - -
int32_t main(int argc, char *argv[]){
  char     **p = *&argv;
  uint32_t n, k;
  Param    *P;
  clock_t  start = clock();

  if(ArgsState(0, p, argc, "-a") || ArgsState(0, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEFAULT_HELP, p, argc, "-h") == 1 || ArgsState(DEFAULT_HELP, p, 
  argc, "?") == 1 || argc < 3){
    PrintMenuFilter();
    return EXIT_SUCCESS;
    }

  P = (Param *) Calloc(1 , sizeof(Param));
  P->tar       = ReadFNames (P, argv[argc-1]);  // TAR
  P->threshold = ArgsDouble (DEFAULT_THRESHOLD, p, argc, "-t");
  P->subsamp   = ArgsNumI64 (DEFAULT_SAMPLE_RATIO, p, argc, "-u", -1, 999999999);
  P->window    = ArgsNumI64 (DEFAULT_WINDOW,  p, argc, "-w", -1,  9999999999);
  P->verbose   = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");

  if(P->verbose){
    fprintf(stderr, "==============[ CHESTER v%u.%u ]============\n",
    VERSION, RELEASE);
    }

  P->max     = 0;
  P->size    = (uint64_t **) Calloc(P->tar->nFiles, sizeof(uint64_t *));
  P->size[0] = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));
  for(k = 0 ; k < P->tar->nFiles ; ++k){
    char *name = (char *) Calloc(4096, sizeof(char));
    sprintf(name, "%s", P->tar->names[k]);
    FILE *Reader = Fopen(name, "r");
    P->size[0][k] = NBytesInFile(Reader);
    if(P->max < P->size[0][k])
      P->max = P->size[0][k];
    fclose(Reader);
    Free(name);
    }
  WindowSizeAndDrop(P, P->max);
  if(P->verbose){
    fprintf(stderr, "Threshold ................................ %lf\n",
    P->threshold);
    fprintf(stderr, "Subsample ................................ %"PRIu64"\n",
    P->subsamp);
    fprintf(stderr, "Window size .............................. %"PRIu64"\n",
    P->window);
    fprintf(stderr, "==========================================\n");
    }
  
  if(P->verbose) fprintf(stderr, "Filtering ...\n");
  FilterStreams(P);
  if(P->verbose){
    fprintf(stderr, "Done!                                     \n");
    fprintf(stderr, "==========================================\n");
    }

  if(P->verbose) fprintf(stderr, "Segmenting ...\n");
  SegmentStreams(P);
  if(P->verbose){
    fprintf(stderr, "Done!                                     \n");
   fprintf(stderr, "==========================================\n");
    }

  if(P->verbose)
    fprintf(stderr, "All jobs done in %.3g sec.\n", ((double)(clock()-start))/
    CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

