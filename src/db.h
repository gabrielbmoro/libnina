/*
    This file is part of libnina

    libnina is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libnina is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with libnina. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __DB_H
#define __DB_H
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>
#include <string.h>
#include <stdbool.h>

extern bool logEnabled;

#define INIT_HASH_SIZE 256
#define STR_HASH_KEY_SIZE 200

struct NINARegion {
  struct hsearch_data htab;
  char *source_filename;
} NINARegion;

void LIBNINA_RegionsFree(void);
void LIBNINA_LoadRegionsFile(void);
long LIBNINA_GetFrequency(char *source_filename, int start_line);

#include "libnina.h"
#endif
