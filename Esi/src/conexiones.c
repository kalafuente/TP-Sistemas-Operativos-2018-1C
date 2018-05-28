/*
 * conexiones.c
 *
 *  Created on: 27 may. 2018
 *      Author: utnso
 */

#include "Esi.h"


void conectarseAlCoordinador() {
	socketCoordinador = conectarseAlServidor(logger, &esiConfig->ipCoordi,
			&esiConfig->puertoCoordi);
	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;
	recibirMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES),
			&handshakeCoordi, socketCoordinador);
	PROTOCOLO_HANDSHAKE_CLIENTE handshakeESI =
			HANDSHAKE_CONECTAR_ESI_A_COORDINADOR;
	enviarMensaje(logger, sizeof(PROTOCOLO_HANDSHAKE_CLIENTE), &handshakeESI,
			socketCoordinador);
	log_info(logger,"Me conecte al Coordinador :)");
}

void conectarseAlPlanificador() {
	socketPlani = conectarseAlServidor(logger, &esiConfig->ipPlanificador,
			&esiConfig->puertoPlanificador);
	PROTOCOLO_PLANIFICADOR_A_ESI handshakePlani;
	recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
			&handshakePlani, socketPlani);
	PROTOCOLO_ESI_A_PLANIFICADOR handshakeESI =
			HANDSHAKE_CONECTAR_ESI_A_PLANIFICADOR;
	enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &handshakeESI,
			socketPlani);
	log_info(logger,"Me conecte al Planificador :)");
}

void cerrarConexion(){

	close(socketCoordinador);
	log_info(logger,"Se cierra la conexion con el coordinador");
	close(socketPlani);
	log_info(logger,"Se cierra la conexion con el planificador");
}
