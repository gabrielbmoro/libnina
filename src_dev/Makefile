

FLAGS := -Wall -Wextra -O2 -lcpufreq

all: exec

exec.o: main.c
	gcc $(FLAGS) -o $@ -c $<

exec: hashmap.h hashmap.c libnina.h libnina.c exec.o
	gcc $(FLAGS) -o $@ $+

clean:
	rm -f *.o
	rm exec
