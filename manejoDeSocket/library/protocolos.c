/*
 * protocolos.c
 *
 *  Created on: 6 jun. 2018
 *      Author: utnso
 */
#include "protocolos.h"

t_instruccion * recibirInstruccion(t_log* logger,int sock)
{
	int32_t lenClave = 0;
	int32_t lenValor = 0;
	//t_instruccion* instruccionAGuardar = malloc(sizeof(t_instruccion));

	PROTOCOLO_INSTRUCCIONES instruccion;

	/*
	recibirMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion,sock);
	instruccionAGuardar->instruccion=instruccion;

	recibirMensaje(logger,sizeof(int32_t),&lenClave,sock);
	instruccionAGuardar->clave=malloc(lenClave);
	recibirMensaje(logger,lenClave,instruccionAGuardar->clave,sock);

	recibirMensaje(logger,sizeof(int32_t),&lenValor,sock);
	instruccionAGuardar->valor=malloc(lenValor);
	recibirMensaje(logger,lenValor,instruccionAGuardar->valor,sock);
	return instruccionAGuardar;

	*/

	log_info(logger, "Esperando la Instruccion\n");

	if(recibirMensaje(logger, sizeof(instruccion), &instruccion, sock) <= 0)
	{
		log_error(logger, "No se pudo recibir la Instruccion\n");
		return NULL;
	}

	log_info(logger, "Instruccion recibida\n");
	log_info(logger, "Pedimos la longitud de la clave\n");

	if(recibirMensaje(logger,sizeof(int32_t),&lenClave,sock) <= 0)
	{
		log_error(logger, "No se pudo recibir la longitud de la clave\n");
		return NULL;
	}

	log_info(logger, "Longitud de la clave recibida: %d\n", lenClave);
	log_info(logger, "Esperamos la clave\n");

	char key[lenClave];

	if(recibirMensaje(logger, lenClave, key, sock) <= 0)
	{
		log_error(logger, "No se pudo recibir la clave\n");
		return NULL;
	}

	log_info(logger, "Clave recibida: %s\n", key);

	if(instruccion == INSTRUCCION_SET)
	{
		log_info(logger, "Esperamos la longitud del valor a almacenar\n");

		if(recibirMensaje(logger, sizeof(lenValor), &lenValor, sock) <= 0)
		{
			log_error(logger, "No se pudo recibir la longitud del valor\n");
			return NULL;
		}

		log_info(logger, "Longitud del valor recibida: %d\n", lenValor);
		log_info(logger, "Esperamos el valor\n");

		char valor[lenValor];

		if(recibirMensaje(logger, lenValor, valor, sock) <= 0)
		{
			log_error(logger, "No se pudo recibir el valor\n");
			return NULL;
		}

		log_info(logger, "Valor recibido: %s\n", valor);
		log_info(logger, "Instruccion recibida correctamente\n");

		return crearInstruccion(instruccion, key, valor);
	}

	log_info(logger, "Instruccion recibida correctamente\n");
	return crearInstruccion(instruccion, key, NULL);

}

void enviarInstruccion(t_log* logger,t_instruccion* instruccion, int sock)
{
	int32_t lenClave = strlen(instruccion->clave)+1;
	//int32_t lenValor= strlen(instruccion->valor)+1;
	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion->instruccion,sock);
	enviarMensaje(logger,sizeof(int32_t),&lenClave,sock);
	enviarMensaje(logger,lenClave,instruccion->clave,sock);

	if(instruccion->instruccion == INSTRUCCION_SET)
	{
		int32_t lenValor= strlen(instruccion->valor)+1;
		enviarMensaje(logger,sizeof(int32_t),&lenValor,sock);
		enviarMensaje(logger,lenValor,instruccion->valor,sock);
	}
}

t_instruccion * crearInstruccion(PROTOCOLO_INSTRUCCIONES tipoInstruccion, char * clave, char * valor)
{
	t_instruccion* instruccionACrear = malloc(sizeof(t_instruccion));
	instruccionACrear->instruccion = tipoInstruccion;
	instruccionACrear->clave = string_new();
	string_append(&(instruccionACrear->clave), clave);

	if(valor == NULL)
	{
		instruccionACrear->valor = NULL;
		return instruccionACrear;
	}

	instruccionACrear->valor=string_new();
	string_append(&(instruccionACrear->valor), valor);

	return instruccionACrear;
}


void destruirInstruccion(t_instruccion* instruccion)
{
	free(instruccion->clave);

	if(instruccion->valor == NULL)
	{
		free(instruccion);
		return;
	}
	free(instruccion->valor);
	free(instruccion);
}


