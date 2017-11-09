/****************************************************************************
**  NINA                                                             **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Inform�tica da UFRGS    **
**                                                                         **
****************************************************************************/

#ifndef NINA_H
#define NINA_H

    ///////////////////////////////////////////////////////////////////////////
    // Importa��o de depend�ncias
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
	#include <time.h>
	#include <cpufreq.h>

    #define MAX_FREQUENCY 2340
    #define BUFFER_SIZE 1024
    #define BUFFER_REGION 3
    #define BUFFER_LINE 200
    #define AMOUNT_OF_CPUS 4

    /*
     * Nome: NINA_CsvLine
     *
     * Author: Gabriel B Moro
     *
     * Descri��o: Estrutura de dados respons�vel por
     * representar a linha do csv de configura��o.
     */
    typedef struct NINA_CsvLine {
        long int 			   	freq;
        int 				   	line_start;
        char 				   	*regionName; //key
        char 				   	*fileName;
        struct NINA_CsvLine 	*next;
    } NINA_CsvLine;


    /*
     * Nome: NINA_CsvFile
     *
     * Author: Gabriel B Moro
     *
     * Descri��o: Estrutura de dados respons�vel por
     * representar o arquivo csv de configura��o.
     */
    typedef struct NINA_CsvFile {
        int 					amountOfLines;
        NINA_CsvLine 			**bufferOfLines;
    } NINA_CsvFile;



    NINA_CsvFile * CsvFile;

	NINA_CsvFile * hashMapCreate(int size);

	NINA_CsvLine * getValue(NINA_CsvFile *hashmap, char *region, char *file, int start_line);

	NINA_CsvLine * createNewPair(long int freq, int line_start, char * regionName, char * fileName);

    /*
     * Nome: NINA_CHANGEFREQ
     *
     * Author: Gabriel B Moro
     *
     * Descri��o: Este m�todo ser� chamado no pomp2_lib.c
     * em todos os m�todos, a fim de que, pela primeira
     * execu��o seja poss�vel instrumentar adequadamente
     * o c�digo alvo.
     */
    void NINA_CHANGEFREQ(long int freq);

    void * NINA_GET_TIME(char * strReturned);

	void NINA_CALL(char *region, char *file, int start_line);

    /*
     * Nome: NINA_GetSizeOfFile
     *
     * Author: Gabriel B Moro
     *
     * Descri��o: Fun��o respons�vel por calcular o
     * tamanho do arquivo em quantidade de linhas.
     */
    int NINA_GetSizeOfFile(char *filePath);

    /*
     * Nome: NINA_GetSizeOfFile
     *
     * Author: Gabriel B Moro
     *
     * Descri��o: Esta fun��o deve ser chamada no
     * POMP_INIT para levantar a frequ�ncia de
     * processador no m�ximo.
     */
    void NINA_maxFrequencyOfProcessor();

    void * NINA_adjustString(char *fileNameTmp, char * strReturned);

    /*
     * Nome: NINA_CsvFileReader
     *
     * Author: Gabriel B Moro
     *
     * Descri��o: Esta fun��o l� o csv e alimenta a
     * estrutura de dados NINA_CsvFile.
     */
    void NINA_CsvFileReader();

#endif // NINA_H