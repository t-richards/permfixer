CFLAGS := $(CFLAGS) -lacl

.PHONY: all
all: permfixer

.PHONY: dist
dist: permfixer
	strip permfixer

.PHONY: clean
clean:
	rm -f permfixer

permfixer:
	$(CC) $(CFLAGS) -o permfixer permfixer.c
