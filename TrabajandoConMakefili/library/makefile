all:
	-gcc -c -Wall -fpic manejoDeSockets.c
	-gcc -c -Wall -fpic protocolos.c
	-gcc -shared -o libmanejoDeSockets.so protocolos.o manejoDeSockets.o
	-cp libmanejoDeSockets.so /usr/lib
	-mkdir /usr/include/library
	-cp *.h /usr/include/library