#include <stdio.h>
#include <stdlib.h>
#include "Esi.h"
#include <parsi/parser.h>

int main(int argc, char **argv) {

	abrirScript(argv);
	logger = crearLogger("loggerEsi.log", "loggerEsi");
	crearConfiguracion();
	conectarseAlCoordinador();
	//conectarseAlPlanificador();
	t_esi_operacion * parsed = calloc(1,sizeof(t_esi_operacion ));

	char*line=NULL;
	size_t len=0;
	ssize_t read;

	while ((read = getline(&line, &len, script)) != -1) {

	        * parsed = parse(line);
	        PROTOCOLO_INSTRUCCIONES get = INSTRUCCION_GET;
        	PROTOCOLO_INSTRUCCIONES set = INSTRUCCION_SET;
        	PROTOCOLO_INSTRUCCIONES store= INSTRUCCION_STORE;

	        switch (parsed->keyword){
	        case GET:
	        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &get,socketCoordinador);
	        	enviarChar(logger,1,parsed->argumentos.GET.clave,socketCoordinador);

	        	//enviarMensaje(logger,100, & parsed->argumentos.GET.clave,socketCoordinador);
				break;
	        case SET:
	        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &set,socketCoordinador);
	        	enviarMensaje(logger,100,&parsed->argumentos.SET.clave,socketCoordinador);
	        	enviarMensaje(logger,100,&parsed->argumentos.SET.valor,socketCoordinador);
	        	break;
	        case STORE:
	        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &store,socketCoordinador);
	        	enviarMensaje(logger,100,&parsed->argumentos.STORE.clave,socketCoordinador);
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
	PROTOCOLO_ESI_A_COORDINADOR handshakeESI = HANDSHAKE_CONECTAR_ESI_A_COORDINADOR;
	enviarMensaje(logger,sizeof(PROTOCOLO_ESI_A_COORDINADOR),&handshakeESI,socketCoordinador);
}

void conectarseAlPlanificador(){
	socketPlani= conectarseAlServidor(logger, &esiConfig->ipPlanificador,&esiConfig->puertoPlanificador);
	PROTOCOLO_PLANIFICADOR_A_ESI handshakePlani;
	recibirMensaje(logger,sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),&handshakePlani,socketPlani);
	PROTOCOLO_ESI_A_PLANIFICADOR handshakeESI = HANDSHAKE_CONECTAR_ESI_A_PLANIFICADOR;
	enviarMensaje(logger,sizeof(PROTOCOLO_ESI_A_COORDINADOR),&handshakeESI,socketPlani);

}



