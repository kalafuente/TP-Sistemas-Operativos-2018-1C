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

void conectarseAlPlanificador() {
	socketPlani = conectarseAlServidor(logger, &esiConfig->ipPlanificador,
			&esiConfig->puertoPlanificador);
	if(socketPlani<0){

		abortarEsi("EL PLANIFICADOR ESTÁ EN CUALQUIERA");
	}
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

void recibirOrdenDelPlanificador(PROTOCOLO_PLANIFICADOR_A_ESI* orden){
	log_info(logger,"Esperando la orden del planificador");
	if(recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
					&orden, socketPlani)<0){
		abortarEsi("RIP Planificador");
	}
	log_info(logger,"Orden recibida");
}


void evaluarRespuestaDelCoordinador(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI* resultado,
	t_instruccion*instruccion,PROTOCOLO_PLANIFICADOR_A_ESI *orden){


	PROTOCOLO_ESI_A_PLANIFICADOR estado;
	if(*resultado==TODO_OK_ESI){
	estado=TERMINE_BIEN;
		enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &estado,
				socketPlani);
		log_info(logger, "el estado enviado es %d", estado);
		enviarInstruccion(logger, instruccion, socketPlani);
	}
	else if(*resultado==BLOQUEATE){
		estado=BLOQUEADO_CON_CLAVE;
		log_info(logger,"La clave %s se encuentra bloqueada",instruccion->clave);
		enviarResultadoAlPlanificador(estado);
		enviarInstruccion(logger,instruccion,socketPlani);

		log_info(logger,"Se espera la orden del planificador para volver a enviar la instruccion");
		recibirOrdenDelPlanificador(orden);

		if(*orden!=FINALIZAR){
		enviarInstruccionAlCoordinador(instruccion);
		recibirResultadoDelCoordiandor(resultado);
		evaluarRespuestaDelCoordinador(resultado,instruccion,orden);
		}
	}
	else{
		estado=ERROR;
		if((*resultado)==ERROR_CLAVE_INACCESIBLE){

		log_error(logger,"Clave %s inaccesible",instruccion->clave);

		}
		else if((*resultado)==ERROR_CLAVE_NO_BLOQUEADA){
			log_error(logger,"Clave %s no bloquead",instruccion->clave);

		}
		else{
			log_error(logger,"Clave %s no identificada",instruccion->clave);
		}

		enviarResultadoAlPlanificador(estado);
		destruirInstruccion(instruccion);
		abortarEsi("GIL");
	}
}

void recibirResultadoDelCoordiandor(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI * resultado){
	log_info(logger,"Esperando resultado de la operación");
	if(recibirMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI),resultado,socketCoordinador)<0){
		abortarEsi("RIP Coordi");
	}
	log_info(logger,"Resultado recibido");
}

void enviarResultadoAlPlanificador(PROTOCOLO_ESI_A_PLANIFICADOR resultado){
	log_info(logger,"Enviando resultado de la operacion al Planificador ");
	if(enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
			socketPlani)<0){
		abortarEsi("RIP Plani");
	}
	log_info(logger,"Resultado enviado");
}

