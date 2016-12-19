#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "model.h"
#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define MAX_STR  4096

FILE        *Fopen           (const char *, const char *);
void        FillLogTable     (uint32_t, uint32_t, uint32_t);
double      SearchLog        (uint32_t );
double      Power            (double, double);
uint32_t    FLog2            (uint64_t );
uint8_t     S2N              (uint8_t  );
uint8_t     N2S              (uint8_t  );
uint8_t     GetCompNum       (uint8_t  );
uint8_t     GetCompSym       (uint8_t  );
uint64_t    NDNASyminFile    (FILE *);
uint64_t    EntriesInFile    (FILE *, uint32_t);
uint64_t    NBytesInFile     (FILE *);
uint64_t    FopenBytesInFile (const char *);
uint8_t     *ReverseStr      (uint8_t *, uint32_t);
char        *CloneString     (char *   );
char        *RepString       (const char *, const char *, const char *);
char        *ReplaceSubStr   (char *, char *, char *);
uint32_t    ArgsNum          (uint32_t , char *[], uint32_t, char *, uint32_t, 
                             uint32_t);
int64_t     ArgsNumI64       (int64_t, char *[], uint32_t, char *, int64_t,
                             int64_t);
uint64_t    ArgsNum64        (uint64_t , char *[], uint32_t, char *, uint64_t,
                             uint64_t);
double      ArgsDouble       (double, char *[], uint32_t, char *);
uint8_t     ArgsState        (uint8_t  , char *[], uint32_t, char *);
char        *ArgsString      (char    *, char *[], uint32_t, char *);
char        *ArgsFiles       (char *[], uint32_t, char *);
void        TestReadFile     (char *);
uint8_t     CmpCheckSum      (uint32_t, uint32_t);
void        FAccessWPerm     (char    *);
SFILES      *ReadFNames      (Param *, char *);
void        CalcProgress     (uint64_t , uint64_t);
void        PrintArgs        (Param *);
char        *concatenate     (char *   , char *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
