/*
 * protocolos.c
 *
 *  Created on: 6 jun. 2018
 *      Author: utnso
 */
#include "protocolos.h"

instruccion* recibirInstruccion(t_log* logger,int sock){
	int32_t lenClave;
	int32_t lenValor;
	instruccion* instruccionAGuardar=malloc(sizeof(instruccion));

	PROTOCOLO_INSTRUCCIONES instruccion;
	recibirMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion,sock);
	instruccionAGuardar->instruccion=instruccion;

	recibirMensaje(logger,sizeof(int32_t),&lenClave,sock);
	instruccionAGuardar->clave=malloc(lenClave);
	recibirMensaje(logger,lenClave,instruccionAGuardar->clave,sock);

	recibirMensaje(logger,sizeof(int32_t),&lenValor,sock);
	instruccionAGuardar->valor=malloc(lenValor);
	recibirMensaje(logger,lenValor,instruccionAGuardar->valor,sock);
	return instruccionAGuardar;
}

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


