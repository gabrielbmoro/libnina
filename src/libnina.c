
/*************************************************************************
**  NINA                                                                **
**************************************************************************
**  Copyright (c) 1998-2009                                             **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS **
**                                                                      **
**************************************************************************/

#include "libnina.h"

ParallelRegionsFile   *head                     = NULL;
int                   isItTheLogServiceEnabled  = 0;
int                   amountOfCPUS              = 4;
long                  maxFrequency              = 1200000;

void insertInList(char * name, int hashSize) {
	ParallelRegionsFile * new;
	new = malloc(sizeof(struct ParallelRegionsFile));
	new->name = malloc(BUFFER_SIZE * sizeof(char));

	memcpy(new->name, name, BUFFER_SIZE);

	new->hash = hashmapCreate(hashSize);
	new->next = head;
	head = new;

	if(isItTheLogServiceEnabled)
		printf("libnina->insertInList: %s\n", name);

}

ParallelRegionsFile * deleteFirst() {

	ParallelRegionsFile * tmpNode = head;
	head = head->next;

	if(isItTheLogServiceEnabled)
		printf("libnina->deleteFirst\n");

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
	while(strcmp(current->name, name) < 0) {

		if(current->next == NULL) {
			return NULL;
		} else {
			current = current->next;
		}

	}

	if(isItTheLogServiceEnabled)
		printf("libnina->find: %s\n", name);

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

	if(isItTheLogServiceEnabled)
		printf("%d_libnina->freeMemoryData\n");

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

int randomNumberBetweenMinMax(int min, int max){
	srand((unsigned) time(NULL));
	if((max-min) != 0)
		return (min + (rand() % (max-min)));
	else
		return (rand() % max);
}

void changeProcessorsFrequency(long freq) {
	int cpufreqReturned = -1;
	int count = 0;

	#pragma omp parallel for schedule(static) private(count)
	for(count = 0; count < amountOfCPUS; count++) {
		cpufreqReturned = cpufreq_set_frequency(count, freq);

		if(cpufreqReturned < 0){
				printf("libnina->changeProcessorsFrequency: libnina need userspace governor to change the processor's frequency\n");
				exit(0);
		}

		if(isItTheLogServiceEnabled)
			printf("libnina->changeProcessorsFrequency: CPU%d to freq %ld, -> %d\n", count, freq, cpufreqReturned);
	}

}

void changeRandomProcessorsFrequency(long freq) {
	int cpufreqReturned = -1;
	int firstCPU = 0;
	int dividedBy = 2;
	int minValue = 0;
	int maxValue = amountOfCPUS;
	int random = 0;

	while ((minValue = maxValue / dividedBy) > 0) {

		random = randomNumberBetweenMinMax(minValue, maxValue);

		cpufreqReturned = cpufreq_set_frequency(random, freq);

		if(cpufreqReturned < 0){
				printf("libnina->changeProcessorsFrequency: libnina need userspace governor to change the processor's frequency\n");
				exit(0);
		}

		if(isItTheLogServiceEnabled)
			printf("libnina->changeProcessorsFrequency: CPU%d to freq %ld, -> %d\n", random, freq, cpufreqReturned);

		dividedBy += 2;
		maxValue -= dividedBy;
	}
}

void callByNINALibrary(char *file, long start_line) {

	ParallelRegionsFile * tmp = find(file);

	if(isItTheLogServiceEnabled)
		printf("libnina->callByNINALibrary: file %s at %ld\n", file, start_line);

	if(tmp != NULL){
		long newFrequency = hashmapGet(tmp->hash, start_line);
		changeProcessorsFrequency(newFrequency);
	}

}

void changeProcessorsFrequencyToMax() {
	changeProcessorsFrequency(maxFrequency);
}

void initLibrary(){
	if(getenv("NINA_CONFIG") == NULL || getenv("NINA_MAX_FREQUENCY") == NULL || getenv("NINA_AMOUNT_OF_CPUS") == NULL) {

		printf("libnina->initLibrary: It is necessary to define the environment variables NINA_CONFIG, NINA_MAX_FREQUENCY, and NINA_AMOUNT_OF_CPUS... \n");

	} else {
		amountOfCPUS = atoi(getenv("NINA_AMOUNT_OF_CPUS"));

		maxFrequency = atol(getenv("NINA_MAX_FREQUENCY"));

		isItTheLogServiceEnabled = (getenv("NINA_LOG") != NULL);

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

				insertInList(fileNameTmp, amountOfLines);

				hashmapInsert(head->hash, freqTmp, start_lineTmp);

			} else {

				hashmapInsert(current->hash, freqTmp, start_lineTmp);

			}

			count++;

		}

		free(fileNameTmp);

		fclose(arq);

	}
}
