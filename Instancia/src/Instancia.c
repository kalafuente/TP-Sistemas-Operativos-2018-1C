#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <manejoDeSockets/enviarYRecibirMensaje.c>
#include <manejoDeSockets/conectarseAlServidor.c>


#define PACKAGESIZE 1024

int main(void) {

	char * ipCoordi = "127.0.0.1";
	char * puertoCoordi = "9191";

	int serverSocket;
	serverSocket = conectarseAlServidor(&ipCoordi, &puertoCoordi);

	int enviar = 1;
	printf("Conectado al servidor.");
	while (enviar) {
	recibirmensaje(serverSocket);
	recibirmensaje(serverSocket);
	enviarmensaje("nos conectamos, soy instancia :)", serverSocket);
	char mensaje[PACKAGESIZE];
	while (1) {
			fgets(mensaje, PACKAGESIZE, stdin);
			enviarmensaje(mensaje, serverSocket);
			}
			enviar = 0;
	}

	printf("\n termine\n");
	return 0;

}
