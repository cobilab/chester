#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <pthread.h>
#include "mem.h"
#include "defs.h"
#include "common.h"
#include "model.h"
#include "filters.h"
#include "segment.h"
#include "paint.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - W R I T E   W O R D - - - - - - - - - - - -
void RWord(FILE *F, uint8_t *b, int32_t i, uint32_t ctx){
  uint8_t w[ctx+1], n;
  i -= ctx;
  for(n = 0 ; n < ctx ; ++n)
    w[n] = N2S(b[i+n]);
  w[ctx] = '\0';
  fprintf(F, "%s\n", w);
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - F I L T E R - - - - - - - - - - - - - - -
void FilterStreams(Param *P){
  float *winWeights;
  uint32_t tar;

  WindowSizeAndDrop(P, P->max);
  winWeights = InitWinWeights(P->window, W_HAMMING);

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
    char *name = (char *) Calloc(4096, sizeof(char));
    sprintf(name, "%s-k%u.oxch", P->tar->names[tar], P->context);
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
    sprintf(name, "%s-k%u.fil", P->tar->names[tar], P->context);
    SegmentSequence(name, P);
    Free(name);
    }
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
    sprintf(name, "%s-k%u.seg", P->tar->names[tar], P->context);
    FILE *Reader = Fopen(name, "r");

    while(fscanf(Reader, "%"PRIu64"\t%"PRIu64"", &init, &end) == 2){
      Rect(Plot, Paint->width, GetPoint(end-init+1), Paint->cx,
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

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - J O I N - - - - - - - - - - - - - - - -
void JoinStreams(Param *P){
  uint32_t ref, tar, k, n;
  FILE *OUT = NULL;
  uint64_t step = 0;
  char **nameout = (char **) Calloc(P->tar->nFiles, sizeof(char *));
  P->chrSize  = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
    nameout[tar]  = (char *) Calloc(4096, sizeof(char));
    sprintf(nameout[tar], "%s-k%u.oxch", P->tar->names[tar], P->context);
    OUT           = Fopen(nameout[tar], "w");
    FILE **Bins   = (FILE **)    Calloc(P->ref->nFiles, sizeof(FILE *));
    char **name   = (char **)    Calloc(P->ref->nFiles, sizeof(char *));
    char **name2  = (char **)    Calloc(P->ref->nFiles, sizeof(char *));
    uint8_t **buf = (uint8_t **) Calloc(P->ref->nFiles, sizeof(uint8_t));
    uint8_t *res  = (uint8_t *)  Calloc(WINDOW_SIZE,    sizeof(uint8_t));
    for(ref = 0 ; ref < P->ref->nFiles ; ++ref){
      name[ref]  = (char *) Calloc(4096, sizeof(char));
      sprintf(name[ref], "-r%u-k%u.xch", ref+1, P->context);
      name2[ref] = concatenate(P->tar->names[tar], name[ref]);
      Bins[ref]  = Fopen(name2[ref], "r");
      buf[ref]   = (uint8_t *) Calloc(WINDOW_SIZE, sizeof(uint8_t));
      }
    res = (uint8_t *) Calloc(WINDOW_SIZE+1, sizeof(uint8_t));

    P->chrSize[tar] = NBytesInFile(Bins[0]); 

    step = WINDOW_SIZE;
    do{
      for(ref = 0 ; ref < P->ref->nFiles ; ++ref)
        k = fread(buf[ref], 1, WINDOW_SIZE, Bins[ref]);
      for(n = 0 ; n < k ; ++n){
        res[n] = '0';
        for(ref = 0 ; ref < P->ref->nFiles ; ++ref){
          if(buf[ref][n] == '1'){
            res[n] = '1';
            break;
            }
          }
        }
      fwrite(res, 1, k, OUT);
      step += WINDOW_SIZE;
      }
    while(step < P->chrSize[tar] && k > 0);

    for(ref = 0 ; ref < P->ref->nFiles ; ++ref){
      fclose(Bins[ref]);
      Free(name2 [ref]);
      Free(name  [ref]);
      Free(buf   [ref]);
      } 
    fclose(OUT);
    Free(name2);
    Free(name);
    Free(buf);
    Free(res);
    }

  P->max = P->chrSize[0];
  for(tar = 1 ; tar < P->tar->nFiles ; ++tar)
    if(P->max < P->chrSize[tar])
      P->max = P->chrSize[tar];
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - T A R G E T - - - - - - - - - - - - - - -
void Target(Param *P, uint8_t ref, uint32_t tar){
  FILE     *Reader = Fopen(P->tar->names[tar], "r");
  char     *name1  = (char *) Calloc(4096, sizeof(char));
  char     *namex  = (char *) Calloc(4096, sizeof(char));
  sprintf(name1, "-r%u-k%u.ch",  ref+1, P->context);
  sprintf(namex, "-r%u-k%u.xch", ref+1, P->context);
  char     *name2  = concatenate(P->tar->names[tar], name1);
  char     *namex2 = concatenate(P->tar->names[tar], namex);
  FILE     *Pos    = Fopen(name2, "w"), *Bin = Fopen(namex2, "w");
  uint64_t nSymbols = NDNASyminFile(Reader), i = 0, raw = 0, unknown = 0;
  uint32_t n, k, idxPos, hIndex, header = 0;
  int32_t  idx = 0;
  uint8_t  *wBuf, *rBuf, *sBuf, sym, found = 0;

  if(P->verbose)
    fprintf(stderr, "Searching target sequence %d ...\n", tar + 1);

  wBuf  = (uint8_t *) Calloc(BUFFER_SIZE,          sizeof(uint8_t));
  rBuf  = (uint8_t *) Calloc(BUFFER_SIZE,          sizeof(uint8_t));
  sBuf  = (uint8_t *) Calloc(BUFFER_SIZE + BGUARD, sizeof(uint8_t));
  sBuf += BGUARD;

  while((k = fread(rBuf, 1, BUFFER_SIZE, Reader))){
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      #ifdef PROGRESS
      CalcProgress(nSymbols, i);
      #endif
      ++i;
      switch(rBuf[idxPos]){
        case '>':  header = 1; continue;
        case '\n': header = 0; continue;  
        default:   if(header==1) continue;
        }
      if((sym = S2N(rBuf[idxPos])) == 4){
        ++unknown;
        continue;
        }
      sBuf[idx] = sym;
      GetIdx(sBuf+idx-1, P->M); 
      if(i > P->M->ctx){  // SKIP INITIAL CONTEXT, ALL "AAA..."
        if(P->M->mode == 0){ // TABLE MODE
          if(!P->M->array.states[P->M->idx]){ // IF NO MATCH:
            fprintf(Pos, "%"PRIu64"\t", i-P->M->ctx);
            fprintf(Bin, "0");
            RWord(Pos, sBuf, idx, P->M->ctx);
            ++raw;
            }
          else{
            fprintf(Bin, "1");
            }
          }
        else{ // BLOOM TABLE
          if(SearchBloom(P->M->bloom, P->M->idx) == 0){ // IF NOT MATCH:
            fprintf(Pos, "%"PRIu64"\t", i-P->M->ctx);
            fprintf(Bin, "0");
            RWord(Pos, sBuf, idx, P->M->ctx);
            ++raw;
            }
          else{
            fprintf(Bin, "1");
            }
          }
        }

      if(++idx == BUFFER_SIZE){
        memcpy(sBuf-BGUARD, sBuf+idx-BGUARD, BGUARD);
        idx = 0;
        }
      }
    }

  fclose(Pos);
  fclose(Bin);
  fclose(Reader);
  ResetIdx(P->M);
  Free(namex);
  Free(name1);
  Free(name2);
  Free(rBuf);
  Free(wBuf);
  Free(sBuf-BGUARD);

  if(P->verbose == 1){
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID
    fprintf(stderr, "RAWs FOUND  : %.4lf %% ( %"PRIu64" in %"PRIu64" )\n", 
    (double) raw / (nSymbols-unknown) * 100.0, raw, nSymbols-unknown);  
    fprintf(stderr, "Unknown sym : %"PRIu64"\n", unknown);
    fprintf(stderr, "Total sym   : %"PRIu64"\n", nSymbols);
    if(P->tar->nFiles != tar+1)
      fprintf(stderr, "------------------------------------------\n");
    }
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - R E F E R E N C E - - - - - - - - - - - - -
void LoadReference(Param *P, uint32_t ref){
  FILE     *Reader = Fopen(P->ref->names[ref], "r");
  uint32_t k, idxPos, header = 0;
  int32_t  idx = 0;
  uint8_t  *rBuf, *sBuf, sym;
  uint64_t i = 0;
  #ifdef PROGRESS
  uint64_t size = NBytesInFile(Reader);
  #endif

  if(P->verbose == 1)
    fprintf(stderr, "Building reference model (k=%u) ...\n", P->context);

  rBuf  = (uint8_t *) Calloc(BUFFER_SIZE + 1, sizeof(uint8_t));
  sBuf  = (uint8_t *) Calloc(BUFFER_SIZE + BGUARD+1, sizeof(uint8_t));
  sBuf += BGUARD;

  while((k = fread(rBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      ++i;
      #ifdef PROGRESS
      CalcProgress(size, i);
      #endif
      switch(rBuf[idxPos]){
        case '>':  header = 1; continue;
        case '\n': header = 0; continue;  
        default:   if(header==1) continue;
        }
      if((sym = S2N(rBuf[idxPos])) == 4) continue;    
      sBuf[idx] = sym;
      GetIdx(sBuf+idx-1, P->M);
      if(i > P->M->ctx){ // SKIP INITIAL CONTEXT, ALL "AAA..."
        Update(P->M);
        if(P->M->ir == 1){  // Inverted repeats
          GetIdxIR(sBuf+idx, P->M);
          UpdateIR(P->M);
          }
        }
      if(++idx == BUFFER_SIZE){
        memcpy(sBuf-BGUARD, sBuf+idx-BGUARD, BGUARD);
        idx = 0;
        }
      }
 
  ResetIdx(P->M);
  Free(rBuf);
  Free(sBuf-BGUARD);
  fclose(Reader);

  if(P->verbose == 1){
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID  
    fprintf(stderr, "==========================================\n");
    }
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - C H E S T E R   M A I N - - - - - - - - - - - - -
int32_t main(int argc, char *argv[]){
  char     **p = *&argv;
  uint32_t n, k, min, max, kmer;
  float    *w;
  Param    *P;
  clock_t  start = clock();

  if(ArgsState(DEFAULT_HELP, p, argc, "-h") == 1 || argc < 2 ||
    ArgsState(DEFAULT_HELP, p, argc, "?") == 1){
    fprintf(stderr, "Usage: CHESTER <OPTIONS>... [FILE]:<...> [FILE]:<...>\n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  -v                       verbose mode,             \n");
    fprintf(stderr, "  -a                       about CHESTER,            \n");
    fprintf(stderr, "  -t <value>               threshold [0.0;1.0],      \n");
    fprintf(stderr, "  -w <value>               window size,              \n");
    fprintf(stderr, "  -u <value>               sub-sampling,             \n");
    fprintf(stderr, "  -n <value>               bloom hashes number,      \n");
    fprintf(stderr, "  -s <value>               bloom size,               \n");
    fprintf(stderr, "  -i                       use inversions,           \n");
    fprintf(stderr, "  -k <value>               k-mer size,               \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  [rFile1]:<rFile2>:<...>  reference file(s) (db),   \n");
    fprintf(stderr, "  [tFile1]:<tFile2>:<...>  target file(s).           \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "CHESTER is a fast tool to compute uniqueness maps.   \n");
    fprintf(stderr, "The input files should be FASTA (.fa) or SEQ [ACGTN].\n");
    return EXIT_SUCCESS;
    }

  if(ArgsState(0, p, argc, "-a") || ArgsState(0, p, argc, "-V")){
    fprintf(stderr, "CHESTER %u.%u\n"
    "Copyright (C) 2015 University of Aveiro.\nThis is Free software.\nYou "
    "may redistribute copies of it under the terms of the GNU \nGeneral "
    "Public License v2 <http://www.gnu.org/licenses/gpl.html>. \nThere is NO "
    "WARRANTY, to the extent permitted by law.\nCode by Diogo Pratas,"
    " Armando J. Pinho and Paulo J. S. G Ferreira\n{pratas,ap,pjf}@ua.pt.\n", 
    RELEASE, VERSION);
    return EXIT_SUCCESS;
    }

  kmer = ArgsNum (DEF_MIN_CTX, p, argc, "-k", MIN_CTX, MAX_CTX);

  P = (Param *) Calloc(1 , sizeof(Param));
  P->ref       = ReadFNames(P, argv[argc-2]);  // REF
  P->tar       = ReadFNames(P, argv[argc-1]);  // TAR
  P->context   = kmer;
  P->threshold = ArgsDouble (DEFAULT_THRESHOLD, p, argc, "-t");
  P->subsamp   = ArgsNum    (DEFAULT_SAMPLE_RATIO, p, argc, "-u", 1, 999999);
  P->window    = ArgsNumI64 (DEFAULT_WINDOW,  p, argc, "-w", -1,  9999999);
  P->bSize     = ArgsNum64  (DEFAULT_BSIZE,   p, argc, "-s", 100, 9999999999);
  P->bHashes   = ArgsNum    (DEFAULT_BHASHES, p, argc, "-n", 1,   999999);
  P->verbose   = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  P->inverse   = ArgsState  (DEFAULT_IR,      p, argc, "-i");

  if(P->verbose){
    fprintf(stderr, "==============[ CHESTER v%u.%u ]============\n", 
    VERSION, RELEASE);
    PrintArgs(P);
    fprintf(stderr, "==========================================\n");
    }

  P->size = (uint64_t **) Calloc(P->ref->nFiles, sizeof(uint64_t *));
  for(n = 0 ; n < P->ref->nFiles ; ++n){
    P->M = CreateModel(P->context, P->inverse, P->bHashes, P->bSize); 
    LoadReference(P, n);
    P->size[n] = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));
    for(k = 0 ; k < P->tar->nFiles ; ++k){
      FILE *Reader = Fopen(P->tar->names[k], "r");
      P->size[n][k] = NDNASyminFile(Reader);
      fclose(Reader);
      Target(P, n, k);
      }
    DeleteModel(P->M);
    if(P->verbose)
      fprintf(stderr, "==========================================\n");
    }

  if(P->verbose) fprintf(stderr, "Joinning ...\n");
  JoinStreams(P);
  if(P->verbose){
    fprintf(stderr, "Done!                                     \n");
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

