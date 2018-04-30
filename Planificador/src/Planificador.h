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
#include <manejoDeSockets/enviarYRecibirMensaje.c>
#include <manejoDeSockets/conectarseAlServidor.c>
#include <manejoDeSockets/crearSocketQueEscucha.c>

void *comunicacionCoordinador(void *socketCordinador);
int conectarseAlServidor(char ** ip, char ** puerto);

void crearConfiguracion(char ** puerto, char ** entradas, char ** tamanioEntradas, t_config ** config);

int crearServidor(char ** puerto, int * entradas);
int recibirmensaje(int unsocket);
int enviarmensaje(char*mensaje, int unsocket);

void *manejaconexionconESI(void * socket_desc);




#endif /* PLANIFICADOR_H_ */
