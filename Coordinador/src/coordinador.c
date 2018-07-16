#include "coordinador.h"


int main(int argc, char **argv) {

	prepararLoggers();
	prepararConfiguracion();
	crearListas();
	crearServidor();
	cerrarTodo();
	return 0;

}

void cerrarTodo(){
	close(listenningSocket);
	destroy_coordConfig(coordConfig);
	config_destroy(config);

}

void prepararLoggers(){
	logger= crearLogger("loggerCoordi.log","loggerCoordi");
	logDeOperaciones = crearLogger("logDeOperaciones.log", "logDeOperaciones");
	logControlDeDistribucion = crearLogger("logControlDeDistribucion.log", "logControlDeDistribucion");
}
void prepararConfiguracion(){
	config = config_create("configuracion.config");
	coordConfig = init_coordConfig();
	crearConfiguracion(coordConfig,config);
}

void crearListas(){
	listaDeInstancias= list_create();
	listaDeClavesConInstancia= list_create();
}

void crearServidor(){
	listenningSocket = crearSocketQueEscucha(&coordConfig->puerto, &coordConfig->entradas);
	crearServidorMultiHilo();
}

void crearServidorMultiHilo() {
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int socketCliente;
	pthread_t thread_id;


	while ((socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,	&addrlen))) {
		puts("Cliente conectado. Esperando mensajes prueba:\n");

		if (pthread_create(&thread_id, NULL, manejadorDeConexiones, (void*) &socketCliente) < 0) {
			perror("No se pudo crear el hilo");
			exit(1);
		}

		puts("Manejador de conexiones asignado");
	}

	if (socketCliente < 0) {
		perror("falló la aceptación");
		exit(1);
	}

	puts("Damos otra vuelta");
}

void *manejadorDeConexiones(void *socket_desc) {

	int sock = *(int*) socket_desc;
	t_instruccion* instruccionAGuardar;

	saludar(sock);
	PROTOCOLO_HANDSHAKE_CLIENTE handshake = recibirSaludo(sock);


	//----------IDENTIFICO A QUIEN SE ME CONECTA
	switch(handshake){


	case HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR:
		log_info(logger, "Se me conectó una Instancia");
		mandarConfiguracionAInstancia(sock);
		registrarInstancia(sock);
		mostrarListaIntancias(listaDeInstancias);
		break;


	case HANDSHAKE_CONECTAR_ESI_A_COORDINADOR:
		log_info(logger, "Se me conectó un Esi");
		cantEsi++;
		//recibirInstruccion(sock, &instruccionAGuardar);
		PROTOCOLO_ESI_A_COORDI esi;

		recibirMensaje(logger,sizeof(esi), &esi, sock);
		instruccionAGuardar=recibirInstruccionDelEsi(sock);

		while (esi == MANDO_INTRUCCIONES && instruccionAGuardar != NULL) {

			procesarInstruccion(instruccionAGuardar,sock);
			retardo();

			destruirInstruccion(instruccionAGuardar);
			recibirMensaje(logger,sizeof(esi), &esi, sock);
			instruccionAGuardar=recibirInstruccionDelEsi(sock);
		}

		if (esi == TERMINE_INSTRUCCIONES){
			log_info(logger, "ESI TERMINÓ DE MANDAR LAS INSTRUCCIONES, YUPI");
		}

		if (instruccionAGuardar== NULL){
			log_info(logger, "no me puedo conectar con esi");
		}
		break;

		case HANDSHAKE_CONECTAR_PLANIFICADOR_A_COORDINADOR:
			log_info(logger, "Se me conectó el planificador");
			socketPlani = sock;
			printf("\n socket: %d", sock);
			printf("\n socketguardado: %d", socketPlani);
		break;

	}

	//close(sock)
	printf("\n termino el hilo\n ");
	return NULL;


}

