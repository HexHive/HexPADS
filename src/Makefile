CC=clang
OPT=-O -g

FILES=hexpads.c readproc.c readperfctrs.c detector.c mitigator.c

.PHONY=all
all:
	$(CC) $(OPT) $(FILES) -o hexpads

.PHONE=clean
clean:
	rm -f hexpads
