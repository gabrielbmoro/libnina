
/*************************************************************************
**  NINA                                                                **
**************************************************************************
**  Copyright (c) 1998-2009                                             **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS **
**                                                                      **
**************************************************************************/

#include "libnina.h"

ParallelRegionsFile *head = NULL;
bool isItTheLogServiceEnabled = false;
int amountOfCpus = 0;
long maxFrequency = 1200000;
int *targetCPUS = NULL;

void insertInList(char *name, int hashSize)
{
  ParallelRegionsFile *new;
  new = malloc(sizeof(struct ParallelRegionsFile));
  new->name = malloc(BUFFER_SIZE * sizeof(char));

  memcpy(new->name, name, BUFFER_SIZE);

  new->hash = hashmapCreate(hashSize);
  new->next = head;
  head = new;

  if (isItTheLogServiceEnabled)
    printf("libnina->insertInList: %s (%d)\n", name, hashSize);

}

ParallelRegionsFile *deleteFirst()
{

  ParallelRegionsFile *tmpNode = head;
  head = head->next;

  if (isItTheLogServiceEnabled)
    printf("libnina->deleteFirst\n");

  return tmpNode;

}

int isListEmpty()
{
  return (head == NULL);
}

ParallelRegionsFile *find(char *name)
{

  ParallelRegionsFile *current = malloc(sizeof(ParallelRegionsFile));

  current = head;

  if (head == NULL) {
    return NULL;
  }
  while (strcmp(current->name, name) < 0) {

    if (current->next == NULL) {
      return NULL;
    } else {
      current = current->next;
    }

  }

  if (isItTheLogServiceEnabled)
    printf("libnina->find: %s\n", name);

  return current;
}

void freeMemoryData()
{

  if (isListEmpty()) {
    return;
  }

  ParallelRegionsFile *current = NULL;

  while (head != NULL) {
    hashmapDelete(head->hash);
    current = head;
    head = head->next;
    free(current);
  }

  if (targetCPUS != NULL)
    free(targetCPUS);

  if (isItTheLogServiceEnabled)
    printf("libnina->freeMemoryData\n");

}

void printList()
{

  ParallelRegionsFile *tmp = head;

  while (tmp != NULL) {
    printf("---------------------------\n");
    printf("File name node %s\n", tmp->name);
    printf("Count of Hash %ld\n", hashmapCount(tmp->hash));
    printf("---------------------------\n\n");
    tmp = tmp->next;
  }

}

int getFileSize(char *filePath)
{
  FILE *arq;
  int ch, lines = 0;
  arq = fopen(filePath, "r");

  do {
    ch = fgetc(arq);
    if (ch == '\n')
      lines++;
  } while (ch != EOF);

  if (ch != '\n' && lines != 0) {
    lines++;
  }
  fclose(arq);
  return lines;
}

int randomNumberBetweenMinMax(int min, int max)
{
  srand((unsigned) time(NULL));
  if ((max - min) != 0)
    return (min + (rand() % (max - min)));
  else
    return (rand() % max);
}

void changeProcessorsFrequency(long freq)
{
  int cpufreqReturned = -1;
  int count = 0;

  for (count = 0; count < amountOfCpus; count++) {
    cpufreqReturned = cpufreq_set_frequency(targetCPUS[count], freq);
    if (isItTheLogServiceEnabled)
      printf("libnina->changeFreq: returned is %d of processor %d\n",
	     cpufreqReturned, targetCPUS[count]);
  }

  if (cpufreqReturned != 0) {
    printf
	("libnina->changeProcessorsFrequency: Problems to change processor's frequency...\n");
    exit(0);
  }

}

void callByNINALibrary(char *file, long start_line)
{

  ParallelRegionsFile *tmp = find(file);

  if (isItTheLogServiceEnabled)
    printf("libnina->callByNINALibrary: file %s at %ld\n", file,
	   start_line);

  if (tmp != NULL) {
    long newFrequency = hashmapGet(tmp->hash, start_line);
    changeProcessorsFrequency(newFrequency);
  }

}

int *convertStringToIntegerArray(char *str)
{

  char **res = NULL;

  char *p = strtok(str, ",");

  int n_spaces = 0, i;

  while (p) {
    res = realloc(res, sizeof(char *) * ++n_spaces);
    if (res == NULL)
      exit(-1);
    res[n_spaces - 1] = p;
    p = strtok(NULL, ",");
  }
  res = realloc(res, sizeof(char *) * (n_spaces + 1));
  res[n_spaces] = 0;

  int *numbers = malloc(sizeof(int) * n_spaces);

  for (i = 0; i < n_spaces; ++i)
    numbers[i] = atoi(res[i]);

  free(res);
  return numbers;
}

void changeProcessorsFrequencyToMax()
{
  changeProcessorsFrequency(maxFrequency);
}

void initLibrary()
{
  if ((getenv("NINA_CONFIG") == NULL)
      || (getenv("NINA_MAX_FREQUENCY") == NULL)
      || (getenv("NINA_TARGET_CPUS") == NULL)
      || (getenv("NINA_AMOUNT_OF_CPUS") == NULL)) {

    printf
	("libnina->initLibrary: It is necessary to define the environment variables NINA_CONFIG, NINA_MAX_FREQUENCY, NINA_AMOUNT_OF_CPUS, and NINA_TARGET_CPUS... \n");

    exit(1);

  } else {

    maxFrequency = atol(getenv("NINA_MAX_FREQUENCY"));

    amountOfCpus = atoi(getenv("NINA_AMOUNT_OF_CPUS"));

    isItTheLogServiceEnabled = (getenv("NINA_LOG") != NULL);

    char *str = malloc(sizeof(char) * 40);

    str = getenv("NINA_TARGET_CPUS");

    targetCPUS = convertStringToIntegerArray(str);

    if (targetCPUS == NULL)
      exit(0);

    FILE *arq;

    char *filePath = malloc(BUFFER_LINE * sizeof(char));

    filePath = getenv("NINA_CONFIG");

    arq = fopen(filePath, "r");

    char *fileNameTmp = malloc(BUFFER_SIZE * sizeof(char));

    int start_lineTmp = 0;

    long int freqTmp = 0;

    int amountOfLines = getFileSize(filePath);

    int count = 0;

    ParallelRegionsFile *current;

    while (count < amountOfLines) {

      fscanf(arq, "%d,%ld,%s\n", &start_lineTmp, &freqTmp, fileNameTmp);

      current = find(fileNameTmp);

      if (current == NULL) {

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
