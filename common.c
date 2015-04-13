#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "defs.h"
#include "mem.h"
#include "common.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NBytesInFile(FILE *F){
  uint64_t s = 0;
  fseek(F, 0, SEEK_END);
  s = ftell(F);
  rewind(F);
  return s;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NDNASyminFile(FILE *file)
  {
  uint8_t  buffer[BUFFER_SIZE];
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, file)))
    for(idx = 0 ; idx < k ; ++idx)
      switch(buffer[idx])
        {
        case 'A': ++nSymbols; break;
        case 'T': ++nSymbols; break;
        case 'C': ++nSymbols; break;
        case 'G': ++nSymbols; break;
        default : ++nSymbols; break;
        }

  rewind(file);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t FopenBytesInFile(const char *fn)
  {
  uint64_t size = 0;
  FILE *file = Fopen(fn, "r");
  
  size = NBytesInFile(file);  
  fclose(file);

  return size;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t S2N(uint8_t c){
  switch(c){
    case 'A': case 'a':                     return 0;
    case 'T': case 't': case 'U': case 'u': return 3;
    case 'C': case 'c':                     return 1;
    case 'G': case 'g':                     return 2;
    default :                               return 4;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t N2S(uint8_t c){
  switch(c){
    case 0: return 'A';
    case 3: return 'T';
    case 1: return 'C';
    case 2: return 'G';
    default: fprintf(stderr, "Error: unknown numerical symbols\n"); exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetCompSym(uint8_t symbol)
  {
  switch(symbol)
    {
    case 'A': return 'T';
    case 'C': return 'G';
    case 'G': return 'C';
    case 'T': return 'A';
    default:  return symbol;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetCompNum(uint8_t symbol)
  {
  switch(symbol)
    {
    case 0: return 3;
    case 1: return 2;
    case 2: return 1;
    case 3: return 0;
    default:  fprintf(stderr, "symbol\n");
    return symbol;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILE *Fopen(const char *path, const char *mode)
  {
  FILE *file = fopen(path, mode);

  if(file == NULL)
    {
    fprintf(stderr, "Error opening: %s (mode %s). Does the file exist?\n", 
    path, mode);
    exit(1);
    }

  return file;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

uint8_t *ReverseStr(uint8_t *str, uint32_t end)
  {
  uint32_t start = 0;

  while(start < end)
    {
    str[start] ^= str[end];
    str[end]   ^= str[start];
    str[start] ^= str[end];
    ++start;
    --end;
    }

  return str;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SortString(char str[])
  {
  char     tmp;
  int32_t  i, j, length = strlen(str);

  for(i = 0 ; i != length-1 ; ++i)
    for (j = i+1 ; j != length ; ++j)
      if(str[i] > str[j])
        {
        tmp    = str[i];
        str[i] = str[j];
        str[j] = tmp;
        }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *CloneString(char *str)
  {
  char *clone;
  if(str == NULL)
    return NULL;
  strcpy((clone = (char*) Malloc((strlen(str) + 1) * sizeof(char))), str);
  return clone;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *concatenate(char *a, char *b)
  {
  char *base;

  base = (char *) Malloc(strlen(a) + strlen(b) + 1);
  strcpy(base, a);
  strcat(base, b);
  return base;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *RepString(const char *str, const char *old, const char *new)
  {
  size_t sLen = strlen(str) + 1;
  char *cout = 0, *p = 0, *tmp; 

  if(!(p = (cout = (char *) Malloc(sLen * sizeof(char)))))
    return 0;
  while(*str)
    if((*str & 0xc0) != 0x80 && !strncmp(str, old, strlen(old)))
      {
      p   -= (intptr_t) cout;
      tmp  = strcpy(p = (cout = (char *) Realloc(cout, sLen += strlen(new) - 
             strlen(old), strlen(new) - strlen(old))) + (intptr_t) p, new);
      p   += strlen(tmp);
      str += strlen(old);
      }
    else
      *p++ = *str++;
  *p = 0;
  return cout;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// READ NUMBER FROM ARGS & CHECK BOUNDS
//
uint32_t ArgsNum(uint32_t d, char *a[], uint32_t n, char *s, uint32_t l,
uint32_t u){
  uint32_t x;
  for( ; --n ; ) if(!strcmp(s, a[n])){
    if((x = atol(a[n+1])) < l || x > u){
      fprintf(stderr, "[x] Invalid number! Interval: [%u;%u].\n", l, u);
      exit(EXIT_FAILURE);
      }
    return x;
    }
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// READ NUMBER FROM ARGS & CHECK BOUNDS
//
int64_t ArgsNumI64(int64_t d, char *a[], uint32_t n, char *s, int64_t l,
int64_t u){
  int64_t x;
  for( ; --n ; ) if(!strcmp(s, a[n])){
    if((x = atol(a[n+1])) < l || x > u){
      fprintf(stderr, "[x] Invalid number! Interval: [%"PRIi64";%"PRIi64"].\n", 
      l, u);
      exit(EXIT_FAILURE);
      }
    return x;
    }
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// READ NUMBER FROM ARGS & CHECK BOUNDS 64BITES
//
uint64_t ArgsNum64(uint64_t d, char *a[], uint32_t n, char *s, uint64_t l,
uint64_t u){
  uint64_t x;
  for( ; --n ; ) if(!strcmp(s, a[n])){
    if((x = atol(a[n+1])) < l || x > u){
      fprintf(stderr, "[x] Invalid number! Interval: [%"PRIu64";%"PRIu64"].\n", 
      l, u);
      exit(EXIT_FAILURE);
      }
    return x;
    }
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double ArgsDouble(double def, char *arg[], uint32_t n, char *str){
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return atof(arg[n+1]);
  return def;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t ArgsState(uint8_t def, char *arg[], uint32_t n, char *str){     
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return def == 0 ? 1 : 0;
  return def;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsString(char *def, char *arg[], uint32_t n, char *str){
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return arg[n+1];
  return def;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsFiles(char *arg[], uint32_t argc, char *str){
  int32_t n = argc;
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);
  return concatenate(concatenate(arg[argc-2], arg[argc-1]), ".svg");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FAccessWPerm(char *fn){
  if(access(fn, F_OK) != -1){
    fprintf(stderr, "Error: file %s already exists!\n", fn);
    if(access(fn, W_OK) != -1)
      fprintf(stderr, "Note: file %s has write permission.\nTip: to force "
      "writing rerun with \"-f\" option.\nWarning: on forcing, the old (%s) "
      "file will be deleted permanently.\n", fn, fn);
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void TestReadFile(char *fn){
  FILE *f = NULL;
  f = Fopen(fn, "r");
  fclose(f);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SFILES *ReadFNames(Param *P, char *arg){
  SFILES   *SF = (SFILES *) Calloc(1, sizeof(SFILES));
  uint32_t nFiles = 1, k = 0, argLen = strlen(arg);
  for( ; k != argLen ; ++k)
    if(arg[k] == ':')
      ++nFiles;
  SF->names = (char **) Malloc(nFiles * sizeof(char *));
  SF->names[0] = strtok(arg, ":");
  TestReadFile(SF->names[0]);
  for(k = 1 ; k < nFiles ; ++k){
    SF->names[k] = strtok(NULL, ":");
    TestReadFile(SF->names[k]);
    }
  SF->nFiles = nFiles;
  return SF;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void CalcProgress(uint64_t size, uint64_t i){
  if(size > PROGRESS_MIN && i % (size / 100) == 0)
    fprintf(stderr, "Progress:%3d %%\r", (uint8_t) (i / (size / 100)));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t CmpCheckSum(uint32_t cs, uint32_t checksum){
  if(checksum != cs){ 
    fprintf(stderr, "Error: invalid reference file!\n"
    "Compression reference checksum ................. %u\n"
    "Decompression reference checksum ............... %u\n",
    cs, checksum);
    fprintf(stderr, "Tip: rerun with correct reference file\n");
    return 1;
    }
  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintArgs(Param *P){
  uint32_t n;
  fprintf(stderr, "K-mer model:\n");
  fprintf(stderr, "  [+] K-mer ........................ %u\n", P[n].context);
  fprintf(stderr, "  [+] Use inversions ............... %s\n", !P[n].inverse? 
  "no" : "yes");
  if(P[n].context >= BLOOM_TABLE_BEGIN_CTX){
    fprintf(stderr, "  [+] Bloom array size ............. %"PRIu64"\n", 
    P[n].bSize);
    fprintf(stderr, "  [+] Bloom hashes number .......... %u\n", P[n].bHashes);
    }
  fprintf(stderr, "Sub-sampling ....................... %"PRIi64"\n", P->subsamp);
  if(P->window == -1)
    fprintf(stderr, "Window size ........................ Automatic\n");
  else 
    fprintf(stderr, "Window size ........................ %"PRIi64"\n", P->window);
  fprintf(stdout, "Reference files (%u):\n", P[0].ref->nFiles);
  for(n = 0 ; n < P[0].ref->nFiles ; ++n)
    fprintf(stderr, "  [+] Filename %-2u .................. %s\n", n+1,
    P[0].ref->names[n]);
  fprintf(stderr, "Target files (%u):\n", P[0].tar->nFiles);
  for(n = 0 ; n < P[0].tar->nFiles ; ++n)
    fprintf(stderr, "  [+] Filename %-2u .................. %s\n", n+1, 
    P[0].tar->names[n]);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
