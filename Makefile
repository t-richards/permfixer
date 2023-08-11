.PHONY: all
all: permfixer

.PHONY: dist
dist: permfixer
	strip permfixer

.PHONY: clean
clean:
	rm -f permfixer

.PHONY: lint
lint:
	clang-format -i permfixer.c

permfixer: permfixer.c
	$(CC) -o permfixer permfixer.c $(CFLAGS)
