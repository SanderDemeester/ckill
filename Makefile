src=$(wildcard src/*.c)
obj=$(addprefix obj/,$(notdir $(src:.c=.o)))
CC=gcc
CFLAGS=-I. -Wall -pthread -std=c99 -lm
BIN=ckill

all:	ckill
debug: CC += -g
debug: ckill

ckill: $(obj)
	$(CC) $(CFLAGS) $(obj) -o $@
obj/%.o: src/*.c
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	if [ -f "AD3zip" ]; then \
	rm AD3zip; \
	fi; done
	@for file in $(obj); do \
	if [ -f $$file ]; then \
	rm "$$file"; \
	fi; done

