#include "comunicacionConCoordinador.h"

int conectarseAlCoordinador(planificador_config * planificadorConfig){

	log_info(logger, "El ip del Coordinador es: %s\n", planificadorConfig->ipCoordinador);
		log_info(logger, "El puerto del Coordinador es: %s\n", planificadorConfig->puertoCoordinador);

	int socketCoordinador = conectarseAlServidor(logger, &planificadorConfig->ipCoordinador, &planificadorConfig->puertoCoordinador);
		PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;
		recibirMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES), &handshakeCoordi, socketCoordinador);
		PROTOCOLO_HANDSHAKE_CLIENTE handshakePlani = HANDSHAKE_CONECTAR_PLANIFICADOR_A_COORDINADOR;
		enviarMensaje(logger, sizeof(PROTOCOLO_HANDSHAKE_CLIENTE), &handshakePlani,socketCoordinador);
	return socketCoordinador;
}
