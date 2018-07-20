#include "Planificador.h"

planificador_config * init_planificaorConfig(){
	planificador_config* planificador = malloc(sizeof (planificador_config));
	planificador->ipCoordinador=string_new();
	planificador->puertoCoordinador=string_new();
	planificador->puertoEscucha=string_new();
	planificador->alfaPlanificacion = 0;
	planificador->algoritmoPlanificacion=SJF_SD;
	planificador->entradas= 0;
	planificador->estimacionInicial=0;
	planificador->clavesPrebloqueadas = (char**) calloc(30, sizeof(char*));
	return planificador;
}

void crearConfiguracion(planificador_config* planificador, t_config* config){
	int aux = 0;
	ALGORITMO_PLANIFICACION i = traducir(config_get_string_value(config, "ALGORITMO"));

	string_append(&(planificador->ipCoordinador), config_get_string_value(config, "IP_COORDINADOR"));
	string_append(&(planificador->puertoCoordinador), config_get_string_value(config, "PUERTO_COORDINADOR"));
	string_append(&(planificador->puertoEscucha), config_get_string_value(config, "PUERTO_DE_ESCUCHA"));
	planificador->alfaPlanificacion = config_get_int_value(config, "ALFAP");
	planificador->estimacionInicial = config_get_double_value(config,"ESTIMACION");
	planificador->entradas = 500;
	planificador->algoritmoPlanificacion = i;

	while (config_get_array_value(config, "CLAVES_PREBLOQUEADAS")[aux] != NULL) {
		planificador->clavesPrebloqueadas[aux] = string_new();
		string_append(&(planificador->clavesPrebloqueadas[aux]),
				config_get_array_value(config, "CLAVES_PREBLOQUEADAS")[aux]);
		aux++;
	}

}

void destroy_planificadorConfig(planificador_config* planificador_config) {
	free(planificador_config->ipCoordinador);
	free(planificador_config->puertoCoordinador);
	free(planificador_config->puertoEscucha);
	free(planificador_config);
}

ALGORITMO_PLANIFICACION traducir(char* algoritmo) {
	if (string_equals_ignore_case(algoritmo, "SJF_SD")) {
		return SJF_SD;
	}
	if (string_equals_ignore_case(algoritmo, "HRRN")) {
		return HRRN;
	}

	return SJF_CD;

}
