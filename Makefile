
CFLAGS+=-O3 -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function# -DNDEBUG
LDFLAGS+=-O3 -g -lpthread

.PHONY: clean

FILES=\
src/main.o\
src/image.o\
src/image_pnm.c\
src/mem.o\
src/unit.o\
src/file.o\
src/build.o\
src/build_albedo.o\
src/build_height.o\
src/tile.o\
src/convert.o\
src/progress.o\
src/clock.o\
src/sample.o

qst: $(FILES)
	$(CC) $^ -o $@ $(LDFLAGS)