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

#define BUFFER_SIZE 1024
#define BUFFER_LINE 200
#define INIT_HASH_SIZE 256

#define LOG(COMMAND) if(logEnabled){ printf("%d: ", omp_get_thread_num()); COMMAND; };
extern bool logEnabled;

typedef struct ParallelRegionsFile {
  char *name;
  s_hashmap *hash;
  struct ParallelRegionsFile *next;
} ParallelRegionsFile;

void insertInList(char *name, int hashSize);

ParallelRegionsFile *deleteFirst();

int isListEmpty();

ParallelRegionsFile *find(char *name);

void freeMemoryData();

void printList();

int getFileSize(char *filePath);

int randomNumberBetweenMinMax(int min, int max);

void initLibrary();

void changeProcessorsFrequency(long freq);

void callByNINALibrary(char *file, long start_line);

void changeProcessorsFrequencyToMax();

#endif				// LIB_NINA_H
