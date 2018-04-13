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
	$(CC) $(CFLAGS) -o permfixer permfixer.c
