/*
 * old.c
 *
 *  Created on: 6 jun. 2018
 *      Author: utnso
 */

/*

void enviarInstruccion(t_esi_operacion parsed) {
	PROTOCOLO_INSTRUCCIONES instruccion;
	PROTOCOLO_ESI_A_PLANIFICADOR resultado = TERMINE_BIEN;
	switch (parsed.keyword) {
	case GET:
		instruccion = INSTRUCCION_GET;
		enviarMensaje(logger, sizeof(PROTOCOLO_INSTRUCCIONES), &instruccion,socketCoordinador);
		enviarString(logger, parsed.argumentos.GET.clave, socketCoordinador);
		enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,socketPlani);
		break;
	case SET:
		instruccion = INSTRUCCION_SET;
		enviarMensaje(logger, sizeof(PROTOCOLO_INSTRUCCIONES), &instruccion,socketCoordinador);
		enviarString(logger, parsed.argumentos.SET.clave, socketCoordinador);
		enviarString(logger, parsed.argumentos.SET.valor, socketCoordinador);
		enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
				socketPlani);
		break;
	case STORE:
		instruccion = INSTRUCCION_STORE;
		enviarMensaje(logger, sizeof(PROTOCOLO_INSTRUCCIONES), &instruccion,
				socketCoordinador);
		enviarString(logger, parsed.argumentos.STORE.clave, socketCoordinador);
		enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
				socketPlani);
		break;
	}
}
*/
