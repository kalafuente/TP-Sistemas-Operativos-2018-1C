#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <pthread.h>
#include <manejoDeSockets/manejoDeSockets.c>

typedef struct planificador_config{
	int entradas;
	char * puertoEscucha;
	char * ipCoordinador;
	char * puertoCoordinador;

}planificador_config;

t_log* logger;
planificador_config * init_planificaorConfig();
void destroy_planificadorConfig(planificador_config*);
void crearServidorMultiHilo(int listenningSocket);
void crearConfiguracion(planificador_config** planificador,t_config** config);
void *manejadorDeConexiones(void *socket_desc);



#endif /* PLANIFICADOR_H_ */
