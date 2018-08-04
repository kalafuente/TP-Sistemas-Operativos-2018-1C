#include "coordinador.h"

int main(int argc, char **argv) {
	banderaTerminarHilos = 0;

	pthread_mutex_init(&mutexCompactacion, NULL);
	pthread_mutex_init(&mutexlistaInstancias, NULL);
	pthread_mutex_init(&mutexConexionInstancias, NULL);

	sem_init(&terminoCompactacion, 0, 0);

	prepararLoggers();
	prepararConfiguracion(argc,argv);
	crearListas();
	crearServidor();
	killCoordinador();
	return 0;
}

void terminarHilos(){

	void cerrarHilos(pthread_t * elem){
			pthread_cancel(*elem);
		//	printf("cierro hilo %lu \n",*elem);
		}
	list_iterate(hilos, (void *) cerrarHilos);
	list_destroy(hilos);
	log_info(logger, "se cerraron los hilos");
}



void cerrarTodo(){
	close(listenningSocket);
	close(socketPlani);
	destroy_coordConfig(coordConfig);
	config_destroy(config);

}

void prepararLoggers(){
	logger= crearLogger("loggerCoordi.log","loggerCoordi");
	logDeOperaciones = crearLogger("logDeOperaciones.log", "logDeOperaciones");
	logControlDeDistribucion = crearLogger("logControlDeDistribucion.log", "logControlDeDistribucion");
}
void prepararConfiguracion(int argc, char **argv){

	config= abrirArchivoConfig(argc,argv,logger,destruirLoggers);
	//config = config_create("configuracion.config");
	coordConfig = init_coordConfig();
	crearConfiguracion(coordConfig,config);
	mostrarValoresArchConfig(coordConfig);

}

void mostrarValoresArchConfig(coordinador_config* config){
	printf("ALGORITMO DE DISTRIBUCION: %s \n",config->algoritmo);
	printf("ENTRADAS: %d \n",config->entradas);
	printf("TAMANIO ENTRADAS: %d \n",config->tamanioEntradas);
	printf("RETARDO: %d \n",config->retardo);
}


void crearListas(){
	listaDeInstancias= list_create();
	listaDeClavesConInstancia= list_create();
	hilos=list_create();
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

	while (banderaTerminarHilos ==0 && (socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,	&addrlen))>0) {

		puts("Cliente conectado. \n");

		if (pthread_create(&thread_id, NULL, manejadorDeConexiones, (void*) &socketCliente) < 0) {
			log_error(logger,"No se pudo crear el hilo");
			exit(1);
		}

		/*pthread_join(thread_id,NULL); SI ESTA FUNCIÓN FUERA NO BLOQUEANTE
		 * if (banderaTerminarHilos !=0){
				printf("la bandera es: %d \n", banderaTerminarHilos);
				break;
			}
		 */
		puts("Manejador de conexiones asignado \n");
		//printf("la bandera es: %d \n", banderaTerminarHilos);
	}


	if (socketCliente < 0) {

		if (banderaTerminarHilos == 1){
				log_info(logger, "Planificador desconectado, se cierra el sistema");
			}
		else
		log_error(logger,"falló la aceptación");
	}
}


