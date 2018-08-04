/*
 * conexiones.c
 *
 *  Created on: 27 may. 2018
 *      Author: utnso
 */

#include "Esi.h"

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




void recibirOrdenDelPlanificador(PROTOCOLO_PLANIFICADOR_A_ESI* orden,char*line){
	log_info(logger,"Esperando la orden del planificador");
	if(recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
					orden, socketPlani)<0){
		avisarAlCoordi(TERMINE);
		free(line);
		abortarEsi("RIP Planificador");

	}
	if(*orden==FINALIZAR){
		avisarAlCoordi(TERMINE);
		free(line);
		abortarEsi("PLANI KILLED ME");

	}

	log_info(logger,"Orden recibida");
}


void evaluarRespuestaDelCoordinador(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI resultado,
	t_instruccion*instruccion){


	PROTOCOLO_ESI_A_PLANIFICADOR estado;
	if(resultado==TODO_OK_ESI){
	estado=TERMINE_BIEN;
		enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &estado,
				socketPlani);
		log_info(logger, "el estado enviado es %d", estado);
		enviarInstruccion(logger, instruccion, socketPlani);
		destruirInstruccion(instruccion);

	}
	else if(resultado==BLOQUEATE){
		estado=BLOQUEADO_CON_CLAVE;
		log_info(logger,"La clave %s se encuentra bloqueada",instruccion->clave);
		enviarResultadoAlPlanificador(estado);
		enviarInstruccion(logger,instruccion,socketPlani);
		log_info(logger,"Se espera la orden del planificador para volver a enviar la instruccion");

	}
	else{
		estado=ERROR;
		if((resultado)==ERROR_CLAVE_INACCESIBLE){

		log_error(logger,"Clave %s inaccesible",instruccion->clave);

		}
		else if((resultado)==ERROR_CLAVE_NO_BLOQUEADA){
			log_error(logger,"Clave %s no bloquead",instruccion->clave);

		}
		else if ((resultado)==ERROR_NO_HAY_INSTANCIAS){
			log_error(logger,"NO HAY MAS INSTANCIAS");
		}
		else{
			log_error(logger,"Clave %s no identificada",instruccion->clave);
		}

		enviarResultadoAlPlanificador(estado);
		destruirInstruccion(instruccion);
		abortarEsi("");
	}
}



void enviarResultadoAlPlanificador(PROTOCOLO_ESI_A_PLANIFICADOR resultado){
	log_info(logger,"Enviando resultado de la operacion al Planificador ");
	if(enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
			socketPlani)<0){
		abortarEsi("RIP Plani");
	}
	log_info(logger,"Resultado enviado");
}

