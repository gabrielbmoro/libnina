#include <omp.h>
#include "modeling_papi.h"
#ifdef MODELING_PAPI
#define TF_NAME_LEN 100 // Maximum length of the trace file name
#define CFG_DEFAULT_PATH "modeling-libnina.cfg"
extern FILE *fp; //defined somewhere

int *Events;
long_long *Values;
int papi_nevents;

void model_error_check (int returnCode){
  if (returnCode == CONFIG_FALSE){
    fprintf (stderr, "Failed reading configuration from %s.\n", CFG_DEFAULT_PATH);
    fflush (stderr);
    exit(123);
  }
}

void model_read_configuration (void)
{
  int iteration = 0;
  FILE *fd = fopen (CFG_DEFAULT_PATH, "r");
  if (!fd){
    fprintf (stderr, "it %d Configuration file %s coulnd't be open for reading.\n", iteration, CFG_DEFAULT_PATH);
    return;
  }
  config_t cfg;
  config_init (&cfg);
  
  if (config_read(&cfg, fd) == CONFIG_FALSE){
    fprintf (stderr,  "Failed to read the configuration file.\n");
    exit(123);
  }else{
    fclose(fd);
  }

  model_papi_affinity ();
  
  model_papi_configuration (&cfg);

  //The global trace filename
  const char *trace_filename_prefix;
  model_error_check(config_lookup_string(&cfg, "filename", &trace_filename_prefix));
  char trace_filename[TF_NAME_LEN] = "\0";
  snprintf(trace_filename, TF_NAME_LEN, "%s.csv", trace_filename_prefix);
  if (fp) fclose(fp);
  fp = fopen(trace_filename, "w");
  
  //destroy configuration
  config_destroy(&cfg);
}

void model_init (void)
{
  model_papi_init ();
}

void model_papi_init ()
{
  PAPI_library_init(PAPI_VER_CURRENT);
#pragma omp parallel
  if( PAPI_thread_init( (unsigned long (*)(void))(omp_get_thread_num) ) != PAPI_OK ) {
    printf("PAPI couldn't init its threads\n");
    exit(0);
  }
  int num_hwcntrs = PAPI_num_counters();
  if (num_hwcntrs < 0){
    printf("The installation does not support PAPI.\n");
    exit(1);
  }else if(num_hwcntrs == 0){
    printf("The installation supports PAPI, but has no counters.\n");
    exit(1);
  }
  Events = malloc(PAPI_MAX_HWCTRS*sizeof(int));
  Values = malloc(PAPI_MAX_HWCTRS*sizeof(long_long));
  papi_nevents = 0;
}

void model_papi_start_counters ()
{
  PAPI_start_counters(Events, papi_nevents);
}

void model_papi_stop_counters ()
{
  PAPI_stop_counters(Values, papi_nevents);
}

void model_papi_header ()
{
  model_papi_header_with_fp (fp);
}

void model_papi_header_with_fp (FILE *pfp)
{
  int i;
  //report hw counters
  fprintf(pfp, " ");
  for (i = 0; i < papi_nevents; i++){
    char EventCodeStr[PAPI_MAX_STR_LEN];
    PAPI_event_code_to_name(Events[i], EventCodeStr);
    fprintf(pfp, "%s", EventCodeStr);
    if (i+1 != papi_nevents){
      fprintf(pfp, " ");
    }
  }
}

void model_papi_report (FILE *zz)
{
  model_papi_report_with_fp (zz);
}

void model_papi_report_with_fp (FILE *pfp)
{
  int i;
  //report hw counters
  for (i = 0; i < papi_nevents; i++){
    fprintf(pfp, "%lld", Values[i]);
    if (i+1 != papi_nevents){
      fprintf(pfp, " ");
    }
  }
}

void model_papi_affinity ()
{
  //disable core affinity (let OpenMP handle this)
  return;
}

void model_papi_configuration (config_t *cfg)
{
  config_setting_t *sub_cfg;
  papi_nevents = 0;
  sub_cfg = config_lookup (cfg, "papi.counters");
  const char *papi_event_name;
  int i;
  for (i = 0; (papi_event_name = config_setting_get_string_elem (sub_cfg, i)); i++){
    if (PAPI_event_name_to_code ((char*)papi_event_name, &Events[i]) != PAPI_OK){
      fprintf (stderr, "Failed to codify papi event [%s].\n", papi_event_name);
      exit(1);
    }
    papi_nevents++;
  }
  papi_nevents = i;
}
#endif //MODELING_PAPI
