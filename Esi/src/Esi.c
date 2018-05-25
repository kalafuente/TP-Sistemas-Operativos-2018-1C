#include <stdio.h>
#include <stdlib.h>
#include "Esi.h"
#include <parsi/parser.h>

int main(int argc, char **argv) {

	abrirScript(argv);
	logger = crearLogger("loggerEsi.log", "loggerEsi");
	crearConfiguracion();
	conectarseAlCoordinador();
	conectarseAlPlanificador();
	t_esi_operacion * parsed = calloc(1,sizeof(t_esi_operacion ));

	char*line=NULL;
	size_t len=0;
	ssize_t read;

	while ((read = getline(&line, &len, script)) != -1) {

	        * parsed = parse(line);

	        switch (parsed->keyword){
	        case GET:
	        	enviarMensaje(logger,1,parsed->argumentos.GET.clave,socketCoordinador);
				break;
	        case SET:
	        	enviarMensaje(logger,2,parsed->argumentos.SET.clave,socketCoordinador);
	        	enviarMensaje(logger,2,parsed->argumentos.SET.valor,socketCoordinador);
	        	break;
	        case STORE:
	        	enviarMensaje(logger,3,parsed->argumentos.STORE.clave,socketCoordinador);
	        	break;
	        }

	        destruir_operacion(*parsed);
	}

	fclose(script);
	free(line);
	free (parsed);


	close(socketCoordinador);
	close(socketPlani);
	destroy_esiConfig(esiConfig);
	//config_destroy(config);
	return 0;

}
void destroy_esiConfig() {

	free(esiConfig->ipPlanificador);
	free(esiConfig->ipCoordi);
	free(esiConfig->puertoCoordi);
	free(esiConfig->puertoPlanificador);
	free(esiConfig);
}


void init_esiConfig() {
	esiConfig = calloc(1, sizeof(esi_config));
	esiConfig->ipCoordi = calloc(9, sizeof(char));
	esiConfig->puertoCoordi = calloc(4, sizeof(char));
	esiConfig->ipPlanificador = calloc(9, sizeof(char));
	esiConfig->puertoPlanificador = calloc(4, sizeof(char));

}
void crearConfiguracion(){
	t_config *config = config_create("configuracionEsi.config");
	init_esiConfig();
	esiConfig->ipCoordi = strdup(config_get_string_value(config, "IP_COORDINADOR"));
	esiConfig->puertoCoordi = strdup(config_get_string_value(config, "PUERTO_COORDINADOR"));
	esiConfig->ipPlanificador = strdup(config_get_string_value(config,"IP_PLANIFICADOR"));
	esiConfig->puertoPlanificador = strdup(config_get_string_value(config, "PUERTO_PLANIFICADOR"));
	config_destroy(config);
}

/*void crearConfiguracion(esi_config** esiConfig, t_config ** config) {
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
*/
void abrirScript(char *argv[]) {

	script = fopen(argv[1], "r");
	if (script == NULL) {
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

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



