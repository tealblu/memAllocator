fast:
	gcc -O2 -DNDEBUG -Wall -fPIC -shared libmyalloc.c -lm -o libmyalloc.so

dev:
	gcc -g -Wall -fPIC -shared libmyalloc.c -lm -o libmyalloc.so

clean:
	rm -f *.o *.so
	rm -f project3.tgz

tar:
	tar -czvf project3.tgz makefile libmyalloc.c libmyalloc.h README.md

test:
	gcc -g test.c -lm -o test

runtest:
	LD_PRELOAD=./libmyalloc.so ./test