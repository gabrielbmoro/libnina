/*
    This file is part of libnina

    libnina is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libnina is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with libnina. If not, see <http://www.gnu.org/licenses/>.
*/
#include <omp.h>
#include "libnina.h"

bool logEnabled = false;

int amountOfCpus = 0;
int *targetCPUS = NULL;

#ifdef LIBNINA_PAPI
static double last = 0;
FILE *fp;
#endif

#ifdef LIBNINA_PAPI
#include <sys/time.h>
inline double gettime()
{
   struct timespec s;
   clock_gettime(CLOCK_REALTIME, &s);
   double res = s.tv_sec + ((double)s.tv_nsec)/1000000000;
   return res;
}

/* static double gettime (void) */
/* { */
/*   struct timeval tr; */
/*   gettimeofday(&tr, NULL); */
/*   return (double)tr.tv_sec+(double)tr.tv_usec/1000000; */
/* } */
#endif

static void changeProcessorsFrequency(long freq)
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

void LIBNINA_ParallelBegin(char *file, long start_line)
{
  long newFrequency;
  newFrequency = LIBNINA_GetFrequency(file, start_line);
  LOG(printf("libnina->callByNINALibrary: file %s at %ld => %ld\n", file, start_line, newFrequency));
  if (newFrequency > 0){
    changeProcessorsFrequency(newFrequency);
  }
#ifdef LIBNINA_PAPI
  last = gettime();
  model_papi_start_counters ();
#endif
}

void LIBNINA_ParallelEnd(char *file, long start_line)
{
#ifdef LIBNINA_PAPI
  model_papi_stop_counters ();
  double t1 = gettime();
  fprintf(fp, "%ld %g %g %g ", start_line, t1, last, t1 - last);
  model_papi_report(fp);
  fprintf(fp, "\n");
#endif
}

void LIBNINA_InitLibrary()
{
#ifdef LIBNINA_PAPI
  fp = stdout;
#endif
  if ((getenv("NINA_CONFIG") == NULL)
      || (getenv("NINA_TARGET_CPUS") == NULL)
      || (getenv("NINA_AMOUNT_OF_CPUS") == NULL)) {

    printf
      ("%s: It is necessary to define the environment variables NINA_CONFIG, NINA_AMOUNT_OF_CPUS, and NINA_TARGET_CPUS... \n", __func__);

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

    amountOfCpus = atoi(getenv("NINA_AMOUNT_OF_CPUS"));
    char *str = malloc(sizeof(char) * 40);
    str = getenv("NINA_TARGET_CPUS");
    targetCPUS = convertStringToIntegerArray(str);

    if (targetCPUS == NULL){
      exit(0);
    }
    LOG(printf("libnina->initLibrary: finished.\n"));
  }

#ifdef LIBNINA_PAPI
  model_init();
  model_read_configuration ();
#endif
}
