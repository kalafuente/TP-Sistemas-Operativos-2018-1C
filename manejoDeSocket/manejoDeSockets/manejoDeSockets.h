#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <commons/log.h>



#ifndef BIBLIOTECA_MANEJODESOCKETS_H_
#define BIBLIOTECA_MANEJODESOCKETS_H_
#define ID_ESI 1000
#define ID_PLANIFICADOR 3500
#define ID_INSTANCIA 7000
#define ID_COORDINADOR 4112


typedef struct ContentHeader {
	int id;
	int len;
}__attribute__((packed)) ContentHeader;

t_log* crearLogger(char*archivo, char*nombre);
int enviarMensajeGenerico(t_log* logger, int tamanio, int id, void*mensaje, int unsocket);
void exitWithError(t_log* logger, int socket, char* error_msg, void * buffer);
int enviarChar(t_log* logger, char*mensaje, int unsocket);
/*int recibirMensaje(t_log* logger,size_t, int unsocket);*/
int recibirMensaje(t_log* logger, size_t len, void* buffer, int unsocket);
int enviarMensaje(t_log* logger, size_t len, const void* msg, int unsocket);
int crearSocketQueEscucha(char ** puerto, int * entradas);
int conectarseAlServidor(t_log* logger, char ** ip, char ** puerto);
t_log* crearLogger(char* nombreDelArchivo, char* nombreAMostrar);
int conectarAlServidor(t_log* logger, char * ip, char * puerto);
void exitGracefully(t_log* logger, int valorDeRetorno);
void * recibirContenido(t_log * logger, int socket);


#endif /* BIBLIOTECA_MANEJODESOCKETS_H_ */
