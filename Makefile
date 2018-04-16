CFLAGS := $(CFLAGS) -lacl

.PHONY: all
all: permfixer

.PHONY: dist
dist: permfixer
	strip permfixer

.PHONY: clean
clean:
	rm -f permfixer

permfixer: permfixer.c
	$(CC) -o permfixer permfixer.c $(CFLAGS)
