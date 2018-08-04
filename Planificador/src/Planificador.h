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
#include <commons/log.h>
#include <library/manejoDeSockets.h>
#include <library/protocolos.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <library/archivos.h>
#include <signal.h>
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
	char** clavesPrebloqueadas;
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

typedef struct {
	int socketCoordi;
	int socketStatus;
	int socketListen;
} struct_socketsImportantes;

#include "configuracion.h"
#include "comunicacionConCoordinador.h"
#include "sjf.h"
#include "booleanasSobreListas.h"
#include "planificacionDeEsis.h"
#include "comunicacionConEsis.h"
#include "gestionDeEsis.h"
#include "deadlock.h"
#include "liberacionDeMemoria.h"
struct_esi*EsiSistema;
int EsisNuevos;
int IdDisponible;
int PlanificadorON;
int consolaOn;

planificador_config * planiConfig;
t_config *config;
t_log* logger;

sem_t cantidadEsisEnReady;
sem_t pausarPlanificacion;
sem_t huboDesalojoClaves;

t_list *listaReady, *listaBloqueado, *listaEjecutando, *listaTerminados, *listaClaves,*listaEsiClave;
pthread_mutex_t mutex, mutexKillEsi, mutexConsola, mutexConsolaEnEspera,
		mutexPlanificacion;
void cerrarSockets();
void destuirListas();
void inicializar();
void prepararConfiguracion(int argc, char **argv);
void prepararLogger();
void inicializarSemaforos();
void crearListas();
void procesarLinea(char* linea,char ** comando, char ** parametros);
void cargarClavesPrebloqueadas();
void* consola();

void desbloquear(t_list* listaBloqueado, t_list* listaReady, char* clave);
void* actualizarBloqueado();

void crearServidorMultiHilo(int listenningSocket);
void *manejadorDeConexiones(void *socket_desc);


//FUNCIONES AUXILIARES
int indexOfString(t_list* lista, char* valorBuscado);
char* claveEsiClaves();
void destruirLogger();

#endif /* PLANIFICADOR_H_ */

