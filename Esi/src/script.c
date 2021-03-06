/*
 * script.c
 *
 *  Created on: 6 jun. 2018
 *      Author: utnso
 */

#include "Esi.h"

void abrirScript(int argc, char *argv[]) {
	if (argc < 2) {
		log_error(logger, "No pasaste el path del script");
		log_destroy(logger);
		exit(EXIT_FAILURE);
	}
	script = fopen(argv[1], "r");

	if (script == NULL) {
		log_error(logger, "Error al abrir el archivo: ");
		log_destroy(logger);
		exit(EXIT_FAILURE);
	}


	log_info(logger,"Se abrió el archivo %s",argv[1]);

}



t_instruccion* leerInstruccion(char* line) {

	t_esi_operacion parsed = parse(line);

	if (!parsed.valido) {
		free(line);
		enviarResultadoAlPlanificador(ERROR);
		destruir_operacion(parsed);
		//avisarAlCoordi(TERMINE_INSTRUCCIONES);
		abortarEsi("CLAVE INVALIDA");
	}
	t_instruccion* instruccion;
	if (parsed.keyword == GET) {
		instruccion = crearInstruccion(INSTRUCCION_GET,
				parsed.argumentos.GET.clave, "0");
	} else if (parsed.keyword == SET) {
		instruccion = crearInstruccion(INSTRUCCION_SET,
				parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
	} else {
		instruccion = crearInstruccion(INSTRUCCION_STORE,
				parsed.argumentos.STORE.clave, "0");
	}
	log_info(logger, "Se parseo la instruccion: <%s>", line);
	destruir_operacion(parsed);

	return instruccion;

}

void procesarScript() {
	char*line = NULL;
	size_t len = 0;
	ssize_t read;
	PROTOCOLO_PLANIFICADOR_A_ESI orden;
	PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI resultado;
	PROTOCOLO_ESI_A_COORDI coordi;

	while ((read = getline(&line, &len, script)) != -1) {

		recibirOrdenDelPlanificador(&orden,line);
		t_instruccion* inst = leerInstruccion(line);
		enviarInstruccionAlCoordinador(inst);
		recibirResultadoDelCoordiandor(&resultado);
		evaluarRespuestaDelCoordinador(resultado, inst);

		while (resultado == BLOQUEATE) {
			recibirOrdenDelPlanificador(&orden,line);
			enviarInstruccionAlCoordinador(inst);
			recibirResultadoDelCoordiandor(&resultado);
			evaluarRespuestaDelCoordinador(resultado, inst);
		}

	}
	log_info(logger, "No hay más para leer");

	//AVISO AL COORDI QUE TERMINE
	coordi = TERMINE_INSTRUCCIONES;

	enviarMensaje(logger, sizeof(coordi), &coordi, socketCoordinador);
	log_info(logger, "le dije que terminé al coordi");

	enviarResultadoAlPlanificador(TERMINE);
	log_info(logger, "le dije que terminé al plani");

	fclose(script);
	free(line);

}

