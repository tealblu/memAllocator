fast:
	gcc -O2 -DNDEBUG -Wall -fPIC -shared libmyalloc.c -o libmyalloc.so

dev:
	gcc -g -Wall -fPIC -shared libmyalloc.c -o libmyalloc.so

clean:
	rm -f *.o *.so