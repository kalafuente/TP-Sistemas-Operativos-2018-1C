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
#include <commons/collections/list.h>
#include <library/manejoDeSockets.c>
#include <library/protocolos.h>
#include <sys/sem.h>
#include <semaphore.h>

typedef struct planificador_config{
	char * puertoEscucha;
	char * algoritmoPlanificacion;
	int alfaPlanificacion;
	double estimacionInicial;
	char * ipCoordinador;
	char * puertoCoordinador;
	int entradas;

}planificador_config;

typedef struct {
	int socket;
	int tiempoDeEspera;
	int ID;
	double estimacion;
} struct_esi;

typedef struct {
	struct_esi* ESI;
	char* clave;
} struct_esiClaves;


t_log* logger;
planificador_config * init_planificaorConfig();
t_list *listaReady, *listaBloqueado, *listaEjecutando, *listaTerminados,
		*listaEsiClave; //creamos listas para situacion de los Esi's; //creamos listas para situacion de los Esi's
int IdDisponible = 0;
pthread_mutex_t mutex; //puede que despues se necesitan mas(por ahora solo protege la cola Ready, de cuando llegan y cuando la usa)
sem_t cantidadEsisEnReady;
int PlanificadorON;

int tieneAlgunEsiLaClave(t_list* lista, char *claveBuscada);
int perteneceClaveAlEsi(t_list *lista, char* claveBuscada);
void crearListas();
void destruirListas();
void agregarEsi(int socketCliente);
void *recibirEsi(void* socketEscucha);
void ordenarActuar(struct_esi* esi);
PROTOCOLO_ESI_A_PLANIFICADOR recibirResultado(struct_esi* esi);

void destroy_planificadorConfig(planificador_config*);
void crearServidorMultiHilo(int listenningSocket);
void crearConfiguracion(planificador_config** planificador,t_config** config);
void *manejadorDeConexiones(void *socket_desc);


#endif /* PLANIFICADOR_H_ */
