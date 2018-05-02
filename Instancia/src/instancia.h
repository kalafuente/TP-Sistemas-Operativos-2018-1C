/*
 * instancia.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <manejoDeSockets/enviarYRecibirMensaje.c>
#include <manejoDeSockets/conectarseAlServidor.c>

typedef struct instancia_config {
	char * ipCoordi ;
	char * puertoCoordi;
	char * algoritmo;
	char * path ;
	char * nombre ;
	int intervalo;
} instancia_config;


instancia_config * init_instanciaConfig();
void crearConfiguracion2(instancia_config **, t_config**);
void destroy_instanciaConfig(instancia_config*);


void crearConfiguracion(char ** ipCoordi, char ** puertoCoordi,
		char ** algoritmo, char ** path, char ** nombre, int * intervalo,
		t_config ** config);

#endif /* INSTANCIA_H_ */
