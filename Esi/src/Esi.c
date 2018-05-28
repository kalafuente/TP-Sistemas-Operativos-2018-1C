#include <stdio.h>
#include <stdlib.h>
#include "Esi.h"
#include <parsi/parser.h>
#include <signal.h>
int main(int argc, char **argv) {

	abrirScript(argv);
	logger = crearLogger("loggerEsi.log", "loggerEsi");

	//-------ARCHIVO DE CONFIGURACION

	config = config_create("configuracionEsi.config");
	init_esiConfig();
	crearConfiguracion(esiConfig, config);

	//-------ARCHIVO DE CONFIGURACION

	conectarseAlCoordinador();
	conectarseAlPlanificador();
	procesarScript();

	cerrarConexion();
	killEsi();
	return 0;

}


void abrirScript(char *argv[]) {

	script = fopen(argv[1], "r");
	if (script == NULL) {
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

}

void enviarResultado(PROTOCOLO_ESI_A_PLANIFICADOR protocolo){
	enviarMensaje(logger,sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),&protocolo,socketPlani);
}



void enviarInstruccion(t_esi_operacion parsed){
	PROTOCOLO_INSTRUCCIONES instruccion;
	PROTOCOLO_ESI_A_PLANIFICADOR resultado = TERMINE_BIEN;
	switch (parsed.keyword){
	        		        case GET:
	        		        	instruccion=INSTRUCCION_GET;
	        		        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &instruccion,socketCoordinador);
	        		        	enviarString2(logger, parsed.argumentos.GET.clave,socketCoordinador);
	        		        	enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),
	        		        						&resultado, socketPlani);
	        					break;
	        		        case SET:
	        		        	instruccion=INSTRUCCION_SET;
	        		        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &instruccion,socketCoordinador);
	        		        	enviarString2(logger, parsed.argumentos.SET.clave,socketCoordinador);
	        		        	enviarString2(logger,parsed.argumentos.SET.valor,socketCoordinador);
	        		        	enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),
	        		        						&resultado, socketPlani);
	        		        	break;
	        		        case STORE:
	        		        	instruccion=INSTRUCCION_STORE;
	        		        	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES), &instruccion,socketCoordinador);
	        		        	enviarString2(logger,parsed.argumentos.STORE.clave,socketCoordinador);
	        		        	enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),
	        		        						&resultado, socketPlani);
	        		        	break;
	        		        }
}


void procesarScript(){
	char*line = NULL;
	size_t len = 0;
	ssize_t read;
	PROTOCOLO_PLANIFICADOR_A_ESI mensajeDelPlani;

	while ((read = getline(&line, &len, script)) != -1) {
			recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
							&mensajeDelPlani, socketPlani);

		        t_esi_operacion parsed = parse(line);

	        	enviarInstruccion(parsed);

		        destruir_operacion(parsed);
		}

		enviarResultado(TERMINE);
		fclose(script);
		free(line);
		log_info(logger,"Tengo el gusto de informarle que el script ha sido leido y "
				"parseado en todo su esplendor. Espero que haya disfrutado de mi servicio."
				" Saludos ");
}


void killEsi(){
	destroy_esiConfig();
	config_destroy(config);
	log_info(logger,"Hasta la vista, ESI");
}



