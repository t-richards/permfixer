all:
	$(CC) -g -O0 -lacl -o permfixer permfixer.c

dist:
	$(CC) -O2 -lacl -o permfixer permfixer.c
	strip permfixer
