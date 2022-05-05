.PHONY: all clean

CFLAGS=-Iinclude -Wall -Wextra -Werror

all: hw-01_bmp

obj:
	mkdir obj

obj/bmp.o: src/bmp.c include/bmp.h | obj
	gcc -g -c ${CFLAGS} $< -o $@

obj/stego.o: src/stego.c include/stego.h include/bmp.h | obj
	gcc -g -c ${CFLAGS} $< -o $@

obj/main.o: src/main.c include/bmp.h include/stego.h | obj
	gcc -g -c ${CFLAGS} $< -o $@

hw-01_bmp: obj/main.o obj/bmp.o obj/stego.o
	gcc $^ -o $@

clean:
	rm -rf hw-01_bmp obj
