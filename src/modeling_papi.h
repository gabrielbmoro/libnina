#ifndef __MODELING_PAPI_H_
#define __MODELING_PAPI_H_
#include <stdio.h>
#define MODELING_PAPI
#ifdef MODELING_PAPI
#include <libconfig.h>
#include <papi.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>

void model_read_configuration(void);
void model_init(void);
void model_papi_init(void);
void model_papi_start_counters(void);
void model_papi_stop_counters(void);
void model_papi_header(void);
void model_papi_report(FILE * fp);
void model_papi_report_with_fp(FILE * pfp);
void model_papi_header(void);
void model_papi_header_with_fp(FILE * pfp);
void model_papi_affinity(void);
void model_papi_configuration(config_t * cfg);

#else

#define model_papi_init(A)
#define model_papi_start_counters(A)
#define model_papi_stop_counters(A)
#define model_papi_header(A)
#define model_papi_header_with_fp(A)
#define model_papi_report(A)
#define model_papi_report_with_fp(A)
#define model_papi_header(A)
#define model_papi_affinity(A)
#define model_papi_configuration(A)

#endif				//MODELING_PAPI
#endif
