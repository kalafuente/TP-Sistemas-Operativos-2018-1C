all:
	-gcc -c -Wall -fpic ../../library/library/manejoDeSockets.c
	-gcc -c -Wall -fpic ../../library/library/protocolos.c
	-gcc -c -Wall -fpic ../../library/library/archivos.c
	-gcc -shared -o libmanejoDeSockets.so protocolos.o manejoDeSockets.o archivos.o
	-cp libmanejoDeSockets.so /usr/lib
	-mkdir /usr/include/library
	-cp *.h /usr/include/library
clean:
	-rm *.so
	-rm *.o
	-rm -r /usr/include/library
	-rm /usr/lib/libmanejoDeSockets.so
