#include <stdio.h>
#include <stdlib.h>
#include "Esi.h"
#include <parsi/parser.h>

int main(int argc, char **argv) {


	 logger = crearLogger("loggerEsi.log", "loggerEsi");
	 config = calloc(1, sizeof(t_config));
	 esiConfig = init_esiConfig();

	crearConfiguracion(&esiConfig, &config);
	conectarseAlCoordinador();
	conectarseAlPlanificador();
	/*int socketCoordinador = conectarseAlServidor(logger, &esiConfig->ipCoordi,&esiConfig->puertoCoordi);
	recibirMensaje(logger, socketCoordinador);
	enviarMensaje(logger, ID_ESI, "SOYESI", socketCoordinador);

	int socketPlani= conectarseAlServidor(logger, &esiConfig->ipPlanificador,&esiConfig->puertoPlanificador);
	recibirMensaje(logger, socketPlani);
	enviarMensaje(logger, ID_ESI, "SOYESI", socketPlani);
*/


	FILE* script = fopen("script.txt","r");
	char*line=NULL;
	size_t len=0;
	ssize_t read;

	while ((read = getline(&line, &len, script)) != -1) {
	        t_esi_operacion parsed = parse(line);
	        enviarMensajeGenerico(logger,sizeof(parsed),ID_ESI,&parsed,socketCoordinador);

	        destruir_operacion(parsed);
	}

	fclose(script);
	free(line);


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

void conectarseAlCoordinador(){
	socketCoordinador = conectarseAlServidor(logger, &esiConfig->ipCoordi,&esiConfig->puertoCoordi);
	recibirMensaje(logger, socketCoordinador);
	enviarMensaje(logger, ID_ESI, "SOYESI", socketCoordinador);
}

void conectarseAlPlanificador(){
	socketPlani= conectarseAlServidor(logger, &esiConfig->ipPlanificador,&esiConfig->puertoPlanificador);
	recibirMensaje(logger, socketPlani);
	enviarMensaje(logger, ID_ESI, "SOYESI", socketPlani);

}

