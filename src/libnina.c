
/*************************************************************************
**  NINA                                                                **
**************************************************************************
**  Copyright (c) 1998-2009                                             **
**  Gabriel B Moro, Lucas M. Schnorr, Instituto de Informática da UFRGS **
**                                                                      **
**************************************************************************/
#include <omp.h>
#include "libnina.h"

bool logEnabled = false;

int amountOfCpus = 0;
long maxFrequency = 1200000;
int *targetCPUS = NULL;

void changeProcessorsFrequency(long freq)
{
  int cpufreqReturned = -1;
  int i = 0;

  for (i = 0; i < amountOfCpus; i++) {
    cpufreqReturned = cpufreq_set_frequency(targetCPUS[i], freq);
    if (cpufreqReturned != 0) {
      LOG(printf("libnina->changeProcessorsFrequency: Problems to change processor's frequency...\n"));
      exit(0);
    }else{
      LOG(printf("%s: of processor %d to frequency %ld (ret = %d)\n", __func__,
		 targetCPUS[i], freq, cpufreqReturned));
    }
  }
}

void callByNINALibrary(char *file, long start_line)
{
  long newFrequency = LIBNINA_GetFrequency(file, start_line);
  LOG(printf("libnina->callByNINALibrary: file %s at %ld => %ld\n", file, start_line, newFrequency));
  if (newFrequency > 0){
    changeProcessorsFrequency(newFrequency);
  }
}

static int *convertStringToIntegerArray(char *str)
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
  for (i = 0; i < n_spaces; ++i){
    numbers[i] = atoi(res[i]);
  }
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
    // Enable or not the log.
    logEnabled = (getenv("NINA_LOG") != NULL);
    if (logEnabled){
      POMP2_On();
    }else{
      POMP2_Off();
    }

    LOG(printf("libnina->initLibrary: starting...\n"));
    LIBNINA_LoadRegionsFile();

    maxFrequency = atol(getenv("NINA_MAX_FREQUENCY"));
    amountOfCpus = atoi(getenv("NINA_AMOUNT_OF_CPUS"));
    char *str = malloc(sizeof(char) * 40);
    str = getenv("NINA_TARGET_CPUS");
    targetCPUS = convertStringToIntegerArray(str);

    if (targetCPUS == NULL){
      exit(0);
    }
    LOG(printf("libnina->initLibrary: finished.\n"));
  }
}
