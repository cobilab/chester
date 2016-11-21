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

int ReadBit(BLOOM *B, uint64_t idx){
  return (B->array[idx>>3]>>(idx%8)) & 0x01;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SetBitTo1(BLOOM *B, uint64_t idx){
  B->array[idx>>3] |= (0x01<<(idx%8));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HFAM *CreateHFamily(uint32_t number_of_hashes, uint64_t p){
  int n;
  srand(0);
  HFAM *H = (HFAM *) Calloc(1, sizeof(HFAM));
  H->k = number_of_hashes;
  H->a = (uint64_t *)  Calloc(number_of_hashes, sizeof(uint64_t));
  H->b = (uint64_t *)  Calloc(number_of_hashes, sizeof(uint64_t));
  for(n = 0 ; n < number_of_hashes ; ++n){
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

void DeleteBloom(BLOOM *B){
  DeleteHFamily(B->H);
  Free(B->array);
  Free(B);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

BLOOM *CreateBloom(uint32_t k, uint64_t size){
  BLOOM *B = (BLOOM *) Calloc(1, sizeof(BLOOM));
  B->elem  = size;     // Number of bits
  B->size  = size>>3;  // Number of BYTES :: PACK 8 bits in Byte
  B->array = (BCC *) Calloc(B->size, sizeof(BCC));
  B->H     = CreateHFamily(k, 68719476735); // ~2^36 
                                 // GENERATES MAX 64 GB FIXME: INCREASE THIS!?
  return B;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t SearchBloom(BLOOM *B, uint64_t idx){
  uint32_t n;
  for(n = 0 ; n < B->H->k ; ++n)
    if(ReadBit(B, HashFunc(B->H, idx, n) % B->elem) == 0)
      return 0;
  return 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateBloom(BLOOM *B, uint64_t idx){
  uint32_t n;
  for(n = 0 ; n < B->H->k ; ++n)
    SetBitTo1(B, HashFunc(B->H, idx, n) % B->elem);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteModel(Model *M){
  DeleteBloom(M->bloom);
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Model *CreateModel(uint32_t kmer, uint32_t ir, uint32_t number_of_hashes, 
uint64_t size){
  Model *M = (Model *) Calloc(1, sizeof(Model));
  uint64_t prod = 1, *multipliers;
  uint32_t n;
  
  multipliers  = (uint64_t *) Calloc(kmer, sizeof(uint64_t));
  M->nPModels  = (uint64_t) pow(ALPHABET_SIZE, kmer);
  M->kmer      = kmer;
  M->idx       = 0;
  M->idxIR     = M->nPModels - 1;
  M->ir        = ir == 0 ? 0 : 1;

  M->bloom = CreateBloom(number_of_hashes, size);

  for(n = 0 ; n < M->kmer ; ++n){
    multipliers[n] = prod;
    prod <<= 2;
    }

  M->multiplier = multipliers[M->kmer-1];
  Free(multipliers);
  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetIdx(Model *M){
  M->idx   = 0;
  M->idxIR = M->nPModels - 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetIdxIR(uint8_t *p, Model *M){
  M->idxIR = (M->idxIR>>2)+GetCompNum(*p)*M->multiplier;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetIdx(uint8_t *p, Model *M){
  M->idx = ((M->idx-*(p-M->kmer)*M->multiplier)<<2)+*p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateIR(Model *M){
  UpdateBloom(M->bloom, M->idxIR);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Update(Model *M){
  UpdateBloom(M->bloom, M->idx);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

