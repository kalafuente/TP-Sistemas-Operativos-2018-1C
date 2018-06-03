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

typedef struct instruccion {
	PROTOCOLO_INSTRUCCIONES instruccion;
	char * clave;
	char * valor;
}instruccion;
typedef struct instancia{
	int socket;
	int cantEntradas;
	int tamanioEntradas;
	int tamanioOcupado;
	t_list* claves;
	int bandera;
}instancia
;
//---------------------------VARIABLES GLOBALES-----------------------------

t_log* logger;
t_log* logDeOperaciones;
int cantEsi;
coordinador_config * coordConfig;
t_list* listaDeInstancias;
t_list* listaDeClaves;
t_link_element* instanciaAElegir;

//---------------------------DECLARACION FUNCIONES-----------------------------

//-----------Archivo De Confuguracion
coordinador_config * init_coordConfig();
void destroy_coordConfig(coordinador_config* coord);
void crearConfiguracion(coordinador_config* coordinador, t_config* config);
//-----------Funciones auxiliares
void mandarConfiguracionAInstancia(int sock);
void registrarInstancia(int sock);
void recibirInstruccion(int sock, instruccion * instruccionAGuardar);
void registrarLogDeOperaciones(char* operacion, char* instruccion, char * clave, char * valor );
void procesarInstruccion(instruccion instruccion, int sock);
bool contieneString(t_list* list, void* value);
void elegirInstanciaSegunAlgoritmo(char* instruccion);
t_link_element obtenerInstanciaParaEL(t_list *self);

//-----------Sockets
int crearServidor(char ** puerto, int * entradas);
void crearServidorMultiHilo(int listenningSocket);
void *manejadorDeConexiones(void *socket_desc);


#endif /* COORDINADOR_H_ */
