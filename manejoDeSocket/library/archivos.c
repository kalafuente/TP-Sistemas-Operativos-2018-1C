/*
 * archivos.c
 *
 *  Created on: 19 jul. 2018
 *      Author: utnso
 */

#include "archivos.h"

t_config * abrirArchivoConfig(int argc, char *argv[],t_log* logger,void(*destroy)()){
	if (argc < 2) {
			log_error(logger,"NO PASASTE EL ARCHIVO LPTM");
			destroy();
			exit(EXIT_FAILURE);
		}
	else{
		return config_create(argv[1]);
	}
}
