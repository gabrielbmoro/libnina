#!/bin/bash

bench=/svn/libnina/teste_1/src/strassen_with_scorep
targetPath=/svn/libnina/teste_1/expData

#first execution to get memory results
export OMP_NUM_THREADS=4
export SCOREP_ENABLE_TRACING=true
export SCOREP_TOTAL_MEMORY=2G
export SCOREP_METRIC_RUSAGE=ru_utime,ru_stime
export OMP_PROC_BIND=FALSE
export GOMP_CPU_AFFINITY=0-4
export SCOREP_METRIC_PAPI=PAPI_L2_TCA,PAPI_L2_TCM
export SCOREP_EXPERIMENT_DIRECTORY="$targetPath/mem"
sudo -E $bench 100

#second execution to get cpu results
export OMP_NUM_THREADS=4
export SCOREP_ENABLE_TRACING=true
export SCOREP_TOTAL_MEMORY=2G
export SCOREP_METRIC_RUSAGE=ru_utime,ru_stime
export OMP_PROC_BIND=FALSE
export GOMP_CPU_AFFINITY=0-4
export SCOREP_METRIC_PAPI=PAPI_TOT_INS,PAPI_TOT_CYC
export SCOREP_EXPERIMENT_DIRECTORY="$targetPath/cpu"
sudo -E $bench 100

sudo chmod 777 -R "$targetPath/mem"

sudo chmod 777 -R "$targetPath/cpu"

otf22csv "$targetPath/mem/traces.otf2" > "$targetPath/mem/traces.csv"

otf22csv "$targetPath/cpu/traces.otf2" > "$targetPath/cpu/traces.csv"

