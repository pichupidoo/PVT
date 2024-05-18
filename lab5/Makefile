all: serial parallel
serial: serial.c
	gcc -g -Wall -o serial serial.c
parallel: parallel.c
	gcc -g -Wall -o parallel parallel.c -fopenmp

.PHONY: s_run

s_run: serial
	./serial

.PHONY: p_run

p_run: parallel
	./parallel

.PHONY: clean

clean: serial parallel
	rm serial
	rm parallel