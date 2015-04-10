#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include "defs.h"

#define ARRAY_MODE             0
#define BLOOM_MODE             1
#define BLOOM_TABLE_BEGIN_CTX  17

typedef uint8_t ACC;                        // Size of context counters for arrays
typedef uint8_t BCC;                        // Size of context counters for arrays

typedef struct{
  uint32_t k;        // NUMBER OF HASHES 
  uint64_t *a;       // A SEED
  uint64_t *b;       // B SEED
  uint64_t p;        // PRIME
  }
HFAM;

typedef struct{
  ACC      *states;
  }
ARRAY;

typedef struct{
  uint8_t  *array;
  uint64_t size;
  uint32_t ctx;
  HFAM     *H;
  }
BLOOM;

typedef struct{
  uint32_t ctx;                          // Current depth of context template
  uint64_t nPModels;                  // Maximum number of probability models
  uint64_t multiplier;
  uint64_t idx;
  uint64_t idxIR;
  uint8_t  ir;
  ARRAY    array;
  BLOOM    *bloom;
  uint8_t  mode;
  }
Model;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HFAM        *CreateHFamily   (uint32_t, uint64_t);
void        DeleteHFamily    (HFAM *);
uint64_t    Hash             (HFAM *, uint64_t, uint32_t);
inline void GetIdx           (uint8_t *, Model *);
inline void GetIdxIR         (uint8_t *, Model *);
BLOOM       *CreateBloom     (uint32_t k, uint64_t s);
void        DeleteBloom      (BLOOM *B);
uint8_t     SearchBloom      (BLOOM *B, uint64_t i);
void        UpdateBloom      (BLOOM *B, uint64_t i);
Model       *CreateModel     (uint32_t, uint32_t, uint32_t, uint64_t);
void        DeleteModel      (Model *);
void        ResetModelIdx    (Model *);
void        ResetIdx         (Model *);
void        UpdateIR         (Model *);
void        Update           (Model *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
