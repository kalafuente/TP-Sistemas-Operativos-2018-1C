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
#include <protocolos/protocolos.h>
#include <commons/string.h>
#include <parsi/parser.h>
#include <commons/log.h>
#include <commons/collections/list.h>

typedef struct coordinador_config {
	char * puerto;
	char * algoritmo;
	int entradas;
	int tamanioEntradas;
}coordinador_config;

typedef struct instancia{
	int socket;
	int cantEntradas;
	int tamanioEntradas;
	int tamanioOcupado;
}instancia
;
//---------------------------VARIABLES GLOBALES-----------------------------

t_log* logger;
t_log* logDeOperaciones;
int cantEsi;
coordinador_config * coordConfig;
t_list* listaDeInstancias;

//---------------------------DECLARACION FUNCIONES-----------------------------

//-----------Archivo De Confuguracion
coordinador_config * init_coordConfig();
void destroy_coordConfig(coordinador_config* coord);
void crearConfiguracion(coordinador_config* coordinador, t_config* config);
void mandarConfiguracionAInstancia(int sock);
void registrarInstancia(int sock);

void recibirInstruccion(int sock);
//-----------Sockets
int crearServidor(char ** puerto, int * entradas);
void crearServidorMultiHilo(int listenningSocket);
void *manejadorDeConexiones(void *socket_desc);


#endif /* COORDINADOR_H_ */
