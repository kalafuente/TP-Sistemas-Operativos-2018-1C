#include <stdio.h>
#include <stdlib.h>
#include "Esi.h"
#include <parsi/parser.h>

int main(int argc, char **argv) {

	abrirScript(argv);
	logger = crearLogger("loggerEsi.log", "loggerEsi");

	//-------ARCHIVO DE CONFIGURACION

	config = config_create("configuracionEsi.config");
	 esiConfig = init_esiConfig();
	crearConfiguracion(esiConfig, config);

	//-------ARCHIVO DE CONFIGURACION


	conectarseAlCoordinador();
	//conectarseAlPlanificador();


	char*line=NULL;
	size_t len=0;
	ssize_t read;

	while ((read = getline(&line, &len, script)) != -1) {

	        t_esi_operacion parsed = parse(line);
	        PROTOCOLO_INSTRUCCIONES get = INSTRUCCION_GET;
        	PROTOCOLO_INSTRUCCIONES set = INSTRUCCION_SET;
        	PROTOCOLO_INSTRUCCIONES store= INSTRUCCION_STORE;

	        switch (parsed.keyword){
	        case GET:
	        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &get,socketCoordinador);
	        	enviarString(logger,parsed.argumentos.GET.clave,socketCoordinador);
				break;
	        case SET:
	        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &set,socketCoordinador);
	        	enviarString(logger, parsed.argumentos.SET.clave,socketCoordinador);
	        	enviarString(logger,parsed.argumentos.SET.valor,socketCoordinador);
	        	break;
	        case STORE:
	        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &store,socketCoordinador);
	        	enviarString(logger,parsed.argumentos.STORE.clave,socketCoordinador);
	        	break;
	        }


	        destruir_operacion(parsed);
	}

	fclose(script);
	free(line);

	close(socketCoordinador);
	close(socketPlani);
	destroy_esiConfig(esiConfig);
	config_destroy(config);
	return 0;

}

esi_config * init_esiConfig() {
	esiConfig = malloc(sizeof(esi_config));
	esiConfig->ipCoordi = string_new();
	esiConfig->puertoCoordi = string_new();
	esiConfig->ipPlanificador = string_new();
	esiConfig->puertoPlanificador = string_new();
	return esiConfig;
}
void crearConfiguracion(esi_config* esiConfig, t_config* config){
	string_append(&(esiConfig->ipCoordi), config_get_string_value(config, "IP_COORDINADOR"));
	string_append(&(esiConfig->puertoCoordi), config_get_string_value(config, "PUERTO_COORDINADOR"));
	string_append(&(esiConfig->ipPlanificador), config_get_string_value(config, "IP_PLANIFICADOR"));
	string_append(&(esiConfig->puertoPlanificador), config_get_string_value(config, "PUERTO_PLANIFICADOR"));
}

void destroy_esiConfig() {

	free(esiConfig->ipPlanificador);
	free(esiConfig->ipCoordi);
	free(esiConfig->puertoCoordi);
	free(esiConfig->puertoPlanificador);
	free(esiConfig);
}

void abrirScript(char *argv[]) {

	script = fopen(argv[1], "r");
	if (script == NULL) {
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

}

void conectarseAlCoordinador(){
	socketCoordinador = conectarseAlServidor(logger, &esiConfig->ipCoordi,&esiConfig->puertoCoordi);
	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;
	recibirMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES),&handshakeCoordi,socketCoordinador);
	PROTOCOLO_HANDSHAKE_CLIENTE handshakeESI = HANDSHAKE_CONECTAR_ESI_A_COORDINADOR;
	enviarMensaje(logger,sizeof(PROTOCOLO_HANDSHAKE_CLIENTE),&handshakeESI,socketCoordinador);
}

/*
void conectarseAlPlanificador(){
	socketPlani= conectarseAlServidor(logger, &esiConfig->ipPlanificador,&esiConfig->puertoPlanificador);
	PROTOCOLO_PLANIFICADOR_A_ESI handshakePlani;
	recibirMensaje(logger,sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),&handshakePlani,socketPlani);
	PROTOCOLO_ESI_A_PLANIFICADOR handshakeESI = HANDSHAKE_CONECTAR_ESI_A_PLANIFICADOR;
	enviarMensaje(logger,sizeof(PROTOCOLO_ESI_A_COORDINADOR),&handshakeESI,socketPlani);

}

*/


