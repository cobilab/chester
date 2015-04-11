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
// - - - - - - - - - - - - - - - - - - J O I N - - - - - - - - - - - - - - - -
void JoinStreams(Param *P){
  uint32_t ref, tar, k, n;
  FILE *OUT = NULL;
  char **nameout = (char **) Calloc(P->tar->nFiles, sizeof(char *));
  uint64_t size, step = 0;

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
      res        = (uint8_t *) Calloc(WINDOW_SIZE, sizeof(uint8_t));
      }
    size = NBytesInFile(Bins[0]); 

    step = WINDOW_SIZE;
    do{
      for(ref = 0 ; ref < P->ref->nFiles ; ++ref)
        k = fread(buf[ref], 1, WINDOW_SIZE, Bins[ref]);
      for(n = 0 ; n < k ; ++n){
        res[n] = '0';
        for(ref = 0 ; ref < P->ref->nFiles ; ++ref){
          if(buf[ref][n] != 0){
            res[n] = '1';
            break;
            }
          }
        }
      fwrite(res, 1, k, OUT);
      step += WINDOW_SIZE;
      for(ref = 0 ; ref < P->ref->nFiles ; ++ref)
        fseeko(Bins[ref], step, SEEK_SET);
      }
    while(step < size && k > 0);

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
  Param    *P;

  if(ArgsState(DEFAULT_HELP, p, argc, "-h") == 1 || argc < 3 ||
    ArgsState(DEFAULT_HELP, p, argc, "?") == 1){
    fprintf(stderr, "Usage: CHESTER <OPTIONS>... [FILE]:<...> [FILE]:<...>\n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  -v                       verbose mode,             \n");
    fprintf(stderr, "  -a                       about CHESTER,            \n");
    fprintf(stderr, "  -n                       bloom hashes number,      \n");
    fprintf(stderr, "  -s                       bloom size,               \n");
    fprintf(stderr, "  -i                       use inversions,           \n");
    fprintf(stderr, "  -k <k-mer>               k-mer size,               \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "  [rFile1]:<rFile2>:<...>  reference file(s) (db),   \n");
    fprintf(stderr, "  [tFile1]:<tFile2>:<...>  target file(s).           \n");
    fprintf(stderr, "                                                     \n");
    fprintf(stderr, "CHESTER is a fast tool to compute completeness maps. \n");
    fprintf(stderr, "The input files should be FASTA (.fa) or SEQ [ACGTN].\n");
    return EXIT_SUCCESS;
    }

  if(ArgsState(0, p, argc, "-a") || ArgsState(0, p, argc, "-V")){
    fprintf(stderr, "CHESTER %u.%u\n"
    "Copyright (C) 2015 University of Aveiro.\nThis is Free software. \nYou "
    "may redistribute copies of it under the terms of the GNU General \n"
    "Public License v2 <http://www.gnu.org/licenses/gpl.html>.\nThere is NO "
    "WARRANTY, to the extent permitted by law.\nCode written by Diogo Pratas"
    " (pratas@ua.pt) and Armando J. Pinho (ap@ua.pt).\n", RELEASE, VERSION);
    return EXIT_SUCCESS;
    }

  kmer = ArgsNum (DEF_MIN_CTX, p, argc, "-k", MIN_CTX, MAX_CTX);

  P = (Param *) Calloc(1 , sizeof(Param));
  P->ref      = ReadFNames(P, argv[argc-2]);  // REF
  P->tar      = ReadFNames(P, argv[argc-1]);  // TAR
  P->id       = n;
  P->context  = kmer;
  P->bSize    = ArgsNum64 (DEFAULT_BSIZE,   p, argc, "-s", 100, 9999999999);
  P->bHashes  = ArgsNum   (DEFAULT_BHASHES, p, argc, "-n", 1,   999999);
  P->verbose  = ArgsState (DEFAULT_VERBOSE, p, argc, "-v");
  P->inverse  = ArgsState (DEFAULT_IR,      p, argc, "-i");

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

  if(P->verbose)
    fprintf(stderr, "Building maps ...\n");

  JoinStreams(P);
 

/*
  char stop = 0;
  char states[P->ref->nFiles];
  for(;;){
    for(n = 0 ; n < P->ref->nFiles ; ++n){
      states[n] = fgetc(BINS[n]);
      if(states[n] == EOF){
        stop = 1;
        break;
        }
      }
    if(stop == 1) break;
    if(states[n] == '1'){
      fprintf(OUT, "1");
      continue;
      }
    fprintf(OUT, "0");
    }

  for(n = 0 ; n < P->ref->nFiles ; ++n){
    fclose(BINS[n]);
    Free(names2[n]);
    Free(names[n]);
    Free(BINS[n]);
    }
  Free(names2);
  Free(names);
  Free(BINS);
  fclose(OUT);
  */   
  if(P->verbose)
    fprintf(stderr, "Done!\n");

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

