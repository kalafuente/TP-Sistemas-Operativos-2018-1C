#include "logDeOperaciones.h"

void registrarLogDeOperaciones(char* instruccion, char * clave, char * valor,t_log* logger,t_log* logDeoperaciones){

	PROTOCOLO_COORDINADOR_A_PLANIFICADOR pedidoID = PEDIDO_DE_ID;
	int rtaPlani;
	enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR), &pedidoID,socketPlani);
	recibirMensaje(logger, sizeof(int),&rtaPlani, socketPlani);
	log_info(logger,"el ID del esi segun plani fue recibido");
	log_info(logger,"el id es: %d",rtaPlani);
	if (!(strcmp(valor,"0")==0))
		log_info(logDeOperaciones,"ESI % d SET %s %s", rtaPlani, clave, valor);
	else
		log_info(logDeOperaciones,"ESI % d %s %s", rtaPlani,instruccion,clave);


}
