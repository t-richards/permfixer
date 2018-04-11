all:
	$(CC) -g -O0 -o permfixer permfixer.c

dist:
	$(CC) -O2 -o permfixer permfixer.c
	strip permfixer
