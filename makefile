fast:
	gcc -O2 -DNDEBUG -Wall -fPIC -shared libmyalloc.c -o libmyalloc.so

dev:
	gcc -g -Wall -fPIC -shared libmyalloc.c -o libmyalloc.so

clean:
	rm -f *.o *.so
	rm -f project3.tgz

tar:
	tar -czvf project3.tgz makefile libmyalloc.c libmyalloc.h README.md

test:
	gcc test.c -o test

runtest:
	LD_PRELOAD=./libmyalloc.so ./test 1