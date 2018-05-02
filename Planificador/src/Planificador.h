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
#include <manejoDeSockets/enviarYRecibirMensaje.c>
#include <manejoDeSockets/conectarseAlServidor.c>
#include <manejoDeSockets/crearSocketQueEscucha.c>

typedef struct planificador_config{
	int entradas;
	char * puertoEscucha;
	char * ipCoordinador;
	char * puertoCoordinador;

}planificador_config;

planificador_config * init_planificaorConfig();
void crearConfiguracion2(planificador_config**,t_config**);
void destroy_planificadorConfig(planificador_config*);

void *comunicacionCoordinador(void *socketCordinador);
void crearConfiguracion(char ** puerto, char ** entradas, char ** tamanioEntradas, t_config ** config);
void *manejaconexionconESI(void * socket_desc);




#endif /* PLANIFICADOR_H_ */
