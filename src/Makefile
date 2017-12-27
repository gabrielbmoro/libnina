# Makefile template for shared library

CFLAGS = -I/$SCOREP_PATH/include/ -I.
LDFLAGS = -L$SCOREP_PATH/lib/

CC = gcc # C compiler
CFLAGS += -fPIC -Wall -Wextra -O2 -g -fopenmp # C flags
LDFLAGS += -shared -lcpufreq  # linking flags
RM = rm -f  # rm command
TARGET_LIB = libnina.so # target lib

SRCS = hashmap.c libnina.c pomp2_lib.c pomp2_user_region_info.c pomp2_region_info.c opari2_ctc_parser.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
