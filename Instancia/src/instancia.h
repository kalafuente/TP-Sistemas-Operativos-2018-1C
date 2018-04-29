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

void crearConfiguracion(char ** ipCoordi, char ** puertoCoordi, char ** algoritmo, char ** path, char ** nombre, int * intervalo,	t_config ** config);



#endif /* INSTANCIA_H_ */
