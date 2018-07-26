#include "comunicacionConInstancia.h"

bool enviarSETaInstancia(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible){
	int32_t entradasEnUsoDeLaInstancia;


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
		PROTOCOLO_INSTANCIA_A_COORDINADOR rtaCompactar;
		recibirMensaje(logger,sizeof(rtaInstancia),&rtaInstancia, instanciaALlamar->socket);
		instancia* instanciaNUEVAALlamar;
		switch (rtaInstancia){


			case SE_PUDO_GUARDAR_VALOR:
							log_info(logger, "instancia guardo valor");
							modificarInstanciaListaDeClavesConInstancia(instruccion->clave,instanciaALlamar, listaDeClavesConInstancia);
							enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
							log_info(logControlDeDistribucion,"Set enviado a Instancia:  % d", instanciaALlamar->socket);
							recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->socket);
							registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
							break;


			case NO_SE_PUDO_GUARDAR_VALOR:
							instanciaNUEVAALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);
								if (instanciaNUEVAALlamar == NULL){
									log_error(logger, "NO HAY MÁS INSTANCIAS");
									destruirInstruccion(instruccion);
									killCoordinador();
									exit(1);
								}
								else{
									bool seEnvio = enviarSETaInstancia(instanciaNUEVAALlamar,sock, instruccion, false);
											while(!seEnvio){
											instanciaNUEVAALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);
											seEnvio = enviarSETaInstancia(instanciaNUEVAALlamar,sock, instruccion, false);
											}
								}
							break;


			case SE_NECESITA_COMPACTAR:
							log_info(logger,"me llegó que se necesita compactar");
							pedirCompactar(listaDeInstancias,instruccion);
							recibirMensaje(logger,sizeof(rtaCompactar),&rtaCompactar, instanciaALlamar->socket);
							if (rtaCompactar != SE_PUDO_GUARDAR_VALOR){
								log_error(logger, "NO ME LLEGO SE_PUDO_GUARDAR_VALOR DSP DE MANDAR LA COMPACTACION");
								destruirInstruccion(instruccion);
								killCoordinador();
								exit(1);
							}
							recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->socket);
							registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
							break;

			default:
							log_error(logger, "ERROR EN RTA AL SET");
							destruirInstruccion(instruccion);
							killCoordinador();
							exit(1);
							break;

			}
	}

	return true;
}




void pedirCompactar(t_list* lista,t_instruccion * instruccion){
	t_instruccion * falsa = malloc (sizeof(t_instruccion));
	falsa->instruccion = COMPACTAR;
	falsa->clave = "null";
	falsa->valor = "null";

	if (list_size(lista)==0){
			printf("No hay instancias \n");
		}
	else {
		void compactar(instancia * elem){
			if (enviarInstruccion(logger,falsa,elem->socket)==-1){
				log_error (logger, "no se pudo pedir compactar");
			}
			int32_t entradasEnUsoDeLaInstancia;
			PROTOCOLO_INSTANCIA_A_COORDINADOR rta;
			recibirMensaje(logger,sizeof(rta),&rta, elem->socket);
			if (rta != COMPACTACION_EXITOSA){
				log_error(logger, "LA COMPACTACTACIÓN NO SALIÓ BIEN O NO LLEGÓ EL MSJ COMPACTACION_EXITOSA, una pena, pero metanle garra a la proxima entrega no todo está perdido... o sí");
				destruirInstruccion(instruccion);
				free (falsa);
				killCoordinador();
				exit(1);
			}
			recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, elem->socket);
			registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,elem);

		}
			list_iterate(lista, (void *) compactar);
		}

	free (falsa);
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


