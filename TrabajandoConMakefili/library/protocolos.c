/*
 * protocolos.c
 *
 *  Created on: 6 jun. 2018
 *      Author: utnso
 */
#include "protocolos.h"

t_instruccion * recibirInstruccion(t_log* logger,int sock, char* deQuien)
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

	int resultado = recibirMensaje(logger, sizeof(instruccion), &instruccion, sock);
	if (resultado == -3){
		log_error(logger, "SE CORTÓ LA CONEXIÓN CON %s", deQuien);
		return NULL;
	}

	if( resultado <= 0 )
	{
		log_error(logger, "recibirInstruccion:: No se pudo recibir la Instruccion\n");
		return NULL;
	}

	log_info(logger, "recibirInstruccion:: Instruccion recibida\n");
	log_info(logger, "recibirInstruccion:: Pedimos la longitud de la clave\n");

	if(recibirMensaje(logger,sizeof(int32_t),&lenClave,sock) <= 0)
	{
		log_error(logger, "recibirInstruccion:: No se pudo recibir la longitud de la clave\n");
		return NULL;
	}

	log_info(logger, "recibirInstruccion:: Longitud de la clave recibida: %d\n", lenClave);
	log_info(logger, "recibirInstruccion:: Esperamos la clave\n");

	char key[lenClave];

	if(recibirMensaje(logger, lenClave, key, sock) <= 0)
	{
		log_error(logger, "recibirInstruccion:: No se pudo recibir la clave\n");
		return NULL;
	}

	log_info(logger, "recibirInstruccion:: Clave recibida: %s\n", key);

	if(instruccion == INSTRUCCION_SET)
	{
		log_info(logger, "recibirInstruccion:: Esperamos la longitud del valor a almacenar\n");

		if(recibirMensaje(logger, sizeof(lenValor), &lenValor, sock) <= 0)
		{
			log_error(logger, "recibirInstruccion:: No se pudo recibir la longitud del valor\n");
			return NULL;
		}

		log_info(logger, "recibirInstruccion:: Longitud del valor recibida: %d\n", lenValor);
		log_info(logger, "recibirInstruccion:: Esperamos el valor\n");

		char valor[lenValor];

		if(recibirMensaje(logger, lenValor, valor, sock) <= 0)
		{
			log_error(logger, "recibirInstruccion:: No se pudo recibir el valor\n");
			return NULL;
		}

		log_info(logger, "recibirInstruccion:: Valor recibido: %s\n", valor);
		log_info(logger, "recibirInstruccion:: Instruccion recibida correctamente\n");

		return crearInstruccion(instruccion, key, valor);
	}

	log_info(logger, "recibirInstruccion:: Instruccion recibida correctamente\n");
	return crearInstruccion(instruccion, key, NULL);

}

int enviarInstruccion(t_log* logger,t_instruccion* instruccion, int sock)
{
	int32_t lenClave = strlen(instruccion->clave)+1;
	//int32_t lenValor= strlen(instruccion->valor)+1;
	int resultado = enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion->instruccion,sock);
	if (resultado <= 0){
		if (resultado == 0){
			log_info(logger,"enviarInstruccion:: SE CORTÓ LA CONEXIÓN");
			return -3;
		}
		else
			log_info(logger,"enviarInstruccion:: ERROR AL ENVIAR PROTOCOLO");
			return -1;
	}

	int resultado2 = enviarMensaje(logger,sizeof(int32_t),&lenClave,sock);
	if (resultado2 <= 0){
			if (resultado == 0){
				log_info(logger,"enviarInstruccion:: SE CORTÓ LA CONEXIÓN");
				return -3;
			}
			if (resultado == -1){
				log_info(logger,"enviarInstruccion:: ERROR AL ENVIAR TAMAÑOCLAVE");
				return -1;
			}
		}

	int resultado3= enviarMensaje(logger,lenClave,instruccion->clave,sock);
	if (resultado3 <= 0){
				if (resultado == 0){
					log_info(logger,"enviarInstruccion:: SE CORTÓ LA CONEXIÓN");
					return -3;
				}
				if (resultado == -1){
					log_info(logger,"enviarInstruccion:: ERROR AL ENVIAR CLAVE");
					return -1;
				}
			}

	if(instruccion->instruccion == INSTRUCCION_SET)
	{
		int32_t lenValor= strlen(instruccion->valor)+1;
		enviarMensaje(logger,sizeof(int32_t),&lenValor,sock);
		enviarMensaje(logger,lenValor,instruccion->valor,sock);
	}
	return 1;
}

void enviarClave(t_log* logger,char* clave, int sock)
{
	int32_t lenClave = strlen(clave)+1;
	enviarMensaje(logger,sizeof(int32_t),&lenClave,sock);
	enviarMensaje(logger,lenClave,clave,sock);
}

char * recibirClave(t_log* logger,int sock, char * dondeGuardarClave)
{
	int32_t lenClave = 0;

	log_info(logger, "recibirClave:: Esperando la Instruccion\n");

	if(recibirMensaje(logger,sizeof(int32_t),&lenClave,sock) <= 0)
	{
		log_error(logger, "No se pudo recibir la longitud de la clave\n");
		return NULL;
	}

	log_info(logger, "recibirClave: Longitud de la clave recibida: %d\n", lenClave);
	log_info(logger, "recibirClave: Esperamos la clave\n");

	char key[lenClave];

	if(recibirMensaje(logger, lenClave, key, sock) <= 0)
	{
		log_error(logger, "recibirClave: No se pudo recibir la clave\n");
		return NULL;
	}

	log_info(logger, "recibirClave:: Clave recibida: %s\n", key);
	string_append(&dondeGuardarClave, key);
	return key;


}

t_instruccion * crearInstruccion(PROTOCOLO_INSTRUCCIONES tipoInstruccion, char * clave, char * valor)
{
	t_instruccion* instruccionACrear = malloc(sizeof(t_instruccion));
	instruccionACrear->instruccion = tipoInstruccion;
	instruccionACrear->clave = string_new();
	string_append(&(instruccionACrear->clave), clave);

	if(tipoInstruccion != INSTRUCCION_SET)
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

	if(instruccion->instruccion != INSTRUCCION_SET)
	{
		free(instruccion);
		return;
	}
	free(instruccion->valor);
	free(instruccion);
}


