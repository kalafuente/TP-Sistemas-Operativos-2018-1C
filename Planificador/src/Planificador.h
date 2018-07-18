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
#include <library/manejoDeSockets.h>
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
	int rafagaActual;
	int socket;
	int tiempoDeEspera;
	int ID;
	double estimacion;
} struct_esi;

typedef struct {
	struct_esi* ESI;
	char* clave;
} struct_esiClaves;


#include "configuracion.h"
#include "comunicacionConCoordinador.h"
#include "sjf.h"

planificador_config * planiConfig;
t_config *config;
int PlanificadorON;
sem_t pausarPlanificacion;
sem_t cantidadEsisEnReady;

t_log* logger;
t_list *listaReady, *listaBloqueado, *listaEjecutando, *listaTerminados, *listaClaves,
		*listaEsiClave; //creamos listas para situacion de los Esi's; //creamos listas para situacion de los Esi's


int IdDisponible;
pthread_mutex_t mutex;


void prepararConfiguracion();
void procesarInstruccion(t_instruccion* instruccion, struct_esi * esi);
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
void desbloquear(t_list* listaBloqueado, t_list* listaReady, char* clave);
void actualizarBloqueado();

void crearServidorMultiHilo(int listenningSocket);
void *manejadorDeConexiones(void *socket_desc);

void imprimirConfiguracion(planificador_config* plani);

//FUNCIONES AUXILIARES
int indexOfString(t_list* lista, char* valorBuscado);
char* claveEsiClaves();

#endif /* PLANIFICADOR_H_ */

