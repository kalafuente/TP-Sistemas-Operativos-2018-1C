#include <stdio.h>
#include <stdlib.h>
#include "Esi.h"

int main(int argc, char **argv) {


	t_log* logger = crearLogger("loggerEsi.log", "loggerEsi");
	t_config * config = calloc(1, sizeof(t_config));
	esi_config * esiConfig = init_esiConfig();

	crearConfiguracion(&esiConfig, &config);
	int socketCoordinador = conectarseAlServidor(logger, &esiConfig->ipCoordi,&esiConfig->puertoCoordi);
	recibirMensaje(logger, socketCoordinador);
	enviarMensaje(logger, ID_ESI, "SOYESI", socketCoordinador);

	int socketPlani= conectarseAlServidor(logger, &esiConfig->ipPlanificador,&esiConfig->puertoPlanificador);
	recibirMensaje(logger, socketPlani);
	enviarMensaje(logger, ID_ESI, "SOYESI", socketPlani);


	close(socketCoordinador);
	destroy_esiConfig(esiConfig);
	config_destroy(config);
	return 0;

}

void crearConfiguracion(esi_config** esiConfig, t_config ** config) {
	*config = config_create("configuracionEsi.config");
	(*esiConfig)->ipCoordi = config_get_string_value(*config, "IP_COORDINADOR");
	(*esiConfig)->puertoCoordi = config_get_string_value(*config, "PUERTO_COORDINADOR");
	(*esiConfig)->ipPlanificador = config_get_string_value(*config,"IP_PLANIFICADOR");
	(*esiConfig)->puertoPlanificador = config_get_string_value(*config, "PUERTO_PLANIFICADOR");
}

void destroy_esiConfig(esi_config * esi) {

	free(esi->ipPlanificador);
	free(esi->ipCoordi);
	free(esi->puertoCoordi);
	free(esi->puertoPlanificador);
	free(esi);
}

esi_config* init_esiConfig() {
	esi_config* esiConfig = calloc(1, sizeof(esi_config));
	esiConfig->ipCoordi = calloc(9, sizeof(char));
	esiConfig->puertoCoordi = calloc(4, sizeof(char));
	esiConfig->ipPlanificador = calloc(9, sizeof(char));
	esiConfig->puertoPlanificador = calloc(4, sizeof(char));
	return esiConfig;
}

FILE* abrirScript(char *argv[]) {
	FILE * file;
	file = fopen(argv[1], "r");
	if (file == NULL) {
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}
	return file;
}

