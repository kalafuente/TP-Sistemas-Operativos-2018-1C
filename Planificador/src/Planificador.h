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
#include <protocolos/protocolos.h>
#include <commons/collections/list.h>
#include <sys/sem.h>

typedef struct planificador_config{
	char * puertoEscucha;
	char * algoritmoPlanificacion;
	int alfaPlanificacion;
	int estimacionInicial;
	char * ipCoordinador;
	char * puertoCoordinador;
	int entradas;

}planificador_config;

typedef struct {
	int socket;
	int tiempoDeEspera;
	int ID;
	int estimacion;
} struct_esi;

t_log* logger;
planificador_config * init_planificaorConfig();

void crearListas();
void agregarEsi(int socketCliente);
void *recibirEsi(void* socketEscucha);
void ordenarActuar(struct_esi* esi);
PROTOCOLO_ESI_A_PLANIFICADOR recibirResultado(struct_esi* esi);

void destroy_planificadorConfig(planificador_config*);
void crearServidorMultiHilo(int listenningSocket);
void crearConfiguracion(planificador_config** planificador,t_config** config);
void *manejadorDeConexiones(void *socket_desc);


#endif /* PLANIFICADOR_H_ */
