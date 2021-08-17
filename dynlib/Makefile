main: main.c libreturn42.so
	gcc -Wall -Werror -o $@ main.c -ldl

libreturn42.so: return_42.o
	gcc -Wall -Werror -shared -fPIC -Wl,-soname,$@ -o $@ return_42.o -lc

return_42.o: return_42.c
	gcc -Wall -Werror -c -fPIC -o $@ $<
