#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <pthread.h>
#include <manejoDeSockets/manejoDeSockets.h>
#include <commons/string.h>

typedef struct coordinador_config {
	char * puerto;
	int entradas;
	int tamanioEntradas;
}coordinador_config;

t_log* logger;

coordinador_config * init_coordConfig();

void destroy_coordConfig(coordinador_config* coord);
void crearConfiguracion(coordinador_config** coord, t_config ** config);
int crearServidor(char ** puerto, int * entradas);
void crearServidorMultiHilo(int listenningSocket);
void *manejadorDeConexiones(void *socket_desc);


#endif /* COORDINADOR_H_ */