/*************************************************************************
**  NINA                                                                **
**************************************************************************
**  Copyright (c) 1998-2009                                             **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS **
**                                                                      **
**************************************************************************/

#ifndef LIB_NINA_H
#define LIB_NINA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <cpufreq.h>
#include <time.h>
#include <stdbool.h>
#include "hashmap.h"
#include "pomp2_user_lib.h"
#include "db.h"

#define BUFFER_SIZE 1024
#define BUFFER_LINE 200
#define INIT_HASH_SIZE 256

#define LOG(COMMAND) if(logEnabled){ printf("%d: ", omp_get_thread_num()); COMMAND; };
extern bool logEnabled;

void LIBNINA_InitLibrary();
void LIBNINA_ParallelBegin(char *file, long start_line);
void LIBNINA_ParallelEnd();

#endif				// LIB_NINA_H
