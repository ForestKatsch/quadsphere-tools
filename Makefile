
CFLAGS+=-O3
LDFLAGS+=-O3

.PHONY: clean

qst: src/main.o src/image.o src/image_pnm.c src/mem.o src/unit.o
	$(CC) $^ -o $@ $(LDFLAGS)