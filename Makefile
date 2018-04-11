all:
	$(CC) -g -O0 -o permfixer permfixer.c

dist:
	$(CC) -O3 -o permfixer permfixer.c
	strip permfixer
