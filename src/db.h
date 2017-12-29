#ifndef __DB_H
#define __DB_H
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>
#include <string.h>

#define INIT_HASH_SIZE 256
#define STR_HASH_KEY_SIZE 200

struct NINARegion {
  struct hsearch_data htab;
  char *source_filename;
} NINARegion;

void LIBNINA_RegionsFree (void);
void LIBNINA_LoadRegionsFile (void);
long LIBNINA_GetFrequency (char *source_filename, int start_line);

#include "libnina.h"
#endif
