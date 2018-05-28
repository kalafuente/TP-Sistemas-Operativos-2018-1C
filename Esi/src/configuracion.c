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
	string_append(&(esiConfig->ipCoordi),
			config_get_string_value(config, "IP_COORDINADOR"));
	string_append(&(esiConfig->puertoCoordi),
			config_get_string_value(config, "PUERTO_COORDINADOR"));
	string_append(&(esiConfig->ipPlanificador),
			config_get_string_value(config, "IP_PLANIFICADOR"));
	string_append(&(esiConfig->puertoPlanificador),
			config_get_string_value(config, "PUERTO_PLANIFICADOR"));
}

void destroy_esiConfig() {

	free(esiConfig->ipPlanificador);
	free(esiConfig->ipCoordi);
	free(esiConfig->puertoCoordi);
	free(esiConfig->puertoPlanificador);
	free(esiConfig);
}
