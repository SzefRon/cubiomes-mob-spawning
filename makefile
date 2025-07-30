#CC      = gcc
#AR      = ar
ARFLAGS = cr
override LDFLAGS = -lm
override CFLAGS += -Wall -Wextra -fwrapv

ifeq ($(OS),Windows_NT)
	override CFLAGS += -D_WIN32
	CC = gcc
	RM = del
else
	override LDFLAGS += -pthread
	#RM = rm
endif

.PHONY : all debug release native libcubiomes clean

all: release

debug: CFLAGS += -DDEBUG -O0 -ggdb3
debug: libcubiomes
debug: build
release: CFLAGS += -O3
release: libcubiomes
release: build
native: CFLAGS += -O3 -march=native -ffast-math
native: libcubiomes

ifneq ($(OS),Windows_NT)
release: CFLAGS += -fPIC
#debug: CFLAGS += -fsanitize=undefined
endif


libcubiomes: biomes.o noise.o layers.o biomenoise.o generator.o finders.o util.o quadbase.o md5.o
	$(AR) $(ARFLAGS) libcubiomes.a $^

finders.o: finders.c finders.h
	$(CC) -c $(CFLAGS) $<

generator.o: generator.c generator.h
	$(CC) -c $(CFLAGS) $<

biomenoise.o: biomenoise.c
	$(CC) -c $(CFLAGS) $<

layers.o: layers.c layers.h
	$(CC) -c $(CFLAGS) $<

biomes.o: biomes.c biomes.h
	$(CC) -c $(CFLAGS) $<

noise.o: noise.c noise.h
	$(CC) -c $(CFLAGS) $<

util.o: util.c util.h
	$(CC) -c $(CFLAGS) $<

quadbase.o: quadbase.c quadbase.h
	$(CC) -c $(CFLAGS) $<

md5.o: md5.c md5.h
	$(CC) -c $(CFLAGS) $<

main.o: main.c
	$(CC) -c $(CFLAGS) $<

clean:
	$(RM) *.o *.a

build: main.o libcubiomes.a
	$(CC) $(CFLAGS) -o "build.exe" $^ $(LDFLAGS)