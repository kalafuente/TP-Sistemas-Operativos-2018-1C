#include "comunicacionConPlanificador.h"

PROTOCOLO_PLANIFICADOR_A_COORDINADOR  estadoEsi(PROTOCOLO_COORDINADOR_A_PLANIFICADOR estadoClave, int socketPlani, char * clave){
	PROTOCOLO_PLANIFICADOR_A_COORDINADOR rtaPlani;
	if (enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR), &estadoClave,socketPlani)==-1){
		printf ("Se desconecto plani, SISTEMA EN ESTADO INVALIDO");
		exit(1);
	}
	if (enviarClave(logger, clave, socketPlani)==-1){
		printf ("Se desconecto plani, SISTEMA EN ESTADO INVALIDO");
		exit(1);
	}

	if (recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),&rtaPlani, socketPlani)==-1){
		printf ("Se desconecto plani, SISTEMA EN ESTADO INVALIDO");
		exit(1);
	}
	return rtaPlani;
}

