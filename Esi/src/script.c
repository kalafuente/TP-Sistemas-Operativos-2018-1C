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
		exit(EXIT_FAILURE);
	}
	script = fopen(argv[1], "r");
	if (script == NULL) {
		perror("Error al abrir el archivo: ");
		exit(EXIT_FAILURE);
	}

}


t_instruccion* leerInstruccion(char* line){

	t_esi_operacion parsed = parse(line);

	if(! parsed.valido){
		free(line);
		fclose(script);
		enviarResultadoAlPlanificador(ERROR);
		abortarEsi("CLAVE INVALIDA");
	}
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
	log_info(logger, "Se parseo la instruccion: <%s>", line);
	destruir_operacion(parsed);

	return instruccion;

}

void procesarScript() {
	char*line;;
	size_t len = 0;
	ssize_t read;
	PROTOCOLO_PLANIFICADOR_A_ESI orden;
	PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI resultado;
	recibirOrdenDelPlanificador(&orden);

	PROTOCOLO_ESI_A_COORDI coordi= MANDO_INTRUCCIONES;

	while ((read = getline(&line, &len, script)) != -1 && orden!=FINALIZAR) {

		t_instruccion* inst = leerInstruccion(line);
		enviarMensaje(logger,sizeof(coordi),&coordi, socketCoordinador);
		enviarInstruccionAlCoordinador(inst);
		recibirResultadoDelCoordiandor(&resultado);
		evaluarRespuestaDelCoordinador(resultado,inst,orden);
		if(orden!=FINALIZAR){
		recibirOrdenDelPlanificador(&orden);
		}
	}
	if(orden!=FINALIZAR){
	log_info(logger,"no hay más para leer");
		//recibirOrdenDelPlanificador(&orden);
	}else{
		log_info(logger,"El Plani me aborto");
	}



	//AVISO AL COORDI QUE TERMINE
	coordi= TERMINE_INSTRUCCIONES;

	enviarMensaje(logger, sizeof(coordi), &coordi,socketCoordinador);
	log_info(logger, "le dije que terminé al coordi");

	enviarResultadoAlPlanificador(TERMINE);
	log_info(logger,"le dije que terminé al plani");

	fclose(script);
	free(line);

}




