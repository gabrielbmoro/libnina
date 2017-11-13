
/****************************************************************************
**  NINA                                                             **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS    **
**                                                                         **
****************************************************************************/

#include "libnina.h"

/*
 * Variáveis globais
 */
int 	isLogServiceEnabled 	= 	0; //1 com log, 0 sem log


NINA_CsvFile * hashMapCreate(int size) {
	
	NINA_CsvFile * hashmap = NULL;

	int i;

	if(size < 1) return NULL;

	if((hashmap = malloc(sizeof(NINA_CsvFile))) == NULL) {
		return NULL;
	}

	if((hashmap->bufferOfLines = malloc(sizeof(NINA_CsvLine *) * size)) == NULL) {
		return NULL;
	}

	for(i = 0; i < size; i++) {
		hashmap->bufferOfLines[i] = NULL;
	}

	hashmap->amountOfLines = size;

	return hashmap;
}



NINA_CsvLine * getValue(NINA_CsvFile *hashmap, char *region, char *file, int start_line) {

	NINA_CsvLine * tmp = hashmap->bufferOfLines[0];

	while(tmp == NULL || tmp->regionName == NULL || (strcmp(region, tmp->regionName) !=0)) {

		if(start_line == tmp->line_start && strstr(file, tmp->fileName) !=NULL && strcmp(region,tmp->regionName)==0) {
			break;
		}

		if(tmp->next == NULL){
			break;	
		}

		tmp = tmp->next;

	}

	if(tmp == NULL || tmp->regionName == NULL){
		return NULL;
	} else {
		return tmp;
	}
}

NINA_CsvLine * createNewPair(long int freq, int line_start, char * regionName, char * fileName) {
	
	NINA_CsvLine *newPair;
	
	newPair = malloc(sizeof(NINA_CsvLine));

    newPair->freq = freq;
    newPair->line_start = line_start;
    newPair->regionName = malloc(sizeof(char) * BUFFER_REGION);
    newPair->fileName = malloc(sizeof(char) * BUFFER_LINE);

    memcpy(newPair->regionName,regionName,BUFFER_REGION);
	memcpy(newPair->fileName,fileName,BUFFER_LINE);

	newPair->next = NULL;

	return newPair;

}

void NINA_CHANGEFREQ(long int freq) {

	int cpufreqReturned = -1;

	char *time = (char *) malloc(BUFFER_SIZE * sizeof(char));

	int count = 0;

	#pragma omp parallel for schedule(static) private(count)
	for(count = 0; count < AMOUNT_OF_CPUS; count++){
		cpufreqReturned = cpufreq_set_frequency(count, freq);

		if(cpufreqReturned < 0){
			NINA_GET_TIME(time);
			printf("%s -> NINA_CHANGEFREQ: it is impossible to change the CPU frequency. It's necessary the userspace governor...\n", time);
			free(time);
			exit(0);
		} else {
			if(isLogServiceEnabled){		
			      	NINA_GET_TIME(time);
				printf("%s: ->NINA_CHANGEFREQ: %d freq changed is %d \n",time,count,cpufreqReturned);
			}
		}
	}
	
	free(time);
	
}

void * NINA_GET_TIME(char * strReturned) {

	time_t t =  time(NULL);
	struct tm tm = *localtime(&t);

	sprintf(strReturned, "%.2d.%.2d.%.4d.%.2dh%.2dm%.2ds",tm.tm_mday,tm.tm_mon + 1, 
		tm.tm_year + 1900, tm.tm_hour,tm.tm_min,tm.tm_sec);

}

void NINA_CALL(char *region, char *file, int start_line) {
	///////////////////////////////////////////////////////////////////////////
	// Aqui vai acontecer a magia, primeiramente vou verificar se o region id
	//  é de uma região memory-bound, depois disso, vou recuperar o número da
	//  linha pela estrutura my_region_info disponibilizada pela ferramenta opari2.
	//  O primeiro teste que vou fazer é imprimir informações sobre a região alvo.

	char *time = (char *) malloc(BUFFER_SIZE * sizeof(char));

	if(isLogServiceEnabled){

		NINA_GET_TIME(time);

	}

	NINA_CsvLine * tmp = getValue(CsvFile, region, file, start_line);

	if(tmp != NULL){


	  if(isLogServiceEnabled){
	    	printf("%s: ->NINA_CALL: I was called... at %s -> %s:%d\n",
			time,
			tmp->regionName,
			tmp->fileName,
			tmp->line_start);

	  }

		NINA_CHANGEFREQ(tmp->freq);	

	}

	free(time);
}

