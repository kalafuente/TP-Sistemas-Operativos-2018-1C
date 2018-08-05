/*
 * archivos.h
 *
 *  Created on: 19 jul. 2018
 *      Author: utnso
 */

#ifndef LIBRARY_ARCHIVOS_H_
#define LIBRARY_ARCHIVOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>

t_config * abrirArchivoConfig(int argc, char *argv[],t_log* logger,void(*destroy)());


#endif /* LIBRARY_ARCHIVOS_H_ */
