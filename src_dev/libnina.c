
/****************************************************************************
**  NINA                                                             **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS    **
**                                                                         **
****************************************************************************/

#include "libnina.h"

ParallelRegionsFile       *head                   = NULL;

void insertInList(char * name) {

	ParallelRegionsFile * new;

	new = malloc(sizeof(struct ParallelRegionsFile));

	new->name = malloc(BUFFER_SIZE * sizeof(char));

	memcpy(new->name, name, BUFFER_SIZE);

	new->hash = hashmapCreate(256);

	new->next = head;

	head = new;

}

ParallelRegionsFile * deleteFirst() {

	ParallelRegionsFile * tmpNode = head;

	head = head->next;

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

	FILE *arq;

	char *filePath = malloc(BUFFER_LINE * sizeof(char));

	filePath = "/home/gbmoro/svn/libnina/src_dev/ninaFileInput.csv";

	arq = fopen(filePath, "r");

	char *fileNameTmp = malloc(BUFFER_SIZE * sizeof(char));
	
	int start_lineTmp = 0;
	
	long int freqTmp = 0;

	ParallelRegionsFile *current;
	
	int	amountOfLines = getFileSize(filePath);

	int count = 0;

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

	fclose(arq);

}