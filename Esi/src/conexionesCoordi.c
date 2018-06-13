/*
 * conexionesCoordi.c
 *
 *  Created on: 13 jun. 2018
 *      Author: utnso
 */

#include "Esi.h"



void conectarseAlCoordinador() {
	socketCoordinador = conectarseAlServidor(logger, &esiConfig->ipCoordi,
			&esiConfig->puertoCoordi);
	if(socketCoordinador<0){

		abortarEsi("EL COORDNADOR ESTÁ EN CUALQUIERA");

	}
	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;
	recibirMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES),
			&handshakeCoordi, socketCoordinador);
	PROTOCOLO_HANDSHAKE_CLIENTE handshakeESI =
			HANDSHAKE_CONECTAR_ESI_A_COORDINADOR;
	enviarMensaje(logger, sizeof(PROTOCOLO_HANDSHAKE_CLIENTE), &handshakeESI,
			socketCoordinador);
	log_info(logger,"Me conecte al Coordinador :)");
}



void enviarInstruccionAlCoordinador(t_instruccion* instruccion){
	//PROTOCOLO_ESI_A_PLANIFICADOR resultado = TERMINE_BIEN;
	log_info(logger,"Enviando instruccion al Coordinador");
	if(enviarInstruccion(logger,instruccion,socketCoordinador)<0){
		abortarEsi("RIP Coordinador");
	}
	log_info(logger,"Se envió la instrucción");
	//enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
	//					socketPlani);

}



void recibirResultadoDelCoordiandor(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI * resultado){
	log_info(logger,"Esperando resultado de la operación");
	if(recibirMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI),resultado,socketCoordinador)<0){
		abortarEsi("RIP Coordi");
	}
	log_info(logger,"Resultado recibido");
}

