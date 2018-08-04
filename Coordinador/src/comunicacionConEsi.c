#include "comunicacionConEsi.h"

void enviarRespuestaAlEsi(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rta, int sock, t_log* logger){
	enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rta, sock);
	switch(rta){
	case TODO_OK_ESI:
		log_info(logger, "Le dije al esi que todo ok, pues CLAVE DISPONIBLE");
		break;

	case ERROR_CLAVE_INACCESIBLE:
		log_info(logger, "Le dije al esi CLAVE INACCESIBLE");
		break;

	case BLOQUEATE:
		log_info(logger, "Le dije al esi BLOQUEATE");
		break;

	case ERROR_CLAVE_NO_BLOQUEADA:
		log_info(logger, "Le dije al esi que NO TIENE CLAVE");
		break;
	case ERROR_CLAVE_NO_IDENTIFICADA:
		log_info(logger, "La lista de claves no contiene esta clave");
		break;
	default:
		break;

}
}

t_instruccion* recibirInstruccionDelEsi(int sock){
	t_instruccion* instruccionAGuardar=recibirInstruccion(logger,sock,"ESI");

	if (instruccionAGuardar != NULL) {
		switch(instruccionAGuardar->instruccion){
					case INSTRUCCION_GET:
						registrarLogDeOperaciones("GET", instruccionAGuardar->clave,"0", logger, logDeOperaciones);

						break;
					case INSTRUCCION_SET:
						registrarLogDeOperaciones("SET", instruccionAGuardar->clave,instruccionAGuardar->valor,logger, logDeOperaciones);
						break;

					case INSTRUCCION_STORE:
						registrarLogDeOperaciones("STORE", instruccionAGuardar->clave,"0",logger, logDeOperaciones);
						break;

	}
	}

	return instruccionAGuardar;
}

