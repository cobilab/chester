#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>

#include "model.h"

typedef struct{
  char     **names;
  uint32_t nFiles;
  }
SFILES;

typedef struct{
  uint8_t  verbose;
  char     *output;
  SFILES   *ref;
  SFILES   *tar;
  uint8_t  id;
  uint32_t nThreads;
  uint32_t context;
  uint32_t inverse;
  uint64_t **size;
  int64_t  subsamp;
  int64_t  window;
  uint64_t bSize;
  uint32_t bHashes;
  Model    *M;
  }
Param;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// CHESTER VALUES:
#define VERSION                1
#define RELEASE                1

// SYSTEM VALUES:
#define PROGRESS               1
#define BUFFER_SIZE            262144      
#define WINDOW_SIZE            262144      
#define PROGRESS_MIN           1000000
#define DEFAULT_HELP           0
#define DEFAULT_VERBOSE        0
#define DEFAULT_IR             0
#define DEFAULT_CTX            12
#define DEF_MIN_CTX            12
#define DEF_MAX_CTX            13
#define WINDOW_RATIO           1300
#define SUBSAMPLE_RATIO        5
#define DEFAULT_WINDOW         -1
#define DEFAULT_SUBSAMPLE      50000
#define DEFAULT_SAMPLE_RATIO   10000
#define MIN_CTX                1
#define MAX_CTX                30
#define BGUARD                 32
#define ALPHABET_SIZE          4
#define DEFAULT_BHASHES        7
#define DEFAULT_BSIZE          1<<30

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

