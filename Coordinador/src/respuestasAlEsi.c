#include "respuestasAlESI.h"

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


}
}