int NINA_GetSizeOfFile(char *filePath) {

	FILE *arq;

	int ch, lines = 0;
	
	char *time = (char *) malloc(BUFFER_SIZE * sizeof(char));


	arq = fopen(filePath, "r");

	do {
		ch = fgetc(arq);
		if(ch == '\n')
			lines++;
	}while(ch != EOF);

	if(ch != '\n' && lines != 0) {
		lines++;
	}

	fclose(arq);


	if(isLogServiceEnabled){
		NINA_GET_TIME(time);

		printf("%s: ->NINA_GetSizeOfFile: NINA_Amount of lines in file: %d\n", time, lines);
	}

	free(time);

	return lines;
}


void NINA_maxFrequencyOfProcessor() {

	char *time = (char *) malloc(BUFFER_SIZE * sizeof(char));
	
	if(isLogServiceEnabled){
		NINA_GET_TIME(time);
		
		printf("%s: ->NINA_maxFrequencyOfProcessor: The frequency will be set to max\n",time);
	}

	NINA_CHANGEFREQ(MAX_FREQUENCY);

	free(time);

}

void NINA_CsvFileReader() {
	
	char *time = (char *) malloc(BUFFER_SIZE * sizeof(char));

	if(isLogServiceEnabled){
		NINA_GET_TIME(time);

		printf("%s: ->NINA_CsvFileReader: csvFileReader -> begin ...\n",time);
	}

	if(getenv("NINA_CONFIG") == NULL) {

		if(isLogServiceEnabled){
			NINA_GET_TIME(time);
			
			printf(
				"%s: ->NINA_CsvFileReader: The environment variable called NINA_CONFIG is as NULL...\n",time
				);
		}

	} else {

		char regionNameTmp[BUFFER_REGION];
		
		char fileNameTmp[BUFFER_SIZE];
		
		char *filePath = malloc(BUFFER_LINE * sizeof(char));

		FILE *arq;

		filePath = getenv("NINA_CONFIG");

		arq = fopen(filePath, "r");

		if(isLogServiceEnabled) {

			NINA_GET_TIME(time);

			printf("%s: ->NINA_CsvFileReader: The environment variable called NINA_CONFIG is defined...\n",	time);
			
			printf("%s: ->NINA_CsvFileReader: The environment variable called NINA_CONFIG is %s...\n",time, filePath);

			NINA_GET_TIME(time);

			printf("%s: ->NINA_CsvFileReader: csvFileReader -> The file was readed %d...\n",time, arq == NULL);

		}

		int amountOfLines = NINA_GetSizeOfFile(filePath);

		CsvFile = hashMapCreate(amountOfLines);
		
		CsvFile->amountOfLines = amountOfLines;

		int count = 0;

		long int freqTmp = 0;

		int start_lineTmp = 0;
		
		NINA_CsvLine * tmp;                

		while(count < CsvFile->amountOfLines) {

            fscanf(arq, "%d,%ld,%s,%s\n",&start_lineTmp,&freqTmp,&regionNameTmp,&fileNameTmp);

            tmp = createNewPair(freqTmp,start_lineTmp,regionNameTmp,fileNameTmp);
			
			CsvFile->bufferOfLines[count] = tmp;
			
			if(isLogServiceEnabled) {
				NINA_GET_TIME(time);

				printf("%s: ->NINA_CsvFileReader: l%d regionName %s, fileName %s, start_line %d, freq_tmp %ld...\n", time, count, 
					tmp->regionName, tmp->fileName, tmp->line_start, tmp->freq);
			}
			
			if(count > 0) {
				CsvFile->bufferOfLines[count-1]->next = tmp;
			}

			count++;
		}

		
		if(isLogServiceEnabled) {
			NINA_GET_TIME(time);

			printf("%s: ->NINA_CsvFileReader: csvFileReader -> end...\n",time);
		}

		fclose(arq);

	}
	
	free(time);
}

