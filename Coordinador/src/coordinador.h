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
}coordinador_config;

typedef struct instancia{
	int socket;
	int cantEntradas;
	int tamanioEntradas;
	int tamanioOcupado;

}instancia;

typedef struct claveConInstancia{
	char* clave;
	instancia instancia;
}claveConInstancia;
;
//---------------------------VARIABLES GLOBALES-----------------------------

t_log* logger;
t_log* logDeOperaciones;
int cantEsi;
coordinador_config * coordConfig;
t_list* listaDeInstancias;
t_link_element* nodoDeInstancias;
t_link_element* nodoAuxiliarDeInstancias;
t_list* listaDeClavesConInstancia;
t_link_element* instanciaAElegir;

//---------------------------DECLARACION FUNCIONES-----------------------------

//-----------Archivo De Confuguracion
coordinador_config * init_coordConfig();
void destroy_coordConfig(coordinador_config* coord);
void crearConfiguracion(coordinador_config* coordinador, t_config* config);
//-----------Funciones auxiliares
void mandarConfiguracionAInstancia(int sock);
void registrarInstancia(int sock);
//void recibirInstruccion(int sock, instruccion * instruccionAGuardar);
void registrarLogDeOperaciones(char* operacion, char* instruccion, char * clave, char * valor );
void procesarInstruccion(t_instruccion * instruccion, int sock);
bool contieneClave(t_list* list, void* value);
bool contieneString(t_list* list, void* value);
instancia*  elegirInstanciaSegunAlgoritmo();
instancia * EquitativeLoad();
instancia nuevaInstanciaNula();
claveConInstancia* nuevaClaveConInstancia(char* clave, instancia _instancia);
t_instruccion* recibirInstruccionDelEsi(int sock);
void mostrarLista(t_list* lista);
void destruirInstruccion(t_instruccion*);
void mostrarListaIntancias();

//-----------Sockets
int crearServidor(char ** puerto, int * entradas);
void crearServidorMultiHilo(int listenningSocket);
void *manejadorDeConexiones(void *socket_desc);


#endif /* COORDINADOR_H_ */
