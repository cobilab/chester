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
    fprintf(stderr, "Usage: CHESTER-visual <OPTIONS>... [FILE]:<...>\n");
    fprintf(stderr, "CHESTER-visual: visualize relative singularity regions.\n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  -v                       verbose mode,             \n");
    fprintf(stderr, "  -a                       about CHESTER,            \n");
    fprintf(stderr, "  -e <value>               enlarge painted regions,  \n");
    fprintf(stderr, "  -k <value>               k-mer size (up to 30),    \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  [tFile1]:<tFile2>:<...>  target file(s).           \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "Report bugs to <{pratas,ap,pjf}@ua.pt>.              \n");
    return EXIT_SUCCESS;
    }

  P = (Param *) Calloc(1 , sizeof(Param));
  P->tar       = ReadFNames (P, argv[argc-1]);  // TAR
  P->enlarge   = ArgsNumI64 (DEFAULT_ENLARGE, p, argc, "-e", -1,  999999999);
  P->verbose   = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  P->kmer      = ArgsNum    (DEFAULT_KMER,    p, argc, "-k", MIN_KMER, MAX_KMER);

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

