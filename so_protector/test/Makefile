all: libtest.so main

libtest.so: libtest.cpp
	g++ -fPIC -shared libtest.cpp -o libtest.so

main: main.cpp
	g++ main.cpp -o main -ldl

clean:
	rm -f libtest.so main
