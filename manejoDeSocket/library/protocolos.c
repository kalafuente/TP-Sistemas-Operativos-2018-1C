#include "protocolos.h"

t_instruccion * recibirInstruccion(t_log* logger,int sock, char* deQuien)
{
	int32_t lenClave = 0;
	int32_t lenValor = 0;

	PROTOCOLO_INSTRUCCIONES instruccion;

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

	// --------------------- ESTO AGREGUE --------------------------

	if(instruccion == PEDIDO_VALOR || instruccion == COMPACTAR)
	{
		t_instruccion* instruccionACrear = malloc(sizeof(t_instruccion));
		instruccionACrear->instruccion = instruccion;
		instruccionACrear->clave = NULL;
		instruccionACrear->valor = NULL;

		return instruccionACrear;
	}

	// --------------------- HASTA ACA -------------------------

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
			log_info(logger,"enviarInstruccion:: ERROR AL ENVIAR PROTOCOLO");
			return -1;
	}

	//------------------ ESTO ES LO QUE AGREGUE ---------------------

	if(instruccion->instruccion == PEDIDO_VALOR || instruccion->instruccion == COMPACTAR)
	{
		return 1;
	}

	// ----------------- HASTA ACA -----------------------

	int resultado2 = enviarMensaje(logger,sizeof(int32_t),&lenClave,sock);

	if (resultado2 <= 0){
				log_info(logger,"enviarInstruccion:: ERROR AL ENVIAR TAMAÑOCLAVE");
				return -1;
			}
	int resultado3= enviarMensaje(logger,lenClave,instruccion->clave,sock);

	if (resultado3 <= 0){
					log_info(logger,"enviarInstruccion:: ERROR AL ENVIAR CLAVE");
					return -1;
	}

	if(instruccion->instruccion == INSTRUCCION_SET)
	{
		int32_t lenValor= strlen(instruccion->valor)+1;
		enviarMensaje(logger,sizeof(int32_t),&lenValor,sock);
		enviarMensaje(logger,lenValor,instruccion->valor,sock);
	}
	return 1;
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
char* recibirID(int sock, t_log* logger) {
	int32_t lenClave = 0;
	recibirMensaje(logger,sizeof(int32_t),&lenClave,sock);
	char* buffer;

	if (lenClave < 0) {
		log_error(logger,"Error al recibir tamaño");
		return NULL;
	}
	if(lenClave == 0 ){
		return NULL;
	}

	buffer = malloc(lenClave+1);

	if (recv(sock, buffer,lenClave, MSG_WAITALL) < 0) {
		log_error(logger,"Error al recibir string");
		free(buffer);
		return NULL;
	}

	buffer[lenClave] = '\0';

	return buffer;
}
int enviarID(int sock ,char* id, t_log* logger){


	int total =0;
	int lenClave = string_length(id);
	int pendiente = lenClave;
	char* string = string_duplicate(id);

	if (enviarMensaje(logger,sizeof(int32_t),&lenClave,sock) < 0) {
				log_error (logger, "No se pudo enviar tamaño");
				free(string);
				return -1;
	}

	while (total < pendiente) {

	int enviado = send(sock, string, lenClave, MSG_NOSIGNAL);
		if (enviado < 0) {
			free(string);
			return -1;
		}
		total += enviado;
		pendiente -= enviado;
	}
    free(string);
    return total;
}
