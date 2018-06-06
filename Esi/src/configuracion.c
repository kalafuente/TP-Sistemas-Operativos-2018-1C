/*
 * configuracion.c
 *
 *  Created on: 27 may. 2018
 *      Author: utnso
 */

#include "Esi.h"

void init_esiConfig() {
	esiConfig = malloc(sizeof(esi_config));
	esiConfig->ipCoordi = string_new();
	esiConfig->puertoCoordi = string_new();
	esiConfig->ipPlanificador = string_new();
	esiConfig->puertoPlanificador = string_new();

}
void crearConfiguracion() {
	config = config_create("configuracionEsi.config");
		init_esiConfig();
	string_append(&(esiConfig->ipCoordi),
			config_get_string_value(config, "IP_COORDINADOR"));
	string_append(&(esiConfig->puertoCoordi),
			config_get_string_value(config, "PUERTO_COORDINADOR"));
	string_append(&(esiConfig->ipPlanificador),
			config_get_string_value(config, "IP_PLANIFICADOR"));
	string_append(&(esiConfig->puertoPlanificador),
			config_get_string_value(config, "PUERTO_PLANIFICADOR"));

	config_destroy(config);
}

void destroy_esiConfig() {

	free(esiConfig->ipPlanificador);
	free(esiConfig->ipCoordi);
	free(esiConfig->puertoCoordi);
	free(esiConfig->puertoPlanificador);
	free(esiConfig);
}


void killEsi() {
	destroy_esiConfig();
	log_info(logger, "Hasta la vista, ESI");
	log_destroy(logger);
}

void enviarResultado(PROTOCOLO_ESI_A_PLANIFICADOR protocolo) {
	enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &protocolo,
			socketPlani);
}

void cargarLogger(int argc, char*argv[]) {
	if (argc < 2) {
		perror("Te olvidaste el path del logger capo");
		exit(1);
	}
	char* path = string_new();
	string_append(&path, argv[0]);
	logger = crearLogger(path, "loggerEsi");

}


