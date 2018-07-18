#include "comunicacionConEsis.h"

void ordenarActuar(struct_esi* esi) {

	PROTOCOLO_PLANIFICADOR_A_ESI mensajeParaEsi = ACTUAR;

	if (enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
			&mensajeParaEsi, esi->socket) <= 0) {
		log_error(logger, "NO SE PUDO ENVIAR ACCION AL ESI");
	} else {
		log_info(logger, "Se envio accion al Esi");
	}

}
PROTOCOLO_ESI_A_PLANIFICADOR recibirResultado(struct_esi* esi) {
	PROTOCOLO_ESI_A_PLANIFICADOR resultado;
	recibirMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
			esi->socket);
	//recv + decodificar Mensaje
	return resultado;
}

void * recibirEsi(void* socketEscucha) {
	int listeningSocket = *(int*) socketEscucha;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente;
	PROTOCOLO_PLANIFICADOR_A_ESI handshakeEsi;
	PROTOCOLO_ESI_A_PLANIFICADOR hanshakeEP;
	while (PlanificadorON) {
		socketCliente = accept(listeningSocket, (struct sockaddr *) &addr,
				&addrlen);
		log_info(logger, "se conecto un esi");
		pthread_mutex_lock(&mutex);	//La condicion del while se puede reemplazar por otra cosa que permita dejar de recibir nuevos esis
		handshakeEsi = HANDSHAKE_CONECTAR_PLANIFICADOR_A_ESI;
		enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
				&handshakeEsi, socketCliente);
		recibirMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),
				&hanshakeEP, socketCliente);

		agregarEsi(socketCliente);
		pthread_mutex_unlock(&mutex);
		//signal
	}
	return NULL;

}
