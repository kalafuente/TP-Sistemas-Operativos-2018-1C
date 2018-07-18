#include "comunicacionConPlanificador.h"

PROTOCOLO_PLANIFICADOR_A_COORDINADOR  estadoEsi(t_log* logger,PROTOCOLO_COORDINADOR_A_PLANIFICADOR estadoClave, int socketPlani, t_instruccion* instruccion){
	PROTOCOLO_PLANIFICADOR_A_COORDINADOR rtaPlani;
	if (enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR), &estadoClave,socketPlani)==-1){
		log_error (logger,"Se desconecto plani, SISTEMA EN ESTADO INVALIDO");
		destruirInstruccion(instruccion);
		killCoordinador();
		exit(1);
	}
	if (enviarID(logger, instruccion->clave , socketPlani)==-1){
		log_error (logger,"Se desconecto plani, SISTEMA EN ESTADO INVALIDO");
		destruirInstruccion(instruccion);
		killCoordinador();
		exit(1);
	}

	if (recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),&rtaPlani, socketPlani)==-1){
		log_error(logger, "Se desconecto plani, SISTEMA EN ESTADO INVALIDO");
		destruirInstruccion(instruccion);
		killCoordinador();
		exit(1);
	}
	return rtaPlani;
}

