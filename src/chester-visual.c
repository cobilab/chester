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
#include "paint.h"
#include "parser.h"
#include "buffer.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - P A I N T - - - - - - - - - - - - - - - -
void PaintStreams(Param *P){
  FILE *Plot = Fopen("plot.svg", "w");
  char backColor[] = "#ffffff";
  uint64_t init, end;
  Painter *Paint;
  uint32_t tar;

  P->chrSize = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));

  // SET MAXIMUM FROM FILE:
  FILE *XReader = Fopen(P->tar->names[0], "r");
  uint64_t x_size = 0, i_size = 0;
  if(fscanf(XReader, "#%"PRIu64"#%"PRIu64"", &x_size, &i_size) != 2){
    fprintf(stderr, "Error: unknown segmented file!\n");
    exit(1);
    }
  P->max = x_size;
  fclose(XReader);

  SetScale(P->max);
  Paint = CreatePainter(GetPoint(P->max), backColor);

  PrintHead(Plot, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) * 
  P->tar->nFiles) - DEFAULT_SPACE), Paint->size + EXTRA);
  Rect(Plot, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) * 
  P->tar->nFiles) - DEFAULT_SPACE), Paint->size + EXTRA, 0, 0, backColor);

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
    FILE *Reader = Fopen(P->tar->names[tar], "r");

    if(fscanf(Reader, "#%"PRIu64"#%"PRIu64"\n", &x_size, &i_size) != 2){
      fprintf(stderr, "Error: unknown segmented file!\n");
      exit(1);
      }

    P->chrSize[tar] = i_size;

    while(fscanf(Reader, "%"PRIu64":%"PRIu64"", &init, &end) == 2){
      Rect(Plot, Paint->width, GetPoint(end-init+1+P->enlarge), Paint->cx,
      Paint->cy + GetPoint(init), GetRgbColor(LEVEL_HUE));
      }

    Chromosome(Plot, Paint->width, GetPoint(P->chrSize[tar]), Paint->cx, 
    Paint->cy);

    if(P->tar->nFiles > 0) Paint->cx += DEFAULT_WIDTH + DEFAULT_SPACE;
    fclose(Reader);
    }

  PrintFinal(Plot);
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
    PrintMenuVisual();
    return EXIT_SUCCESS;
    }

  P = (Param *) Calloc(1 , sizeof(Param));
  P->tar       = ReadFNames (P, argv[argc-1]);  // TAR
  P->enlarge   = ArgsNumI64 (DEFAULT_ENLARGE, p, argc, "-e", -1,  999999999);
  P->verbose   = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");

  if(P->verbose){
    fprintf(stderr, "==============[ CHESTER v%u.%u ]============\n",
    VERSION, RELEASE);
    }

  if(P->verbose) fprintf(stderr, "Painting ...\n");
  PaintStreams(P);
  if(P->verbose){
    fprintf(stderr, "Done!                                     \n");
    fprintf(stderr, "==========================================\n");
    }

  if(P->verbose)
    fprintf(stderr, "Job done in %.3g sec.\n", ((double)(clock()-start))/
    CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


