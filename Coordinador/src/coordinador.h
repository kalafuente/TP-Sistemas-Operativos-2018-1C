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
#include <commons/string.h>
#include <parsi/parser.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <library/manejoDeSockets.h>
#include <library/protocolos.h>
#include <library/archivos.h>
#include <semaphore.h>


typedef struct coordinador_config {
	char * puerto;
	char * algoritmo;
	int entradas;
	int tamanioEntradas;
	int retardo;
}coordinador_config;

typedef struct instancia{
	char * identificador;
	int socket;
	int cantEntradasTotales;
	int tamanioEntradas;
	int cantEntradasOcupadas;

}instancia;

typedef struct claveConInstancia{
	char* clave;
	instancia* instancia;
}claveConInstancia;
;


typedef struct instanciaYSusCaracteres {
	instancia * instancia;
	t_list* caracteres;
}instanciaYSusCaracteres;

#include "alfabeto.h"
#include "distribucion.h"
#include "comunicacionConEsi.h"
#include "comunicacionConInstancia.h"
#include "comunicacionConPlanificador.h"
#include "booleanasSobreListas.h"
#include "mostrarListas.h"
#include "configuracion.h"
#include "logDeOperaciones.h"
#include "handshakes.h"
#include "gestionDeInstancias.h"
#include "status.h"

//---------------------------VARIABLES GLOBALES-----------------------------
t_log* logger;
t_log* logDeOperaciones;
t_log* logControlDeDistribucion;

coordinador_config * coordConfig;
t_config *config;

t_list* alfabeto;
t_list* letrasDeLaInstancia;
t_list* listaDeInstancias;
t_list* listaDeClavesConInstancia;
t_list* hilos;

int banderaTerminarHilos;
int32_t socketPlani;
int listenningSocket;

pthread_mutex_t mutexCompactacion;
pthread_mutex_t mutexlistaInstancias;
sem_t terminoCompactacion;



//---------------------------DECLARACION FUNCIONES-----------------------------
void prepararConfiguracion(int argc, char **argv);
void prepararLoggers();
void crearListas();
void crearServidor();
void terminarHilos();
void cerrarTodo();
void procesarInstruccion(t_instruccion * instruccion, int sock);
void destruirInstruccion(t_instruccion*);
void instanciaCaida(char * clave, int sock);
void retardo();
void destruirListas();
void destruirLoggers();
void killCoordinador();
void mostrarValoresArchConfig(coordinador_config* config);
void eliminarEsteHilo(pthread_t hilo);
void *compactar(void * elemento);
//Sockets

void crearServidorMultiHilo();
void *manejadorDeConexiones(void *socket_desc);


#endif /* COORDINADOR_H_ */
