#include "coordinador.h"


int main(int argc, char **argv) {

	prepararLoggers();
	prepararConfiguracion(argc,argv);
	crearListas();
	crearServidor();
	killCoordinador();
	return 0;

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

}


void *manejadorDeConexiones(void *socket_desc) {

	int sock = *(int*) socket_desc;
	t_instruccion* instruccionAGuardar;
	saludar(sock);

	switch(recibirSaludo(sock)){

		case HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR:
			log_info(logger, "Se me conectó una Instancia");
			char * id = recibirID(sock, logger);
			mandarConfiguracionAInstancia(sock);
			enviarClavesCorrespondientes(sock,id,listaDeClavesConInstancia);

			if(existeID(id,listaDeInstancias)){
				printf("Se reconecta instancia, socket nuevo: %d \n", sock);
				printf("instancias viejas: \n");
				mostrarListaIntancias(listaDeInstancias);
				actualizarSocketInstancia(sock, id, listaDeInstancias);
				printf("instancias actualizadas: \n");
				mostrarListaIntancias(listaDeInstancias);
			}
			else{
				printf("Nueva instancia");
				registrarInstancia(sock, id);
				mostrarListaIntancias(listaDeInstancias);
			}

			break;

		case HANDSHAKE_CONECTAR_STATUS_A_COORDINADOR:
			log_info(logger, "Se me conectó el planificador para pedir status");
			char * claveAPedir; char * valor;
			printf("esperando clave del status \n");
			claveAPedir = recibirID(sock,logger);
			printf("la clave que me pidio saber el plani es %s", claveAPedir);
			while(strcmp(claveAPedir, "null")!=0){
				if (contieneClave(listaDeClavesConInstancia,claveAPedir)){
					claveConInstancia * instanciaALlamar = instanciaQueTieneLaClave(claveAPedir,listaDeClavesConInstancia);
					if (instanciaALlamar->instancia == NULL){
						enviarID(sock,"no hay valor, pero hay get",logger);
						instancia * instanciaElegida = simulacionElegirInstanciaSegunAlgoritmo(claveAPedir,letrasDeLaInstancia);
						enviarID(sock,instanciaElegida->identificador,logger);
					}
					else{
							t_instruccion * falsa = malloc (sizeof(t_instruccion));
							falsa->instruccion = PEDIDO_DE_VALOR;
							falsa->clave = "null";
							falsa->valor = "null";

							PROTOCOLO_INSTANCIA_A_COORDINADOR rta;
							if (enviarInstruccion(logger,falsa,instanciaALlamar->instancia->socket)==-1){
								enviarID(sock,"no hay valor, se cayó la instancia",logger);
								enviarID(sock,instanciaALlamar->instancia->identificador,logger);
							}
							else {
								if (enviarID(instanciaALlamar->instancia->socket,claveAPedir,logger)==-1){
									enviarID(sock,"no hay valor, se cayó la instancia",logger);
									enviarID(sock,instanciaALlamar->instancia->identificador,logger);
								}
								else{
										valor = recibirID(instanciaALlamar->instancia->socket,logger);
										printf("recibi valor %s \n", valor);
										if (strcmp(valor, "null")!=0){
											enviarID(sock,valor,logger);
											printf("envié valor: %s \n", valor);
										}


										else
										enviarID(sock,"no hay valor",logger);

										enviarID(sock,instanciaALlamar->instancia->identificador,logger);

								}
						}
							recibirMensaje(logger,sizeof(rta),&rta, instanciaALlamar->instancia->socket);
							int32_t entradasEnUsoDeLaInstancia;
							recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->instancia->socket);

							free(falsa);
					}

				}
				else
					enviarID(sock,"ClaveInexistente",logger);



			free(claveAPedir);
			claveAPedir = recibirID(sock,logger);
			free(valor);

			}
			free(valor);
			free(claveAPedir);
			break;

		case HANDSHAKE_CONECTAR_ESI_A_COORDINADOR:
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

			if (esi == TERMINE_INSTRUCCIONES){
				log_info(logger, "ESI TERMINÓ DE MANDAR LAS INSTRUCCIONES, YUPI");
				close(sock);
			}

			if (instruccionAGuardar== NULL){
				log_info(logger, "no me puedo conectar con esi");
				close(sock);
			}


			break;

		case HANDSHAKE_CONECTAR_PLANIFICADOR_A_COORDINADOR:
			log_info(logger, "Se me conectó el planificador");
			socketPlani = sock;
			printf("\n socket: %d", sock);
			printf("\n socketguardado: %d", socketPlani);
			break;

	}

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
				mostrarListaDeClaves(listaDeClavesConInstancia);
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
									mostrarListaDeClaves(listaDeClavesConInstancia);
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
							instancia* instanciaALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);

							bool seEnvio = enviarSETaInstancia(instanciaALlamar,sock, instruccion, false);
							while(!seEnvio){
								instanciaALlamar = elegirInstanciaSegunAlgoritmo(instruccion->clave, logger, logControlDeDistribucion, letrasDeLaInstancia);
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
