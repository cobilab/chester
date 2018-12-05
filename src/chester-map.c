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
// - - - - - - - - - - - - - - - - - - J O I N - - - - - - - - - - - - - - - -
void JoinStreams(Param *P){
  uint32_t ref, tar, k = 0, n;
  FILE *OUT = NULL;
  uint64_t step = 0;
  char **nameout = (char **) Calloc(P->tar->nFiles, sizeof(char *));
  P->chrSize  = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar){
    nameout[tar]  = (char *) Calloc(4096, sizeof(char));
    sprintf(nameout[tar], "%s.oxch", P->tar->names[tar]);
    OUT           = Fopen(nameout[tar], "w");
    FILE **Bins   = (FILE **)    Calloc(P->ref->nFiles, sizeof(FILE *));
    char **name   = (char **)    Calloc(P->ref->nFiles, sizeof(char *));
    char **name2  = (char **)    Calloc(P->ref->nFiles, sizeof(char *));
    uint8_t **buf = (uint8_t **) Calloc(P->ref->nFiles, sizeof(uint8_t *));
    uint8_t *res  = (uint8_t *)  Calloc(WINDOW_SIZE,    sizeof(uint8_t));
    for(ref = 0 ; ref < P->ref->nFiles ; ++ref){
      name[ref]  = (char *) Calloc(4096, sizeof(char));
      sprintf(name[ref], "-r%u.xch", ref+1);
      name2[ref] = concatenate(P->tar->names[tar], name[ref]);
      Bins[ref]  = Fopen(name2[ref], "r");
      buf[ref]   = (uint8_t *) Calloc(WINDOW_SIZE, sizeof(uint8_t));
      }
    res = (uint8_t *) Calloc(WINDOW_SIZE+1, sizeof(uint8_t));

    P->chrSize[tar] = P->size[0][tar]; 
    uint64_t min = P->chrSize[tar];
    do{
      for(ref = 0 ; ref < P->ref->nFiles ; ++ref){
        k = fread(buf[ref], 1, WINDOW_SIZE, Bins[ref]);
        if(min > k)
          min = k;
        }
      for(n = 0 ; n < k ; ++n){
        res[n] = '0';
        for(ref = 0 ; ref < P->ref->nFiles ; ++ref){
          if(buf[ref][n] == '7'){
            res[n] = '7'; break;
            }
          else if(buf[ref][n] == '1'){
            res[n] = '1'; break;
            }
          }
        }
      fwrite(res, 1, k, OUT);
      }
    while(min > 0);

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
void Target(Param *P, uint32_t ref, uint32_t tar){
  FILE     *Reader;
  char     fName[4096];
  char     *name1 = (char *) Calloc(4096, sizeof(char));
  char     *namex = (char *) Calloc(4096, sizeof(char));
  sprintf(name1, "-r%u.ch",  (uint32_t) ref+1);
  sprintf(namex, "-r%u.xch", (uint32_t) ref+1);
  char     *name2 = concatenate(P->tar->names[tar], name1);
  char     *namex2 = concatenate(P->tar->names[tar], namex);
  FILE     *Pos = NULL, *Bin = Fopen(namex2, "w");
  uint64_t nSymbols, idx_symbols = 0, raw = 0, unknown = 0, 
           base = 0, hPos = 0, possibleK = 0;
  uint32_t n, k, idxPos, hIndex;
  int32_t  idx_buffer = 0;
  uint8_t  *wBuf, *rBuf, *sBuf, sym, found = 0;
 
  if(P->verbose)
    fprintf(stderr, "Searching target sequence %u ...\n", tar + 1);
            
  if(P->disk == 0)
    Pos = Fopen(name2, "w");

  // OPEN FILE FOR POSSIBLE DECOMPRESSION WITH GZIP
  Reader = Fopen(P->tar->names[tar], "r"); // CHECK IF IS READABLE
  fclose(Reader);
  sprintf(fName, "zcat -f %s", P->tar->names[tar]);
  Reader = Popen(fName, "r");

  #ifdef PROGRESS
  nSymbols = NBytesInFileGZip(Reader, fName);
  #endif

  wBuf  = (uint8_t *) Calloc(BUFFER_SIZE,          sizeof(uint8_t));
  rBuf  = (uint8_t *) Calloc(BUFFER_SIZE,          sizeof(uint8_t));
  sBuf  = (uint8_t *) Calloc(BUFFER_SIZE + BGUARD, sizeof(uint8_t));
  sBuf += BGUARD;

  PARSER   *PA = CreateParser();
  FileType(PA, Reader);
  ResetIdx(P->M);

  while((k = fread(rBuf, 1, BUFFER_SIZE, Reader))){
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      #ifdef PROGRESS
      CalcProgress(nSymbols, idx_symbols);
      #endif
      ++idx_symbols;

      if(ParseSym(PA, (sym = rBuf[idxPos])) == -1){ hPos = 0; continue; }

      // AFTER HEADER IS A BASE OR UNKNOWN BASE
      ++base;
      if((sym = S2N(sym)) == 4){
        ++unknown;
        hPos = 0; // FLUSH INIT OF CONTEXT
        if(P->disk == 0)
          fprintf(Pos, "%"PRIu64"\tN\n", base-P->M->kmer+1);
        fprintf(Bin, "%u", (uint8_t) EXTRA_CHAR_CODE); // THIS IS A FALSE POSITIVE: "N"
        continue;
        }

      sBuf[idx_buffer] = sym;
      GetIdx(sBuf+idx_buffer, P->M);
      if(P->M->ir == 1)
        GetIdxIR(sBuf+idx_buffer, P->M);

      if(++hPos >= P->M->kmer){  // SKIP INITIAL CONTEXT, ALL "AAA..."
        ++possibleK;

        if(P->M->ir == 0){
          if(SearchBloom(P->M->bloom, P->M->idx) == 0){ // IF RAW FOUND
            if(P->disk == 0){
              fprintf(Pos, "%"PRIu64"\t", base-P->M->kmer+1);
              RWord(Pos, sBuf, idx_buffer+1, P->M->kmer);
              }
            fprintf(Bin, "0");
            ++raw;
            }
          else{
            fprintf(Bin, "1");
            }
          }
        else{
          if(SearchBloom(P->M->bloom, P->M->idx) == 0 && 
             SearchBloom(P->M->bloom, P->M->idxIR) == 0){ // IF NOT MATCH:
            if(P->disk == 0){
              fprintf(Pos, "%"PRIu64"\t", base-P->M->kmer+1);
              RWord(Pos, sBuf, idx_buffer+1, P->M->kmer);
              }
            fprintf(Bin, "0");
            ++raw;
            } 
          else{
            fprintf(Bin, "1");
            }
          }
        }
      else{
        fprintf(Bin, "1");
        }

      if(++idx_buffer == BUFFER_SIZE){
        memcpy(sBuf-BGUARD, sBuf+idx_buffer-BGUARD, BGUARD);
        idx_buffer = 0;
        }
      }
    }

  if(P->disk == 0)
    fclose(Pos);
  fclose(Bin);
  pclose(Reader);
  ResetIdx(P->M);
  Free(namex);
  Free(name1);
  Free(name2);
  Free(rBuf);
  Free(wBuf);
  Free(sBuf-BGUARD);
  RemoveParser(PA);

  if(P->verbose == 1){
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID
    fprintf(stderr, "RAWs FOUND  : %.4lf %% ( %"PRIu64" in %"PRIu64" )\n", 
    (double) raw / possibleK * 100.0, raw, possibleK);  
    fprintf(stderr, "Unknown sym : %"PRIu64"\n", unknown);
    fprintf(stderr, "Total base  : %"PRIu64"\n", base);
    if(P->tar->nFiles != tar+1)
      fprintf(stderr, "------------------------------------------\n");
    }
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - R E F E R E N C E - - - - - - - - - - - - -
void LoadReference(Param *P, uint32_t ref){
  FILE     *Reader;
  char     fName[4096];
  uint32_t k, idxPos;
  uint8_t  sym;
  uint64_t idx_symbols = 0, idx_read = 0;
  PARSER   *PA = CreateParser(); 
  CBUF     *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t  *readBuf = (uint8_t *) Calloc(BUFFER_SIZE + 1, sizeof(uint8_t));

  // OPEN FILE FOR POSSIBLE DECOMPRESSION WITH GZIP
  Reader = Fopen(P->ref->names[ref], "r"); // CHECK IF IS READABLE
  fclose(Reader);
  sprintf(fName, "zcat -f %s", P->ref->names[ref]);
  Reader = Popen(fName, "r");

  #ifdef PROGRESS
  uint64_t size = NBytesInFileGZip(Reader, fName);
  #endif

  if(P->verbose == 1)
    fprintf(stderr, "Building reference model (k=%u) ...\n", P->kmer);

  FileType(PA, Reader);
  ResetIdx(P->M);

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      ++idx_symbols;
      #ifdef PROGRESS
      CalcProgress(size, idx_symbols);
      #endif

      if(ParseSym(PA, (sym = readBuf[idxPos])) == -1){ idx_read = 0; continue; }
        symBuf->buf[symBuf->idx] = sym = S2N(sym);

      GetIdx(symBuf->buf+symBuf->idx, P->M);

      if(++idx_read >= P->M->kmer) // SKIP INITIAL CONTEXT FROM EACH READ
        UpdateBloom(P->M->bloom,P->M->idx);

      UpdateCBuffer(symBuf);
      }
 
  RemoveCBuffer(symBuf);
  Free(readBuf);
  RemoveParser(PA);
  pclose(Reader);

  if(P->verbose == 1){
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID  
    fprintf(stderr, "##########################################\n");
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
    PrintMenuMap();
    return EXIT_SUCCESS;
    }

  P = (Param *) Calloc(1 , sizeof(Param));
  P->ref       = ReadFNames (P, argv[argc-2]);  // REF
  P->tar       = ReadFNames (P, argv[argc-1]);  // TAR
  P->kmer      = ArgsNum    (DEFAULT_KMER,    p, argc, "-k", MIN_KMER, MAX_KMER);
  P->bSize     = ArgsNum64  (DEFAULT_BSIZE,   p, argc, "-s", 10, 2999999999999);
  P->verbose   = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  P->inverse   = ArgsState  (DEFAULT_IR,      p, argc, "-i");
  P->disk      = ArgsState  (DEFAULT_DISK,    p, argc, "-p");

  if(P->verbose){
    fprintf(stderr, "==============[ CHESTER v%u.%u ]============\n",
    (uint32_t) VERSION, (uint32_t) RELEASE);
    PrintArgs(P);
    fprintf(stderr, "==========================================\n");
    }

  uint64_t max_entries = 0, min_hashes;
  double max_precision = 0;
  P->size = (uint64_t **) Calloc(P->ref->nFiles, sizeof(uint64_t *));
  for(n = 0 ; n < P->ref->nFiles ; ++n){
    
    // ESTIMATE NUMBER OF HASHES FOR BEST PRECISION ===========================
    FILE *Reader = Fopen(P->ref->names[n], "r"); // CHECK IF IS READABLE
    fclose(Reader);
    char fName[4096];
    sprintf(fName, "zcat -f %s", P->ref->names[n]);
    Reader = Popen(fName, "r"); // POSSIBLE GZIP OPEN
    uint64_t n_entries = EntriesInFileGZip(Reader, P->kmer, fName);
    if(max_entries < n_entries) 
      max_entries = n_entries;

/*
    FILE *Reader = Fopen(P->ref->names[n], "r");
    uint64_t n_entries = EntriesInFile(Reader, P->kmer);
    if(max_entries < n_entries) max_entries = n_entries;
    fclose(Reader);
*/
    P->bHashes = (int32_t) (((double) P->bSize / n_entries) * M_LN2);
    double precision = pow(1-exp(-P->bHashes*((double) n_entries + 0.5)
    / (P->bSize-1)), P->bHashes);

    if(n == 0) min_hashes = P->bHashes;
    if(min_hashes > P->bHashes) min_hashes = P->bHashes;
    if(max_precision < precision) max_precision = precision;

    if(P->verbose){
      fprintf(stderr, "Bloom array size ................... %"PRIu64"\n", 
      P->bSize);
      fprintf(stderr, "Number of entries .................. %"PRIu64"\n", 
      n_entries);
      fprintf(stderr, "Number of optimized hashes ......... %u (%.10lf)\n",
      (uint32_t) P->bHashes, ((double) P->bSize / n_entries) * M_LN2);
      fprintf(stderr, "Probability of false positive ...... %.10lf\n", 
      precision);
      fprintf(stderr, "------------------------------------------\n");
      }

    if(P->bHashes == 0){
      fprintf(stderr, "Error: 0 bloom hashes used!\n");
      fprintf(stderr, "Note: try to increase the bloom array size.\n");
      return 1;
      }

    // ========================================================================

    P->M = CreateModel(P->kmer, P->inverse, P->bHashes, P->bSize);
    LoadReference(P, n);
    P->size[n] = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));
    for(k = 0 ; k < P->tar->nFiles ; ++k){

      FILE *Reader = Fopen(P->tar->names[k], "r"); // CHECK IF IS READABLE
      fclose(Reader);
      char fName[4096];
      sprintf(fName, "zcat -f %s", P->tar->names[k]);
      Reader = Popen(fName, "r"); // POSSIBLE GZIP OPEN
      P->size[n][k] = NDNASyminFileGZip(Reader, fName);
      Target(P, n, k);
      }
    DeleteModel(P->M);
    if(P->verbose)
      fprintf(stderr, "==========================================\n");
    }
  
  if(P->verbose){
    fprintf(stderr, "Bloom metrics:\n");
    fprintf(stderr, "Array size ......................... %"PRIu64"\n", 
    P->bSize);
    fprintf(stderr, "Max number of entries .............. %"PRIu64"\n", 
    max_entries);
    fprintf(stderr, "Min number of optimized hashes ..... %"PRIu64"\n", 
    min_hashes);
    fprintf(stderr, "Max probability of false positive .. %.10lf\n", 
    max_precision);
    fprintf(stderr, "==========================================\n");
    }

  if(P->verbose) fprintf(stderr, "Joinning ...\n");
  JoinStreams(P);
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

