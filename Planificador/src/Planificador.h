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
#include <readline/readline.h>
#include <readline/history.h>

typedef enum ALGORITMO_PLANIFICACION {
	SJF_CD, SJF_SD, HRRN
} ALGORITMO_PLANIFICACION;

typedef struct planificador_config{
	char * puertoEscucha;
	ALGORITMO_PLANIFICACION algoritmoPlanificacion;
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


int PlanificadorON = 1;
sem_t pausarPlanificacion;
sem_t cantidadEsisEnReady;

t_log* logger;
t_list *listaReady, *listaBloqueado, *listaEjecutando, *listaTerminados,
		*listaEsiClave; //creamos listas para situacion de los Esi's; //creamos listas para situacion de los Esi's
int IdDisponible = 0;
pthread_mutex_t mutex;

ALGORITMO_PLANIFICACION traducir(char* algoritmo);
void procesarInstruccion(t_instruccion* instruccion, struct_esi * esi);
void ordenarPorSJF(t_list* lista);
void procesarLinea(char* linea,char ** comando, char ** parametros);
void* consola();
void crearListas();
void destruirListas();

PROTOCOLO_ESI_A_PLANIFICADOR recibirResultado(struct_esi* esi);

struct_esiClaves* crearEsiClave(struct_esi* esi, char*clave);

//FUNCIONES PARA EL ESI
void ordenarActuar(struct_esi* esi);
void agregarEsi(int socketCliente);
void *recibirEsi(void* socketEscucha);

//FUNCIONES PARA EL CORDI
void * manejarConexionCoordi(void * socket);
int tieneAlgunEsiLaClave(t_list* lista, char *claveBuscada);
int perteneceClaveAlEsi(t_list *lista, char* claveBuscada);

//FUNCIONES DE LIBERACION/ BLOQUEO
void agregarEnListaBloqueado(struct_esi *esiActual, char*clave);
void sacarStructDeListaEsiClave(char*clave);
void liberarEsi(char*clave);
//FUNCIONES DE CONFIGURACION

planificador_config * init_planificaorConfig();
void destroy_planificadorConfig(planificador_config*);
void crearServidorMultiHilo(int listenningSocket);
void crearConfiguracion(planificador_config** planificador,t_config** config);
void *manejadorDeConexiones(void *socket_desc);


#endif /* PLANIFICADOR_H_ */
