
/*************************************************************************
**  NINA                                                                **
**************************************************************************
**  Copyright (c) 1998-2009                                             **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS **
**                                                                      **
**************************************************************************/

#include "libnina.h"

ParallelRegionsFile *head = NULL;
// logservice variables
bool isItTheLogServiceEnabled = true;
int logLineCount = 0;

void insertInList(char * name) {
	ParallelRegionsFile * new;
	new = malloc(sizeof(struct ParallelRegionsFile));
	new->name = malloc(BUFFER_SIZE * sizeof(char));
	memcpy(new->name, name, BUFFER_SIZE);
	new->hash = hashmapCreate(256);
	new->next = head;
	head = new;
	if(isItTheLogServiceEnabled){
		printf("%d_libnina->insertInList: %s\n",logLineCount, name);
		logLineCount++;
	}
}

ParallelRegionsFile * deleteFirst() {
	ParallelRegionsFile * tmpNode = head;
	head = head->next;
	if(isItTheLogServiceEnabled){
		printf("%d_libnina->deleteFirst\n",logLineCount);
		logLineCount++;
	}
	return tmpNode;
}

int isListEmpty() {
	return (head == NULL);
}

ParallelRegionsFile * find(char * name) {
	ParallelRegionsFile * current = malloc(sizeof(ParallelRegionsFile));
	current = head;
	if(head == NULL) {
		return NULL;
	}
	while(strcmp(current->name, name) !=0) {
		if(current->next == NULL) {
			return NULL;
		} else {
			current = current->next;
		}
	}
	if(isItTheLogServiceEnabled){
		printf("%d_libnina->find: %s\n",logLineCount, name);
		logLineCount++;
	}
	return current;
}

void freeMemoryData() {
	if(isListEmpty){
		return;
	}
	ParallelRegionsFile * current = NULL;
	while(head != NULL) {
		hashmapDelete(head->hash);
		current = head;
		head = head->next;
		free(current);
	}
	if(isItTheLogServiceEnabled){
		printf("%d_libnina->freeMemoryData\n", logLineCount);
		logLineCount++;
	}
}

void printList() {
	ParallelRegionsFile * tmp = head;
	while(tmp!=NULL) {
		printf("---------------------------\n");
		printf("File name node %s\n", tmp->name);
		printf("Count of Hash %ld\n",hashmapCount(tmp->hash));
		printf("---------------------------\n\n");
		tmp = tmp->next;
	}
}

int getFileSize(char *filePath) {
	FILE *arq;
	int ch, lines = 0;
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
	return lines;
}


void readerOfConfigurationFile(){
	if(getenv("NINA_CONFIG") == NULL) {
		printf("%d_libnina->find: The environment variable NINA_CONFIG is NULL\n",
			logLineCount);
		logLineCount++;
	} else {
		FILE *arq;
		char *filePath = malloc(BUFFER_LINE * sizeof(char));
		filePath = getenv("NINA_CONFIG");
		arq = fopen(filePath, "r");
		char *fileNameTmp = malloc(BUFFER_SIZE * sizeof(char));
		int start_lineTmp = 0;
		long int freqTmp = 0;
		int	amountOfLines = getFileSize(filePath);
		int count = 0;
		ParallelRegionsFile *current;
		while(count < amountOfLines) {
			fscanf(arq,"%d,%ld,%s\n",&start_lineTmp,&freqTmp,fileNameTmp);
			current = find(fileNameTmp);
			if(current==NULL) {
				insertInList(fileNameTmp);
				hashmapInsert(head->hash, &freqTmp, start_lineTmp);
			} else {
				hashmapInsert(current->hash, &freqTmp, start_lineTmp);
			}
			count++;
		}
		free(filePath);
		free(fileNameTmp);
		free(current);
		fclose(arq);
	}
}

void changeProcessorsFrequency(long int freq) {
	int cpufreqReturned = -1;
	int count = 0;
	#pragma omp parallel for schedule(static) private(count)
	for(count = 0; count < AMOUNT_OF_CPUS; count++){
		cpufreqReturned = cpufreq_set_frequency(count, freq);
		if(cpufreqReturned < 0){
			printf("%d_libnina->changeProcessorsFrequency: problems...", logLineCount);
			exit(0);
		}
	}
}

void callByNINALibrary(char *file, unsigned long start_line) {
	ParallelRegionsFile * tmp = find(file);
	if(tmp != NULL){
		long newFrequency = hashmapGet(tmp->hash, start_line);
		changeProcessorsFrequency(newFrequency);
	}
}

void changeProcessorsFrequencyToMax() {
	changeProcessorsFrequency(MAX_FREQUENCY);
}
