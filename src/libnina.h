/*************************************************************************
**  NINA                                                                **
**************************************************************************
**  Copyright (c) 1998-2009                                             **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS **
**                                                                      **
**************************************************************************/

#ifndef NINA_H
#define NINA_H

  #include <stdlib.h>
  #include <stdio.h>
  #include <stdbool.h>
  #include <string.h>
  #include <cpufreq.h>
  #include "hashmap.h"

  #define MAX_FREQUENCY 2340000
  #define BUFFER_SIZE 1024
  #define BUFFER_LINE 200
  #define INIT_HASH_SIZE 256
  #define AMOUNT_OF_CPUS 40

  typedef struct ParallelRegionsFile {
    char *name;
    s_hashmap *hash;
    struct ParallelRegionsFile *next;
  } ParallelRegionsFile;

  void insertInList(char * name);

  ParallelRegionsFile * deleteFirst();

  int isListEmpty();

  ParallelRegionsFile * find(char * name);

  void freeMemoryData();

  void printList();

  int getFileSize(char *filePath);

  void readerOfConfigurationFile();

  void changeProcessorsFrequency(long int freq);

  void callByNINALibrary(char *file, unsigned long start_line);

  void changeProcessorsFrequencyToMax();

#endif // NINA_H
