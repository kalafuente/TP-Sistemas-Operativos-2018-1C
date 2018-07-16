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


typedef struct coordinador_config {
	char * puerto;
	char * algoritmo;
	int entradas;
	int tamanioEntradas;
	int retardo;
}coordinador_config;

typedef struct instancia{
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


#include "booleanasSobreListas.h"
#include "mostrarListas.h"
#include "distribucion.h"
#include "configuracion.h"
#include "logDeOperaciones.h"
#include "handshakes.h"
#include "registrosDeInstancias.h"
#include "respuestasAlESI.h"
//---------------------------VARIABLES GLOBALES-----------------------------

t_list* letrasDeLaInstancia;
int cantEsi;
coordinador_config * coordConfig;

t_link_element* nodoDeInstancias;
t_link_element* nodoAuxiliarDeInstancias;

t_link_element* instanciaAElegir;
int32_t socketPlani;

//---------------------------DECLARACION FUNCIONES-----------------------------
void prepararConfiguracion();
t_config *config;

void prepararLoggers();
t_log* logger;
t_log* logDeOperaciones;
t_log* logControlDeDistribucion;

void crearListas();
t_list* listaDeInstancias;
t_list* listaDeClavesConInstancia;

void crearServidor();
int listenningSocket;

void cerrarTodo();
void destruirLetrasDeLaInstancia();

//-----------Funciones auxiliares
bool enviarSETaInstancia(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible);
void mandarConfiguracionAInstancia(int sock);
void registrarInstancia(int sock);
void procesarInstruccion(t_instruccion * instruccion, int sock);
PROTOCOLO_PLANIFICADOR_A_COORDINADOR  estadoEsi(PROTOCOLO_COORDINADOR_A_PLANIFICADOR estadoClave, int socketPlani, char * clave);

instancia*  elegirInstanciaSegunAlgoritmo(char * clave);

instancia nuevaInstanciaNula();
t_instruccion* recibirInstruccionDelEsi(int sock);

void destruirInstruccion(t_instruccion*);
void instanciaCaida(int socketInstancia, int sock);
claveConInstancia* instanciaQueTieneLaClave(char* clave);
claveConInstancia* nuevaClaveConInstancia(char* clave);
void modificarInstanciaListaDeClavesConInstancia(char* clave, instancia* instanciaNueva);
void eliminarInstanciaYClaveDeLaListaDeInstancias(int socket);
void destruirInstancia(instancia* instancia);
void destruirClaveConInstancia(claveConInstancia* claveConInstancia);
void retardo();

//-----------Sockets

void crearServidorMultiHilo();
void *manejadorDeConexiones(void *socket_desc);


#endif /* COORDINADOR_H_ */
