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
				log_info(logControlDeDistribucion,"Set enviado a Instancia:  % d", instanciaALlamar->socket);
				recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->socket);
				registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
				enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
				break;


			case NO_SE_PUDO_GUARDAR_VALOR:
				registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
				instanciaNUEVAALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);
				bool seEnvio = enviarSETaInstancia(instanciaNUEVAALlamar,sock, instruccion, false);

				while(!seEnvio){
				instanciaNUEVAALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);
				seEnvio = enviarSETaInstancia(instanciaNUEVAALlamar,sock, instruccion, false);
				}

				break;


			case SE_NECESITA_COMPACTAR:
				log_info(logger,"INSTANCIA NECESITA COMPACTAR");

				pthread_mutex_lock(&mutexCompactacion);
				pedirCompactar(listaDeInstancias);
				recibirMensaje(logger,sizeof(rtaCompactar),&rtaCompactar, instanciaALlamar->socket);
					if (rtaCompactar != SE_PUDO_GUARDAR_VALOR){
						log_error(logger, "NO ME LLEGO SE_PUDO_GUARDAR_VALOR DSP DE MANDAR LA COMPACTACION");
						destruirInstruccion(instruccion);
						killCoordinador();
						exit(1);
				}
				log_info(logger,"instancia guardo valor");
				modificarInstanciaListaDeClavesConInstancia(instruccion->clave,instanciaALlamar, listaDeClavesConInstancia);
				log_info(logControlDeDistribucion,"Set enviado a Instancia:  % d", instanciaALlamar->socket);
				recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->socket);
				registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
				log_info(logger,"------FIN DE COMPACTACIÓN------");
				enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
				pthread_mutex_unlock(&mutexCompactacion);

				break;

			default:
				//log_error(logger, "ERROR EN RTA AL SET");
				eliminarClave(listaDeClavesConInstancia, instruccion->clave);
				eliminarInstancia(instanciaALlamar->socket, listaDeInstancias);


				instanciaNUEVAALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);
				bool seEnvio2 = enviarSETaInstancia(instanciaNUEVAALlamar,sock, instruccion, false);

				while(!seEnvio2){
					instanciaNUEVAALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);
					seEnvio2 = enviarSETaInstancia(instanciaNUEVAALlamar,sock, instruccion, false);
				}
				break;
			}
	}

	return true;
}

bool enviarSETaInstanciaConSETPrevio(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible){
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
				log_info(logControlDeDistribucion,"Set enviado a Instancia:  % d", instanciaALlamar->socket);
				recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->socket);
				registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
				enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
				break;


			case NO_SE_PUDO_GUARDAR_VALOR:
				registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
				enviarRespuestaAlEsi(ERROR_NO_HAY_INSTANCIAS, sock, logger);
				log_warning(logger, "Se aborta al esi porque no se puede sobreescribir el valor de la clave debido a que el nuevo valor ocupa más ENTRADAS que el viejo, y eso no debería ser posible por enunciado. ISSUE #1182");
				break;


			case SE_NECESITA_COMPACTAR:
				log_info(logger,"INSTANCIA NECESITA COMPACTAR");

				pthread_mutex_lock(&mutexCompactacion);
				pedirCompactar(listaDeInstancias);
				recibirMensaje(logger,sizeof(rtaCompactar),&rtaCompactar, instanciaALlamar->socket);
					if (rtaCompactar != SE_PUDO_GUARDAR_VALOR){
						log_error(logger, "NO ME LLEGO SE_PUDO_GUARDAR_VALOR DSP DE MANDAR LA COMPACTACION");
						destruirInstruccion(instruccion);
						killCoordinador();
						exit(1);
				}
				log_info(logger,"instancia guardo valor");
				modificarInstanciaListaDeClavesConInstancia(instruccion->clave,instanciaALlamar, listaDeClavesConInstancia);
				log_info(logControlDeDistribucion,"Set enviado a Instancia:  % d", instanciaALlamar->socket);
				recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->socket);
				registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaALlamar);
				log_info(logger,"------FIN DE COMPACTACIÓN------");
				enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
				pthread_mutex_unlock(&mutexCompactacion);

				break;

			default:
				log_error(logger, "ERROR EN RTA AL SET");
				break;
			}
	}

	return true;
}


void pedirCompactar(t_list* lista){
	printf ("pedir compactar \n");
	if (list_size(lista)==0){
			printf("No hay instancias \n");
		}
	else {
			void crearHilosCompactacion (instancia * elemento) {
				pthread_t compactacion_id;
				pthread_create(&compactacion_id, NULL, compactar, (void*) elemento);
			}
			list_iterate(lista, (void *) crearHilosCompactacion);

			//sleep(10); //para probar más facil qué tal los mutex


			for(int i =0; i< list_size(listaDeInstancias); i++){
				sem_wait(&terminoCompactacion);
			}
		}

}


void *compactar(void *elemento){

	instancia* elem = (instancia*) elemento;
	t_instruccion * falsa = malloc (sizeof(t_instruccion));
	falsa->instruccion = COMPACTAR;
	falsa->clave = "null";
	falsa->valor = "null";

		if (enviarInstruccion(logger,falsa,elem->socket)==-1){
			log_error (logger, "no se pudo pedir compactar, quizás la instancia se desconectó");
			sem_post(&terminoCompactacion);
			pthread_exit(EXIT_SUCCESS);
		}
		int32_t entradasEnUsoDeLaInstancia;
		PROTOCOLO_INSTANCIA_A_COORDINADOR rta;
		if (recibirMensaje(logger,sizeof(rta),&rta, elem->socket)>0){
			if (rta == COMPACTACION_EXITOSA){
				recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, elem->socket);
				log_info(logger, "compactacion exitosa");
				registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,elem);
				log_info(logger, "entradas registradas");
		}
		else{
			log_error(logger, "LA COMPACTACTACIÓN NO SALIÓ BIEN"); //no debería pasar
			}
		}
		else{
			log_warning(logger, "instancia desconectada, por lo tanto no hubo compactación en esa instancia");
		}

	free (falsa);
	sem_post(&terminoCompactacion);
	//printf ("hice signal \n");
	pthread_exit(EXIT_SUCCESS);

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
		if (item->instancia == NULL){
			return false;
		}
		else{
			int rta = strcmp(value, item->instancia->identificador);
			if (rta == 0)
						return true;
			else
						return false;
		}

	}

	return list_filter(list, (void *) equals);
}


