src=$(wildcard src/*.c)
obj=$(addprefix obj/,$(notdir $(src:.c=.o)))
CC=gcc
CFLAGS=-g -I. -Wall -lmenu -lncurses -pthread -std=gnu99 -lm 
BIN=ckill

all:	ckill
debug:  CC += -g
debug:
	CC += -D _DEBUG
debug:  ckill

ckill: $(obj)
	$(CC) $(obj) $(CFLAGS)-o $@
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	if [ -f "ckill" ]; then \
	rm ckill; \
	fi; done
	@for file in $(obj); do \
	if [ -f $$file ]; then \
	rm "$$file"; \
	fi; done

