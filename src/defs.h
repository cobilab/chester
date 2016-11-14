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
  uint8_t  disk;
  char     *output;
  SFILES   *ref;
  SFILES   *tar;
  uint32_t kmer;
  uint32_t inverse;
  uint64_t **size;
  uint64_t *chrSize;
  int64_t  subsamp;
  int64_t  window;
  int64_t  ratio;
  uint64_t bSize;
  uint32_t bHashes;
  uint64_t max;
  double   threshold;
  Model    *M;
  }
Param;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// CHESTER VALUES:
#define VERSION                2
#define RELEASE                1

// SYSTEM VALUES:
#define PROGRESS               1
#define BUFFER_SIZE            262144      
#define WINDOW_SIZE            262144      
#define PROGRESS_MIN           1000000
#define DEFAULT_HELP           0
#define DEFAULT_DISK           1
#define DEFAULT_VERBOSE        0
#define DEFAULT_IR             0
#define DEFAULT_KMER           12
#define WINDOW_RATIO           1300
#define SUBSAMPLE_RATIO        5
#define DEFAULT_THRESHOLD      0.5 
#define DEFAULT_WINDOW         -1
#define DEFAULT_SUBSAMPLE      0
#define DEFAULT_SAMPLE_RATIO   10000
#define MIN_KMER               1
#define MAX_KMER               30
#define BGUARD                 32
#define ALPHABET_SIZE          4
#define HASH_SIZE              268435459      // first PRIME NUMBER after 2^28
#define DEFAULT_BHASHES        7
#define DEFAULT_BSIZE          1<<30
#define EXTRA_CHAR_CODE        7

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

