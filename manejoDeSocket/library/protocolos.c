/*
 * protocolos.c
 *
 *  Created on: 6 jun. 2018
 *      Author: utnso
 */
#include "protocolos.h"


void enviarInstruccion(t_log* logger,instruccion*instruccion, int sock) {
	int32_t lenClave = strlen(instruccion->clave)+1;
	int32_t lenValor= strlen(instruccion->valor)+1;
	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion->instruccion,sock);
	enviarMensaje(logger,sizeof(int32_t),&lenClave,sock);
	enviarMensaje(logger,lenClave,instruccion->clave,sock);
	enviarMensaje(logger,sizeof(int32_t),&lenValor,sock);
	enviarMensaje(logger,lenValor,instruccion->valor,sock);
}


void destruirInstruccion(instruccion*instruccion){
	free(instruccion->clave);
	free(instruccion->valor);
	free(instruccion);
}


