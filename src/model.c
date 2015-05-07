#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "model.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HFAM *CreateHFamily(uint32_t k, uint64_t p){
  int n;
  srand(0);
  HFAM *H = (HFAM *) Calloc(1, sizeof(HFAM));
  H->k = k;
  H->a = (uint64_t *)  Calloc(k, sizeof(uint64_t));
  H->b = (uint64_t *)  Calloc(k, sizeof(uint64_t));
  for(n = 0 ; n < k ; ++n){
    H->a[n] = rand() % UINT64_MAX;
    H->b[n] = rand() % UINT64_MAX;
    }
  H->p = p;
  return H;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteHFamily(HFAM *H){
  Free(H->a);
  Free(H->b);
  Free(H);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t HashFunc(HFAM *H, uint64_t i, uint32_t n){
  return (H->a[n] * i + H->b[n]) % H->p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InitArray(Model *M){
  M->array.states = (ACC *) Calloc(M->nPModels, sizeof(ACC));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteBloom(BLOOM *B){
  DeleteHFamily(B->H);
  Free(B->array);
  Free(B);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

BLOOM *CreateBloom(uint32_t k, uint64_t size){
  BLOOM *B = (BLOOM *) Calloc(1, sizeof(BLOOM));
  B->array = (BCC *) Calloc(size, sizeof(BCC));
  B->H     = CreateHFamily(k, 68719476735); // ~2^36 // GENERATES MAX 64 GB
  B->size  = size;
  return B;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t SearchBloom(BLOOM *B, uint64_t i){
  uint32_t n;
  for(n = 0 ; n < B->H->k ; ++n)
    if(B->array[HashFunc(B->H, i, n) % B->size] == 0)
      return 0;
  return 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateBloom(BLOOM *B, uint64_t i){
  uint32_t n;
  for(n = 0 ; n < B->H->k ; ++n)
    B->array[HashFunc(B->H, i, n) % B->size] = 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InitHashTable(Hash *H){
  H->keys      = (KEYSMAX **) Calloc(HASH_SIZE, sizeof(KEYSMAX *));
  H->entrySize = (ENTMAX   *) Calloc(HASH_SIZE, sizeof(ENTMAX   ));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Hash *CreateHash(void){
  Hash *H = (Hash *) Calloc(1, sizeof(Hash));
  InitHashTable(H);
  return H;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InsertKey(Hash *hash, uint32_t h, uint64_t key){
  hash->keys[h] = (KEYSMAX *) Realloc(hash->keys[h], (hash->entrySize[h] + 1)
  * sizeof(KEYSMAX), sizeof(KEYSMAX));
  hash->keys[h][hash->entrySize[h]] = (uint32_t) (key / HASH_SIZE);
  hash->entrySize[h]++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateHash(Hash *hModel, uint64_t idx){
  uint32_t n, h = idx % HASH_SIZE;                                // The hash index
  for(n = 0 ; n < hModel->entrySize[h] ; n++)
    if(((uint64_t) hModel->keys[h][n] * HASH_SIZE) + h == idx)
      return;
  InsertKey(hModel, h, idx);                           // If key not found
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteHash(Hash *H){
  uint32_t k;
  for(k = 0 ; k < HASH_SIZE ; ++k)
    if(H->entrySize[k] != 0)
      Free(H->keys[k]);
  Free(H->keys);
  Free(H->entrySize);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteModel(Model *M){
  switch(M->mode){
    case BLOOM_MODE:       DeleteBloom(M->bloom);  break;
    case HASH_TABLE_MODE:  DeleteHash(M->hash);    break;
    default:               Free(M->array.states);  break;
    }
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Model *CreateModel(uint32_t ctx, uint32_t ir, uint32_t bh, uint64_t bs, 
uint8_t x){
  Model *M = (Model *) Calloc(1, sizeof(Model));
  uint64_t prod = 1, *multipliers;
  uint32_t n;
  
  multipliers  = (uint64_t *) Calloc(ctx, sizeof(uint64_t));
  M->nPModels  = (uint64_t) pow(ALPHABET_SIZE, ctx);
  M->ctx       = ctx;
  M->idx       = 0;
  M->idxIR     = M->nPModels - 1;
  M->ir        = ir == 0 ? 0 : 1;

  if(ctx >= BLOOM_TABLE_BEGIN_CTX){
    if(x == 1){
      M->mode  = BLOOM_MODE;
      M->bloom = CreateBloom(bh, bs);
      }
    else{
      if(ctx > MAX_HASH_CTX){
        fprintf(stderr, "Error: context is greater than %d!\n", MAX_HASH_CTX);
        exit(1);
        }
      M->mode = HASH_TABLE_MODE;
      M->hash = CreateHash();
      }
    }
  else{
    M->mode = ARRAY_MODE;
    InitArray(M);
    }
  
  for(n = 0 ; n < M->ctx ; ++n){
    multipliers[n] = prod;
    prod <<= 2;
    }

  M->multiplier = multipliers[M->ctx-1];
  Free(multipliers);
  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetIdx(Model *M){
  M->idx   = 0;
  M->idxIR = M->nPModels - 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void GetIdxIR(uint8_t *p, Model *M){
  M->idxIR = (M->idxIR>>2)+GetCompNum(*p)*M->multiplier;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void GetIdx(uint8_t *p, Model *M){
  M->idx = ((M->idx-*(p-M->ctx)*M->multiplier)<<2)+*p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateIR(Model *M){
  switch(M->mode){
    case BLOOM_MODE:       UpdateBloom(M->bloom, M->idxIR);  break;
    case HASH_TABLE_MODE:  UpdateHash(M->hash, M->idxIR);    break;
    default:               M->array.states[M->idxIR] = 1;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Update(Model *M){
  switch(M->mode){
    case BLOOM_MODE:       UpdateBloom(M->bloom, M->idx);  break;
    case HASH_TABLE_MODE:  UpdateHash(M->hash, M->idx);    break;
    default:               M->array.states[M->idx] = 1;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

