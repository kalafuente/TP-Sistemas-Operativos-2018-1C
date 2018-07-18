#include "comunicacionConInstancia.h"

bool enviarSETaInstancia(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible){

	if (enviarInstruccion(logger, instruccion, instanciaALlamar->socket)==-1)
	{
		if (avisarClaveInaccesible){
			instanciaCaida(instruccion->clave, sock);
		}
		eliminarInstancia(instanciaALlamar->socket, listaDeInstancias);
		return false;
	}

	else{

	PROTOCOLO_INSTANCIA_A_COORDINADOR rtaInstancia;
	recibirMensaje(logger,sizeof(rtaInstancia),&rtaInstancia, instanciaALlamar->socket);

	if (rtaInstancia == SE_PUDO_GUARDAR_VALOR) {

		log_info(logger, "instancia guardo valor");
		modificarInstanciaListaDeClavesConInstancia(instruccion->clave,instanciaALlamar, listaDeClavesConInstancia);
		enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
		log_info(logControlDeDistribucion,"Set enviado a Instancia:  % d", instanciaALlamar->socket);
		}
	else
	log_info(logger, "instancia no guardo valor");
	//-----------------------no sé que pasaría acá
	}

	int32_t entradasEnUsoDeLaInstancia;
	recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->socket);
	registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
	return true;
}



void mandarConfiguracionAInstancia(int sock){

	PROTOCOLO_COORDINADOR_A_INSTANCIA entradas = ENTRADAS;
	enviarMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&entradas,sock);
	enviarMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&(coordConfig->entradas),sock);

	log_info(logger, "Envie cantidad de entradas a la instancia");
	enviarMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&(coordConfig->tamanioEntradas),sock);
	log_info(logger, "Envie tamanaño de entradas a la instancia");

}

void enviarClavesCorrespondientes(int sock,char * id, t_list* listaDeClavesConInstancia){
	PROTOCOLO_INSTANCIA_A_COORDINADOR pedidoDeClave;
	recibirMensaje(logger,sizeof(pedidoDeClave),&pedidoDeClave,sock);
	t_list* clavesDeEstaInstancia = list_create();
	clavesDeEstaInstancia = clavesDeLaInstancia(listaDeClavesConInstancia, id);

	if (pedidoDeClave == PEDIDO_DE_CLAVES){

		if (sizeof(clavesDeEstaInstancia) == 0){
				enviarID(sock,"null",logger);
		}
		else{
			void enviar(claveConInstancia* elem){
				enviarID(sock,elem->clave,logger);
			}
		list_iterate(clavesDeEstaInstancia, (void *) enviar);
		enviarID(sock,"null",logger);
		}

	}
	list_destroy(clavesDeEstaInstancia);
}

t_list* clavesDeLaInstancia(t_list* list, void* value){

	bool equals(claveConInstancia* item) {
		int rta = strcmp(value, item->instancia->identificador);
		if (rta == 0)
				return true;
		else
				return false;
	}

	return list_filter(list, (void *) equals);
}


