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
#ifndef LIB_NINA_H
#define LIB_NINA_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <cpufreq.h>
#include <time.h>
#include <stdbool.h>
#include "pomp2_user_lib.h"
#include "db.h"
#include "modeling_papi.h"

#define BUFFER_SIZE 1024
#define BUFFER_LINE 200
#define INIT_HASH_SIZE 256

#define LOG(COMMAND) if(logEnabled){ COMMAND; };

void LIBNINA_InitLibrary();
void LIBNINA_ParallelBegin(char *file, long start_line);
void LIBNINA_ParallelEnd(char *file, long start_line);

#endif				// LIB_NINA_H
