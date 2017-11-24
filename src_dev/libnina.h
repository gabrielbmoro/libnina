/****************************************************************************
**  NINA                                                             **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS    **
**                                                                         **
****************************************************************************/

#ifndef NINA_H
#define NINA_H

    ///////////////////////////////////////////////////////////////////////////
    // Importação de dependências
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <time.h>
    #include "hashmap.h"
    //#include <cpufreq.h>

    #define MAX_FREQUENCY 2340
    #define BUFFER_SIZE 1024
    #define BUFFER_REGION 3
    #define BUFFER_LINE 200
    #define INIT_HASH_SIZE 256
    #define AMOUNT_OF_CPUS 4
    
    typedef struct ParallelRegionsFile {
        char                  *name;
        s_hashmap             *hash; //line is the key and frequency is the value
        struct ParallelRegionsFile   *next;
    } ParallelRegionsFile;

    void insertInList(char * name);

    ParallelRegionsFile * deleteFirst();

    int isListEmpty();

    ParallelRegionsFile * find(char * name);

    void freeMemoryData();

    void printList();

    int getFileSize(char *filePath);

    void readerOfConfigurationFile();


#endif // NINA_H
