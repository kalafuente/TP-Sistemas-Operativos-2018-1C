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
#include <manejoDeSockets/manejoDeSockets.c>

typedef struct instancia_config {
	char * ipCoordi ;
	char * puertoCoordi;
	char * algoritmo;
	char * path ;
	char * nombre ;
	int intervalo;
} instancia_config;

//------------Variables globales
t_log* logger;

//------------Declaraciones de funciones
instancia_config * init_instanciaConfig();
void crearConfiguracion(instancia_config **, t_config**);
void destroy_instanciaConfig(instancia_config*);



#endif /* INSTANCIA_H_ */
