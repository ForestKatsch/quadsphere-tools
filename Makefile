
CFLAGS+=-O3 -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function# -DNDEBUG
LDFLAGS+=-O3 -g -lpthread

.PHONY: clean

qst: src/main.o src/image.o src/image_pnm.c src/mem.o src/unit.o src/file.o src/build.o src/build_height.o src/tile.o
	$(CC) $^ -o $@ $(LDFLAGS)