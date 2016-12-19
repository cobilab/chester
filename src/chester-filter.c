#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <ctype.h>
#include "mem.h"
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

  WindowSizeAndDrop(P, P->max);
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
    fprintf(stderr,
    "                                                                       \n"
    "                          ===================                          \n"
    "                          |   CHESTER %u.%u   |                        \n"
    "                          ===================                          \n"
    "                                                                       \n"
    "               A probabilistic tool to map and visualize               \n"
    "                     relative singularity regions.                     \n"
    "                                                                       \n"
    "              Copyright (C) 2015-2017 University of Aveiro.            \n"
    "                                                                       \n"
    "                  This is a Free software, under GPLv3.                \n"
    "                                                                       \n"
    "You may redistribute copies of it under the terms of the GNU - General \n"
    "Public License v3 <http://www.gnu.org/licenses/gpl.html>. There is NOT \n"
    "ANY WARRANTY, to the extent permitted by law. Developed and Written by \n"
    "Diogo Pratas, Armando J. Pinho and Paulo J. S. G. Ferreira.\n\n", VERSION,
    RELEASE);
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEFAULT_HELP, p, argc, "-h") == 1 || ArgsState(DEFAULT_HELP, p, 
  argc, "?") == 1 || argc < 3){
    fprintf(stderr, "Usage: CHESTER-filter <OPTIONS>... [FILE]:<...>      \n");
    fprintf(stderr, "CHESTER-filter: a tool to filter maps (CHESTER-map)  \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  -v                       verbose mode,             \n");
    fprintf(stderr, "  -a                       about CHESTER,            \n");
    fprintf(stderr, "  -t <value>               threshold [0.0;1.0],      \n");
    fprintf(stderr, "  -w <value>               window size,              \n");
    fprintf(stderr, "  -u <value>               sub-sampling,             \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  [tFile1]:<tFile2>:<...>  target file(s).           \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "The target files may be generated by CHESTER-map.    \n");
    fprintf(stderr, "Report bugs to <{pratas,ap,pjf}@ua.pt>.              \n");
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
  if(P->verbose)
    fprintf(stderr, "==========================================\n");
  
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

