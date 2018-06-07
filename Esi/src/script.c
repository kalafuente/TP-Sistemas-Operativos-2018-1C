/*
 * script.c
 *
 *  Created on: 6 jun. 2018
 *      Author: utnso
 */

#include "Esi.h"

void abrirScript(int argc, char *argv[]) {
	if (argc < 2) {
		perror("No pasaste el path del script la concha de tu madre");
		exit(1);
	}
	script = fopen(argv[1], "r");
	if (script == NULL) {
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

}


t_instruccion* leerInstruccion(char* line){
	t_esi_operacion parsed = parse(line);
	t_instruccion* instruccion;
	if(parsed.keyword == GET){
		instruccion= crearInstruccion(INSTRUCCION_GET,parsed.argumentos.GET.clave,"0");
	}
	else if(parsed.keyword ==SET){
		instruccion= crearInstruccion(INSTRUCCION_SET,parsed.argumentos.SET.clave,parsed.argumentos.SET.valor);
	}
	else{
		instruccion= crearInstruccion(INSTRUCCION_STORE,parsed.argumentos.STORE.clave,"0");
	}

	destruir_operacion(parsed);
	return instruccion;

}

void procesarScript() {
	char*line = NULL;
	size_t len = 0;
	ssize_t read;
	PROTOCOLO_PLANIFICADOR_A_ESI mensajeDelPlani;
	while ((read = getline(&line, &len, script)) != -1) {

		recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
				&mensajeDelPlani, socketPlani);

		t_instruccion* inst = leerInstruccion(line);
		enviarInstruccionAlCoordinador(inst);
		destruirInstruccion(inst);
		log_info(logger, "Se envió  la instruccion: %s", line);
	}
	recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
			&mensajeDelPlani, socketPlani);
	enviarResultado(TERMINE);

	fclose(script);
	free(line);
	log_info(logger,
			"Tengo el gusto de informarle que el script ha sido leido y "
					"parseado en todo su esplendor. Espero que haya disfrutado de mi servicio."
					" Saludos ");
}