void procesarInstruccion(t_instruccion * instruccion, int sock){

	log_info(logger,"entro a procesarInstruccion");
	claveConInstancia* instanciaConLaClave;

	switch(instruccion->instruccion){
	case INSTRUCCION_GET:
				log_info(logger, "ESI ENVIO UN GET");
				if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
					log_info(logger, "La lista de claves contiene este GET");
					printf ("\n mostrar lista\n ");
					mostrarLista(listaDeClavesConInstancia);
				switch(estadoEsi(PREGUNTA_CLAVE_DISPONIBLE, socketPlani,instruccion->clave)){
					case CLAVE_DISPONIBLE:
						enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
						break;
					case CLAVE_NO_DISPONIBLE:
						enviarRespuestaAlEsi(BLOQUEATE, sock, logger);
						break;
					default:
						log_error(logger, "ERROR MENSAJE NO ANTICIPADO ");
						break;
					}

				}

				else{

					log_info(logger, "La lista de claves NO contiene este GET");
					claveConInstancia* clavenueva =  nuevaClaveConInstancia(instruccion->clave);
					list_add(listaDeClavesConInstancia, clavenueva);
					log_info(logger, "Se agrego esta clave: %s", clavenueva->clave);

					printf ("\n mostrar lista\n ");
					mostrarLista(listaDeClavesConInstancia);
					enviarRespuestaAlEsi(TODO_OK_ESI, sock, logger);
				}

	break;
	case INSTRUCCION_SET:
				log_info(logger, "ESI ENVIO UN SET");
				if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
					log_info(logger, "La lista de claves contiene este GET");
				switch(estadoEsi(PREGUNTA_ESI_TIENE_CLAVE, socketPlani,instruccion->clave)){
					case ESI_TIENE_CLAVE:
						log_info(logger,"ESI TIENE CLAVE");

						instancia * instanciaQueTieneSetClave = instanciaQueTieneLaClave(instruccion->clave)->instancia;

						 if (instanciaQueTieneSetClave == NULL){
							 log_info(logControlDeDistribucion,"NO HAY SET PREVIO DE ESTA CLAVE");
							 instancia* instanciaALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave);
							 bool seEnvio = enviarSETaInstancia(instanciaALlamar,sock, instruccion, false);
							 while(!seEnvio){
								 instanciaALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave);
								 seEnvio = enviarSETaInstancia(instanciaALlamar,sock, instruccion, false);
							 }
						 }
						 else {
						 log_info(logControlDeDistribucion,"HAY SET PREVIO DE ESTA CLAVE, LA CLAVE ESTÁ EN EL SOCKET: %d",instanciaQueTieneSetClave->socket );
						 enviarSETaInstancia(instanciaQueTieneSetClave,sock, instruccion, true);
						 }

					break;

					case ESI_NO_TIENE_CLAVE:
						enviarRespuestaAlEsi(ERROR_CLAVE_NO_BLOQUEADA, sock, logger);

						break;
					default:
						log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
						break;
					}
				}
				else{
					enviarRespuestaAlEsi(ERROR_CLAVE_NO_IDENTIFICADA, sock, logger);
				}
	break;

	case INSTRUCCION_STORE:

				log_info(logger, "ESI ENVIO UN STORE");
				if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
				log_info(logger, "La lista de claves contiene esta clave");
				switch(estadoEsi(PREGUNTA_ESI_TIENE_CLAVE,socketPlani,instruccion->clave)){
						case ESI_TIENE_CLAVE:
							log_info(logger,"ESI TIENE CLAVE");
							instanciaConLaClave = instanciaQueTieneLaClave(instruccion->clave);
							log_info(logger, "el Socket de la instancia que tiene la clave es: %d",instanciaConLaClave->instancia->socket);
							if (enviarInstruccion(logger, instruccion,instanciaConLaClave->instancia->socket)==-1){
								instanciaCaida(instanciaConLaClave->instancia->socket, sock);
							}
							else{
								log_info(logger, "envie STORE A INSTANCIA");
								PROTOCOLO_INSTANCIA_A_COORDINADOR rtaInstancia;
								recibirMensaje(logger,sizeof(rtaInstancia),&rtaInstancia, instanciaConLaClave->instancia->socket);
								switch (rtaInstancia){
								case SE_CREO_EL_ARCHIVO:
									enviarRespuestaAlEsi(TODO_OK_ESI, sock,logger);
									break;
								case NO_SE_CREO_EL_ARCHIVO:
									log_info(logger, "NO_SE_CREO_EL_ARCHIVO");
									break;
								default:
									log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
									break;
								}

							int32_t entradasEnUsoDeLaInstancia;
							recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaConLaClave->instancia->socket);
							registrarEntradasOcupadasDeLaInstancia(entradasEnUsoDeLaInstancia,instanciaConLaClave->instancia);
							log_info(logControlDeDistribucion, "las entradas ocupadas por instancia % d son %d", instanciaConLaClave->instancia->socket, instanciaConLaClave->instancia->cantEntradasOcupadas);

							}
							break;
						case ESI_NO_TIENE_CLAVE:
							enviarRespuestaAlEsi(ERROR_CLAVE_NO_BLOQUEADA, sock, logger);
							break;
						default:
							log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
							break;
						}
					}
				else{
					enviarRespuestaAlEsi(ERROR_CLAVE_NO_IDENTIFICADA, sock, logger);
				}
	break;

	}
}

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