void *manejadorDeConexiones(void *socket_desc) {
	int sock = *(int*) socket_desc;
	pthread_t  idHilo = pthread_self();
	//printf ("hilo: %lu \n", idHilo);
	t_instruccion* instruccionAGuardar;
	saludar(sock);


	switch(recibirSaludo(sock)){

		case HANDSHAKE_CONECTAR_PLANIFICADOR_A_COORDINADOR:
			log_info(logger, "Planificador conectado");
			socketPlani = sock;
			break;

		case HANDSHAKE_CONECTAR_STATUS_A_COORDINADOR:
			list_add(hilos, &idHilo);
			status(sock);
			close(sock);
			break;

		case HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR:
			pthread_mutex_lock(&mutexConexionInstancias);

			log_info(logger, "Se me conectó una Instancia");
			char * id = recibirID(sock, logger);
			pthread_mutex_lock(&mutexCompactacion);
			mandarConfiguracionAInstancia(sock);
			enviarClavesCorrespondientes(sock,id,listaDeClavesConInstancia);
			pthread_mutex_unlock(&mutexCompactacion);

			if(existeID(id,listaDeInstancias)){
				log_info(logger,"Se reconecta instancia, socket nuevo: %d \n", sock);
				//printf("instancias viejas: \n");mostrarListaIntancias(listaDeInstancias);

				pthread_mutex_lock(&mutexlistaInstancias);
				actualizarSocketInstancia(sock, id, listaDeInstancias);
				pthread_mutex_unlock(&mutexlistaInstancias);

				//printf("instancias actualizadas: \n");mostrarListaIntancias(listaDeInstancias);
			}
			else{
				log_info(logger,"Nueva instancia");

				pthread_mutex_lock(&mutexlistaInstancias);
				registrarInstancia(sock, id);
				pthread_mutex_unlock(&mutexlistaInstancias);

				//mostrarListaIntancias(listaDeInstancias);
			}

			pthread_mutex_unlock(&mutexConexionInstancias);

			break;

		case HANDSHAKE_CONECTAR_ESI_A_COORDINADOR:
			list_add(hilos, &idHilo);
			log_info(logger, "Se me conectó un Esi");
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


			if (esi == TERMINE_INSTRUCCIONES)
				log_info(logger, "ESI TERMINÓ DE MANDAR LAS INSTRUCCIONES, YUPI");

			if (instruccionAGuardar== NULL)
				//log_info(logger, "no me puedo conectar con esi");

			close(sock);
			eliminarEsteHilo(idHilo);
			break;
	}


	if (banderaTerminarHilos == 1){
		shutdown(listenningSocket,SHUT_RD);
	}

	pthread_exit(EXIT_SUCCESS);
}

void eliminarEsteHilo(pthread_t hilo){
	bool equal(pthread_t * item) {
			if ( pthread_equal(hilo,*item)){
				//printf("\n elimino el hilo\n %lu", *item);
				return true;
			}

			else{
				//printf("\n no se elimino el hilo\n ");
				return false;
			}

		}
	list_remove_by_condition(hilos, (void *) equal);
}

