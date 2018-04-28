/*
 * coordinador.h
 *
 *  Created on: 25 abr. 2018
 *      Author: utnso
 */

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

void crearConfiguracion(char ** puerto, int * entradas, int * tamanioEntradas,
		t_config ** config);
int crearServidor(char ** puerto, int * entradas);
void crearServidorMultiHilo(int listenningSocket);
void *manejadorDeConexiones(void *socket_desc);
int recibirmensaje(int unsocket);
int enviarmensaje(char*mensaje, int unsocket);
typedef struct {
	int id;
	int len;
}__attribute__((packed)) ContentHeader;


#endif /* COORDINADOR_H_ */
