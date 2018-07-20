#include "comunicacionConCoordinador.h"

int conectarseAlCoordinador(planificador_config * planificadorConfig,
		PROTOCOLO_HANDSHAKE_CLIENTE protocolo) {

	log_info(logger, "El ip del Coordinador es: %s\n", planificadorConfig->ipCoordinador);
		log_info(logger, "El puerto del Coordinador es: %s\n", planificadorConfig->puertoCoordinador);

	int socketCoordinador = conectarseAlServidor(logger, &planificadorConfig->ipCoordinador, &planificadorConfig->puertoCoordinador);
		PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;
		recibirMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES), &handshakeCoordi, socketCoordinador);
	PROTOCOLO_HANDSHAKE_CLIENTE handshakePlani = protocolo;
		enviarMensaje(logger, sizeof(PROTOCOLO_HANDSHAKE_CLIENTE), &handshakePlani,socketCoordinador);
	return socketCoordinador;
}

void * manejarConexionCoordi(void * socket) {
	int *socketCoordinador = (int*) socket;
	PROTOCOLO_PLANIFICADOR_A_COORDINADOR respuesta;
	PROTOCOLO_COORDINADOR_A_PLANIFICADOR mensajeRecibido;
	//int ID;
	char * CLAVE = string_new();
	struct_esi* ESI;
	int respuesta_bool;
	log_info(logger, "Iniciada recepcion de consultas del coordi");
	while (recibirMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR),
			&mensajeRecibido, *socketCoordinador) > 0) {

		switch (mensajeRecibido) {
		case PEDIDO_DE_ID:
			log_info(logger, "Cordi pidio ID");
			ESI = list_get(listaEjecutando, 0);
			enviarMensaje(logger, sizeof(int), &ESI->ID, *socketCoordinador);
			log_info(logger, "Le mande ID al Coordi");
			break;

		case PREGUNTA_ESI_TIENE_CLAVE:
			log_info(logger, "PREGUNTA_ESI_TIENE_CLAVE");

			CLAVE= recibirID( *socketCoordinador,logger);
			log_info(logger, "La clave es %s", CLAVE);
			respuesta_bool = perteneceClaveAlEsi(listaEsiClave, CLAVE);

			if (respuesta_bool)
				respuesta = ESI_TIENE_CLAVE;
			else
				respuesta = ESI_NO_TIENE_CLAVE;

			enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),
					&respuesta, *socketCoordinador);
			CLAVE = string_new();
			break;
		case PREGUNTA_CLAVE_DISPONIBLE:
			log_info(logger, "PREGUNTA_CLAVE_DISPONIBLE");
			CLAVE= recibirID(*socketCoordinador, logger);
			respuesta_bool = tieneAlgunEsiLaClave(listaEsiClave, CLAVE);
			if (respuesta_bool)
				respuesta = CLAVE_NO_DISPONIBLE;
			else
				respuesta = CLAVE_DISPONIBLE;
			enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),
					&respuesta, *socketCoordinador);

			CLAVE = string_new();
			break;

		default:
			log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
			break;
		}
	}
	log_error(logger, "SE CORTO LA CONEXION CON EL COORDI");

	return 0;
}