void procesarInstruccion(t_instruccion * instruccion, int sock){

	log_info(logger,"entro a procesarInstruccion");
	claveConInstancia* instanciaConLaClave;

	switch(instruccion->instruccion){

		case INSTRUCCION_GET:
			log_info(logger, "ESI ENVIO UN GET");
			if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
				log_info(logger, "La lista de claves contiene este GET");
				//mostrarListaDeClaves(listaDeClavesConInstancia);
				switch(estadoEsi(logger,PREGUNTA_CLAVE_DISPONIBLE, socketPlani,instruccion)){
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
				claveConInstancia* clavenueva;
				switch(estadoEsi(logger,PREGUNTA_CLAVE_DISPONIBLE, socketPlani,instruccion)){
								case CLAVE_DISPONIBLE:
									clavenueva =  nuevaClaveConInstancia(instruccion->clave);
									list_add(listaDeClavesConInstancia, clavenueva);
									log_info(logger, "Se agrego esta clave: %s", clavenueva->clave);
									//mostrarListaDeClaves(listaDeClavesConInstancia);
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

			break;

		case INSTRUCCION_SET:
			log_info(logger, "ESI ENVIO UN SET");
			if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
				log_info(logger, "La lista de claves contiene este GET");

				switch(estadoEsi(logger,PREGUNTA_ESI_TIENE_CLAVE, socketPlani,instruccion)){

					case ESI_TIENE_CLAVE:
						log_info(logger,"ESI TIENE CLAVE");
						instancia * instanciaQueTieneSetClave = instanciaQueTieneLaClave(instruccion->clave, listaDeClavesConInstancia)->instancia;
						if (instanciaQueTieneSetClave == NULL){
							log_info(logControlDeDistribucion,"NO HAY SET PREVIO DE ESTA CLAVE");

							//printf("intenta ahora conectar instancia \n");
							pthread_mutex_lock(&mutexConexionInstancias);
							//sleep(5);
							instancia* instanciaALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);

							if (instanciaALlamar == NULL){
								enviarRespuestaAlEsi(ERROR_NO_HAY_INSTANCIAS, sock, logger);
								log_info(logger, "le dije al esi que no hay más instancias");
								break;
							}

							bool seEnvio = enviarSETaInstancia(instanciaALlamar,sock, instruccion, false);


							while(!seEnvio){
								instanciaALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);

								if (instanciaALlamar == NULL){
									enviarRespuestaAlEsi(ERROR_NO_HAY_INSTANCIAS, sock, logger);
									log_info(logger, "le dije al esi que no hay más instancias");
									break;
								}

								seEnvio = enviarSETaInstancia(instanciaALlamar,sock, instruccion, false);
							}

						}
						else {
							log_info(logControlDeDistribucion,"HAY SET PREVIO DE ESTA CLAVE, LA CLAVE ESTÁ EN EL SOCKET: %d",instanciaQueTieneSetClave->socket );
							enviarSETaInstancia(instanciaQueTieneSetClave,sock, instruccion, true);
						}

						pthread_mutex_unlock(&mutexConexionInstancias);


						break;

					case ESI_NO_TIENE_CLAVE:
							enviarRespuestaAlEsi(ERROR_CLAVE_NO_BLOQUEADA, sock, logger);
							break;

					default:
							log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
							break;
				}
			}
			else
				enviarRespuestaAlEsi(ERROR_CLAVE_NO_IDENTIFICADA, sock, logger);

			break;
		case INSTRUCCION_STORE:
			log_info(logger, "ESI ENVIO UN STORE");
			if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
				log_info(logger, "La lista de claves contiene esta clave");
				switch(estadoEsi(logger, PREGUNTA_ESI_TIENE_CLAVE,socketPlani,instruccion)){

					case ESI_TIENE_CLAVE:
						log_info(logger,"ESI TIENE CLAVE");
						instanciaConLaClave = instanciaQueTieneLaClave(instruccion->clave, listaDeClavesConInstancia);
						log_info(logger, "el Socket de la instancia que tiene la clave es: %d",instanciaConLaClave->instancia->socket);
						if (enviarInstruccion(logger, instruccion,instanciaConLaClave->instancia->socket)==-1){
							instanciaCaida(instruccion->clave, sock);
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
									instanciaCaida(instruccion->clave,sock);
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
			else
				enviarRespuestaAlEsi(ERROR_CLAVE_NO_IDENTIFICADA, sock, logger);
			break;
		default:
			log_error(logger, "ERROR MENSAJE NO ANTICIPADO ");
			break;

		}
}


void instanciaCaida(char * clave, int sock){
	log_info(logger, "La instancia ya no está");
	eliminarClave(listaDeClavesConInstancia, clave);
	//eliminarInstancia(socketInstancia, listaDeInstancias); porque podría ser que tenga otra clave y se pueda reconectar
	enviarRespuestaAlEsi(ERROR_CLAVE_INACCESIBLE, sock, logger);
}


void retardo(){
	usleep(1000 * coordConfig->retardo);
}

void killCoordinador(){
	terminarHilos();
	destruirListas();
	destruirLoggers();
	cerrarTodo();
}

void destruirListas(){
	list_destroy_and_destroy_elements(listaDeInstancias, (void *)destruirInstancia);
	list_destroy_and_destroy_elements(listaDeClavesConInstancia, (void *)destruirClaveConInstancia);
}

void destruirLoggers(){
	log_destroy(logger);
	log_destroy(logDeOperaciones);
	log_destroy(logControlDeDistribucion);

}