void instanciaCaida(int socketInstancia, int sock){
	log_info(logger, "La instancia ya no está");
	eliminarInstanciaYClaveDeLaListaDeInstancias(socketInstancia);
	enviarRespuestaAlEsi(ERROR_CLAVE_INACCESIBLE, sock, logger);
}

bool enviarSETaInstancia(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible){

	if (enviarInstruccion(logger, instruccion, instanciaALlamar->socket)==-1)
	{
		if (avisarClaveInaccesible){
			instanciaCaida(instanciaALlamar->socket, sock);
		}

		modificarInstanciaListaDeClavesConInstancia(instruccion->clave,instanciaALlamar);

		return false;
	}

	else{

	PROTOCOLO_INSTANCIA_A_COORDINADOR rtaInstancia;
	recibirMensaje(logger,sizeof(rtaInstancia),&rtaInstancia, instanciaALlamar->socket);

	if (rtaInstancia == SE_PUDO_GUARDAR_VALOR) {

		log_info(logger, "instancia guardo valor");
		modificarInstanciaListaDeClavesConInstancia(instruccion->clave,instanciaALlamar);
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

void eliminarInstanciaYClaveDeLaListaDeInstancias(int socket){
	bool equals(instancia* item) {

			if (item->socket == socket)
					return true;
			else
					return false;
		}

	bool equals2(claveConInstancia* item) {

			if (item->instancia->socket == socket)
					return true;
			else
					return false;
		}

	list_remove_and_destroy_by_condition(listaDeClavesConInstancia,(void *) equals2, (void *)destruirClaveConInstancia );
	printf("la nueva lista de claves es: \n");
	mostrarLista(listaDeClavesConInstancia);

	list_remove_and_destroy_by_condition(listaDeInstancias,(void *) equals, (void *)destruirInstancia );
	printf("la nueva lista de instancias es: \n");
	mostrarListaIntancias(listaDeInstancias);

	}


void destruirInstancia(instancia* instancia){
	free(instancia);
}

void destruirClaveConInstancia(claveConInstancia* claveConInstancia){
	free(claveConInstancia->clave);
	free(claveConInstancia);
}

claveConInstancia* nuevaClaveConInstancia(char* clave){
	claveConInstancia* nueva=malloc(sizeof(claveConInstancia));
	nueva-> clave = string_new();
	string_append(&(nueva->clave), clave);
	nueva->instancia = NULL;
	return nueva;
}



instancia*  elegirInstanciaSegunAlgoritmo(char * clave){
	letrasDeLaInstancia = list_create();

	if (strcmp(coordConfig->algoritmo, "EL") == 0){
		log_info(logger, "ALGORITMO EQUITATIVE LOAD");
		instancia* instanciaElegida =  EquitativeLoad(listaDeInstancias);
		log_info(logControlDeDistribucion, "el socket de la instancia elegida es %d",instanciaElegida->socket);
		return instanciaElegida;
	}
	else
		{
		if (strcmp(coordConfig->algoritmo, "LSU")==0){
		log_info(logger, "ALGORITMO LSU-----------------------------------");
		mostrarListaIntancias(listaDeInstancias);
		instancia* instanciaElegida =  LSU(listaDeInstancias, logControlDeDistribucion);
		log_info(logControlDeDistribucion, "el socket de la instancia elegida es %d",instanciaElegida->socket);
		return instanciaElegida;
		}
		else {
			if (strcmp(coordConfig->algoritmo, "KE")==0){
				log_info(logger, "ALGORITMO KE-----------------------------------");
				mostrarListaIntancias(listaDeInstancias);
				instancia* instanciaElegida =  KeyExplicit(listaDeInstancias, logControlDeDistribucion,clave,letrasDeLaInstancia);
				log_info(logControlDeDistribucion, "el socket de la instancia elegida es %d",instanciaElegida->socket);
				return instanciaElegida;
			}
			else{
				log_info(logger,"algoritmo desconocido");
			}
		}
		}
	destruirLetrasDeLaInstancia();
	return NULL; //ESTO NO DEBERIA PASAR

}

void destruirLetrasDeLaInstancia(){
	for (int i=0; i< list_size(letrasDeLaInstancia);i++){
		list_remove(letrasDeLaInstancia,i);
	}
}
void modificarInstanciaListaDeClavesConInstancia(char* clave, instancia* instanciaNueva){
	claveConInstancia* elemento =  instanciaQueTieneLaClave(clave);
	elemento->instancia = instanciaNueva;

}
claveConInstancia* instanciaQueTieneLaClave(char* clave){
	bool condicionDeClave(claveConInstancia* item) {
			int rta = strcmp(clave, item->clave);
			if (rta == 0)
					return true;
			else
					return false;
		}

	return list_find(listaDeClavesConInstancia,(void *) condicionDeClave);
}


void registrarInstancia(int sock){
	instancia * registrarInstancia = malloc(sizeof(instancia ));
	registrarInstancia->socket=sock;
	printf("\n socket de esta instancia: %d", sock);
	registrarInstancia->cantEntradasTotales = coordConfig->entradas;
	registrarInstancia->tamanioEntradas= coordConfig->tamanioEntradas;
	registrarInstancia->cantEntradasOcupadas=0;
	list_add(listaDeInstancias,registrarInstancia);

	printf("tamaño listaDeInst: %d \n:", list_size(listaDeInstancias));

	if (list_size(listaDeInstancias)==1)
		nodoDeInstancias = listaDeInstancias->head;

	log_info(logger,"\n Se registro instancia");

	printf("instancias registradas: %d \n", list_size(listaDeInstancias));


}

void mandarConfiguracionAInstancia(int sock){

	PROTOCOLO_COORDINADOR_A_INSTANCIA entradas = ENTRADAS;
	enviarMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&entradas,sock);
	enviarMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&(coordConfig->entradas),sock);

	log_info(logger, "Envie cantidad de entradas a la instancia");
	enviarMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&(coordConfig->tamanioEntradas),sock);
	log_info(logger, "Envie tamanaño de entradas a la instancia");

}


t_instruccion* recibirInstruccionDelEsi(int sock){
	t_instruccion* instruccionAGuardar=recibirInstruccion(logger,sock,"ESI");

	if (instruccionAGuardar != NULL) {
		switch(instruccionAGuardar->instruccion){
					case INSTRUCCION_GET:
						registrarLogDeOperaciones("GET", instruccionAGuardar->clave,"0", logger, logDeOperaciones);

						break;
					case INSTRUCCION_SET:
						registrarLogDeOperaciones("SET", instruccionAGuardar->clave,instruccionAGuardar->valor,logger, logDeOperaciones);
						break;

					case INSTRUCCION_STORE:
						registrarLogDeOperaciones("STORE", instruccionAGuardar->clave,"0",logger, logDeOperaciones);
						break;

	}
	}

	return instruccionAGuardar;
}


void retardo(){
	usleep(1000 * coordConfig->retardo);
}
