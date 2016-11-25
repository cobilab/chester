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
// - - - - - - - - - - - - - - - - W R I T E   W O R D - - - - - - - - - - - -
void RWord(FILE *F, uint8_t *b, int32_t i, uint32_t kmer){
  uint8_t w[kmer+1], n;
  i -= kmer;
  for(n = 0 ; n < kmer ; ++n)
    w[n] = N2S(b[i+n]);
  w[kmer] = '\0';
  fprintf(F, "%s\n", w);
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - P A I N T - - - - - - - - - - - - - - - -
void PaintStreams(Param *P){
  FILE *Plot = Fopen("plot.svg", "w");
  char backColor[] = "#ffffff";
  uint64_t init, end;
  Painter *Paint;
  uint32_t tar;

  SetScale(P->max);
  Paint = CreatePainter(GetPoint(P->max), backColor);

  PrintHead(Plot, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) * 
  P->tar->nFiles) - DEFAULT_SPACE), Paint->size + EXTRA);
  Rect(Plot, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) * 
  P->tar->nFiles) - DEFAULT_SPACE), Paint->size + EXTRA, 0, 0, backColor);

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
    char *name = (char *) Calloc(4096, sizeof(char));
    sprintf(name, "%s-k%u.seg", P->tar->names[tar], P->kmer);
    FILE *Reader = Fopen(name, "r");

    while(fscanf(Reader, "%"PRIu64"\t%"PRIu64"", &init, &end) == 2){
      Rect(Plot, Paint->width, GetPoint(end-init+1+P->enlarge), Paint->cx,
      Paint->cy + GetPoint(init), GetRgbColor(LEVEL_HUE));
      }

    Chromosome(Plot, Paint->width, GetPoint(P->chrSize[tar]), Paint->cx, 
    Paint->cy);
    if(P->tar->nFiles > 0) Paint->cx += DEFAULT_WIDTH + DEFAULT_SPACE;
    fclose(Reader);
    Free(name);
    }

  PrintFinal(Plot);
  }

/*
for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
  nameout[tar]  = (char *) Calloc(4096, sizeof(char));
  sprintf(nameout[tar], "%s-k%u.oxch", P->tar->names[tar], P->kmer);
*/

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - C H E S T E R   M A I N - - - - - - - - - - - - -
int32_t main(int argc, char *argv[]){
  char     **p = *&argv;
  uint32_t n, k;
  Param    *P;
  clock_t  start = clock();

  if(ArgsState(0, p, argc, "-a") || ArgsState(0, p, argc, "-V")){
    fprintf(stderr, "CHESTER %u.%u\n"
    "Copyright (C) 2015-2017 University of Aveiro.\nThis is Free software.\nYou "
    "may redistribute copies of it under the terms of the GNU \nGeneral "
    "Public License v3 <http://www.gnu.org/licenses/gpl.html>. \nThere is NO "
    "WARRANTY, to the extent permitted by law.\nCode by Diogo Pratas,"
    " Armando J. Pinho and Paulo J. S. G Ferreira\n{pratas,ap,pjf}@ua.pt.\n",
    RELEASE, VERSION);
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEFAULT_HELP, p, argc, "-h") == 1 || ArgsState(DEFAULT_HELP, p, 
  argc, "?") == 1 || argc < 3){
    fprintf(stderr, "Usage: CHESTER-visual <OPTIONS>... [FILE]:<...> [FILE]:<...>\n");
    fprintf(stderr, "CHESTER-visual: a tool to visualize relative singularity regions.\n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  -v                       verbose mode,             \n");
    fprintf(stderr, "  -a                       about CHESTER,            \n");
    fprintf(stderr, "  -e <value>               enlarge painted regions,  \n");
    fprintf(stderr, "  -p                       show positions/words,     \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  [rFile1]:<rFile2>:<...>  reference file(s),        \n");
    fprintf(stderr, "  [tFile1]:<tFile2>:<...>  target file(s).           \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "Report bugs to <{pratas,ap,pjf}@ua.pt>.              \n");
    return EXIT_SUCCESS;
    }

  P = (Param *) Calloc(1 , sizeof(Param));
  P->ref       = ReadFNames (P, argv[argc-2]);  // REF
  P->tar       = ReadFNames (P, argv[argc-1]);  // TAR
  P->kmer      = ArgsNum    (DEFAULT_KMER,    p, argc, "-k", MIN_KMER, MAX_KMER);
  P->threshold = ArgsDouble (DEFAULT_THRESHOLD, p, argc, "-t");
  P->subsamp   = ArgsNumI64 (DEFAULT_SAMPLE_RATIO, p, argc, "-u", -1, 999999);
  P->window    = ArgsNumI64 (DEFAULT_WINDOW,  p, argc, "-w", -1,  9999999);
  P->bSize     = ArgsNum64  (DEFAULT_BSIZE,   p, argc, "-s", 10, 2999999999999);
  P->enlarge   = ArgsNumI64 (DEFAULT_ENLARGE, p, argc, "-e", -1,  999999999);
  P->verbose   = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  P->inverse   = ArgsState  (DEFAULT_IR,      p, argc, "-i");
  P->disk      = ArgsState  (DEFAULT_DISK,    p, argc, "-p");

  if(P->verbose){
    fprintf(stderr, "==============[ CHESTER v%u.%u ]============\n",
    VERSION, RELEASE);
    PrintArgs(P);
    fprintf(stderr, "==========================================\n");
    }

  if(P->verbose) fprintf(stderr, "Painting ...\n");
  PaintStreams(P);
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

