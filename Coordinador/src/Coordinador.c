#include "coordinador.h"

int main(int argc, char **argv) {
	logger= crearLogger("loggerCoordi.log","loggerCoordi");
	logDeOperaciones = crearLogger("logDeOperaciones.log", "logDeOperaciones");

	//----------ARCHIVO DE CONFIGURACION

	t_config *config = config_create("configuracion.config");
	coordConfig = init_coordConfig();
	crearConfiguracion(coordConfig,config);

	//---------CREACION DE ESTRUCTURAS NECESARIAS

	listaDeInstancias= list_create();
	listaDeClavesConInstancia= list_create();

/*
 * instancia nula;
	nula.cantEntradas=0;
	nula.socket=0;
	nula.tamanioEntradas=0;
	nula.tamanioOcupado=0;

	claveConInstancia nueva;
	nueva.clave = "deportes:futbol:messa";
	nueva.instancia= nula;


	list_add(listaDeClavesConInstancia,&nueva);

 */





	cantEsi=0;

	//---------CREO MI SERVIDOR

	int listenningSocket = crearSocketQueEscucha(&coordConfig->puerto, &coordConfig->entradas);
	crearServidorMultiHilo(listenningSocket);



	//---------CIERRO TODO
	close(listenningSocket);
	destroy_coordConfig(coordConfig);
	config_destroy(config);
	return 0;
}

void crearServidorMultiHilo(int listenningSocket) {
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
	//instruccion instruccionAGuardar;
	t_instruccion* instruccionAGuardar;
	//----------LA EDUCACIÓN ANTE TODO, VAMOS A SALUDAR A TODO AQUEL QUE SE CONECTE A MÍ----------
	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi = HANDSHAKE_CONECTAR_COORDINADOR_A_CLIENTES;
	enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES), &handshakeCoordi , sock); //Saludamos

	//----------VAMOS A PREPARARME PARA RECIBIR SALUDOS DE LOS DEMÁS----------
	PROTOCOLO_HANDSHAKE_CLIENTE handshake;

	//----------RECIBO SALUDO DE LOS DEMÁS----------
	recibirMensaje(logger,sizeof(PROTOCOLO_HANDSHAKE_CLIENTE),&handshake,sock);

	//----------IDENTIFICO A QUIEN SE ME CONECTA
	switch(handshake){


	case HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR:
		log_info(logger, "Se me conectó una Instancia");
		mandarConfiguracionAInstancia(sock);
		registrarInstancia(sock);
		mostrarListaIntancias();
		break;


	case HANDSHAKE_CONECTAR_ESI_A_COORDINADOR:
		log_info(logger, "Se me conectó un Esi");
		cantEsi++;
		//recibirInstruccion(sock, &instruccionAGuardar);
		instruccionAGuardar=recibirInstruccionDelEsi(sock);
		while (instruccionAGuardar != NULL) {
		//printf("instrucción guardada, clave: %s, valor: %s", instruccionAGuardar.clave, instruccionAGuardar.valor);
		procesarInstruccion(instruccionAGuardar,sock);
		destruirInstruccion(instruccionAGuardar);
			instruccionAGuardar = recibirInstruccionDelEsi(sock);
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
	PROTOCOLO_COORDINADOR_A_PLANIFICADOR claveDisponible = PREGUNTA_CLAVE_DISPONIBLE;
	PROTOCOLO_COORDINADOR_A_PLANIFICADOR claveBloqueada = PREGUNTA_ESI_TIENE_CLAVE;
	PROTOCOLO_PLANIFICADOR_A_COORDINADOR rtaPlani;
	claveConInstancia* instanciaConLaClave;

	PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rtaParaElEsi;

	switch(instruccion->instruccion){
	case INSTRUCCION_GET:
				log_info(logger, "ESI ENVIO UN GET");
				if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
					log_info(logger, "La lista de claves contiene este GET");
					printf ("\n mostrar lista\n ");
					mostrarLista(listaDeClavesConInstancia);

					//Pregunto si esta disponible
					enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR), &claveDisponible,socketPlani);
					enviarClave(logger, instruccion->clave, socketPlani);
					recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),&rtaPlani, socketPlani);

					switch(rtaPlani){
					case CLAVE_DISPONIBLE:
						log_info(logger,"CLAVE DISPONIBLE");
						rtaParaElEsi= TODO_OK_ESI;
						enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
						log_info(logger, "Le dije al esi que todo ok");
						break;
					case CLAVE_NO_DISPONIBLE:
						log_info(logger,"CLAVE NO DISPONIBLE");
						rtaParaElEsi = BLOQUEATE;
						enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
						log_info(logger, "Le dije al esi que se bloquee");
						break;
					default:
						log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
						break;
					}

				}

				else{
					char clavePaElLog[80];
					log_info(logger, "La lista de claves NO contiene este GET");
					claveConInstancia* clavenueva =  nuevaClaveConInstancia(instruccion->clave);
					list_add(listaDeClavesConInstancia, clavenueva);

					sprintf(clavePaElLog, "Se agrego esta clave: %s", clavenueva->clave);
					log_info(logger, clavePaElLog);

					printf ("\n mostrar lista\n ");
					mostrarLista(listaDeClavesConInstancia);
					rtaParaElEsi= TODO_OK_ESI;
					enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
					log_info(logger, "Le dije al esi que todo ok");
				}

	break;
	case INSTRUCCION_SET:
				log_info(logger, "ESI ENVIO UN SET");
				if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
					log_info(logger, "La lista de claves contiene este GET");

					enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR), &claveBloqueada,socketPlani);
					enviarClave(logger, instruccion->clave, socketPlani);
					recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),&rtaPlani, socketPlani);

					switch(rtaPlani){
					case ESI_TIENE_CLAVE:
						log_info(logger,"ESI TIENE CLAVE");
						instancia* instanciaALlamar = elegirInstanciaSegunAlgoritmo();
						enviarInstruccion(logger, instruccion,instanciaALlamar->socket);
						//DEBERIA MANDARME LA INSTANCIA ALGO PARA SABER QUE ESTÁ TODO OK
						modificarInstanciaListaDeClavesConInstancia(instruccion->clave,instanciaALlamar);
						rtaParaElEsi= TODO_OK_ESI;
						enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
						log_info(logger, "Le dije al esi que todo ok");
						break;
					case ESI_NO_TIENE_CLAVE:
						log_info(logger,"ESI NO TIENE CLAVE");
						rtaParaElEsi= ERROR_CLAVE_NO_BLOQUEADA;
						enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
						break;
					default:
						log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
						break;
					}
				}
				else{
					log_info(logger, "La lista de claves no contiene este GET");
					rtaParaElEsi= ERROR_CLAVE_NO_IDENTIFICADA;
					enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
				}
	break;

	case INSTRUCCION_STORE:

				log_info(logger, "ESI ENVIO UN STORE");
					if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
						log_info(logger, "La lista de claves contiene esta clave");
					enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR), &claveBloqueada,socketPlani);
					enviarClave(logger, instruccion->clave, socketPlani);

					recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),&rtaPlani, socketPlani);
						switch(rtaPlani){
						case ESI_TIENE_CLAVE:
							log_info(logger,"ESI TIENE CLAVE");
							log_info(logger,"busco la instancia que tiene esta clave");
							instanciaConLaClave = instanciaQueTieneLaClave(instruccion->clave);
							log_info(logger, "el Socket de la instancia que tiene la clave es: %d",instanciaConLaClave->instancia->socket);

							enviarInstruccion(logger, instruccion,instanciaConLaClave->instancia->socket);
							log_info(logger, "envie STORE A INSTANCIA");

							rtaParaElEsi= TODO_OK_ESI;
							enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
							log_info(logger, "Le dije al esi que todo ok");
							break;
						case ESI_NO_TIENE_CLAVE:
							log_info(logger,"ESI NO TIENE CLAVE");
							rtaParaElEsi= ERROR_CLAVE_NO_BLOQUEADA;
							enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
							break;
						default:
							log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
							break;
						}
					}
					else{
						log_info(logger, "La lista de claves no contiene este GET");
						rtaParaElEsi= ERROR_CLAVE_NO_IDENTIFICADA;
						enviarMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rtaParaElEsi, sock);
					}
	break;
	}
}


void mostrarLista(t_list* lista){
	void mostrar(claveConInstancia * elem){
		printf ("clave, %s \n", elem->clave);
	}
	list_iterate(lista, (void *) mostrar);
}

void mostrarListaIntancias(){
	void mostrar(instancia * elem){
		printf ("sock instancia, %d \n", elem->socket);
	}
	list_iterate(listaDeInstancias, (void *) mostrar);
}

claveConInstancia* nuevaClaveConInstancia(char* clave){
	claveConInstancia* nueva=malloc(sizeof(claveConInstancia));
	nueva-> clave = string_new();
	string_append(&(nueva->clave), clave);
	nueva->instancia = NULL;
	return nueva;
}
instancia nuevaInstanciaNula(){
	instancia nula;
	nula.cantEntradas=0;
	nula.socket=0;
	nula.tamanioEntradas=0;
	nula.tamanioOcupado=0;
	return nula;
}

bool contieneString(t_list* list, void* value){

	bool equals(void* item) {
		int rta = strcmp(value, item);
		if (rta == 0)
				return true;
		else
				return false;
	}

	return list_any_satisfy(list, equals);
}

instancia*  elegirInstanciaSegunAlgoritmo(){
	int comparacion = strcmp(coordConfig->algoritmo, "EL");
	if (comparacion == 0){
		printf("\n el algoritmo es EL");
		instancia* instanciaElegida =  EquitativeLoad();
		printf ("\n el socket de la instancia elegida es %d:", instanciaElegida->socket);
		return instanciaElegida;
	}
	else
		printf("no es el");
		return NULL; //ESTO NO DEBERIA PASAR

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

instancia * EquitativeLoad() {


	instancia * aux = list_remove(listaDeInstancias,0);
	list_add(listaDeInstancias, aux);
	return aux;

}



bool contieneClave(t_list* list, void* value){

	bool equals(claveConInstancia* item) {
		int rta = strcmp(value, item->clave);
		if (rta == 0)
				return true;
		else
				return false;
	}

	return list_any_satisfy(list, (void *) equals);
}


void registrarLogDeOperaciones(char* operacion, char* instruccion, char * clave, char * valor ){

	PROTOCOLO_COORDINADOR_A_PLANIFICADOR pedidoID = PEDIDO_DE_ID;
	int rtaPlani;
	enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR), &pedidoID,socketPlani);
	recibirMensaje(logger, sizeof(int),&rtaPlani, socketPlani);
	log_info(logger,"el ID del esi segun plani fue recibido");
	log_info(logger,"el id es: %d",rtaPlani);
	if (!(strcmp(valor,"0")==0))
		sprintf(operacion, "ESI % d SET %s %s", rtaPlani, clave, valor);
	else
		sprintf(operacion, "ESI % d %s %s", rtaPlani,instruccion,clave);

	log_info(logDeOperaciones, operacion);
}
void registrarInstancia(int sock){
	instancia * registrarInstancia = malloc(sizeof(instancia ));
	registrarInstancia->socket=sock;
	printf("\n socket de esta instancia: %d", sock);
	registrarInstancia->cantEntradas = coordConfig->entradas;
	registrarInstancia->tamanioEntradas= coordConfig->tamanioEntradas;
	registrarInstancia->tamanioOcupado=0;
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

//*************************************FUNCIONES PARA EL ARCHIVO DE CONFIGURACION*************************************
coordinador_config * init_coordConfig(){
	coordinador_config* coordinadorConfig = malloc(sizeof (coordinador_config));
	coordinadorConfig->puerto=string_new();
	coordinadorConfig->algoritmo=string_new();
	coordinadorConfig->entradas=0;
	coordinadorConfig->tamanioEntradas=0;
	return coordinadorConfig;
}
void crearConfiguracion(coordinador_config* coordinador, t_config* config){
	string_append(&(coordinador->puerto), config_get_string_value(config, "PUERTO_DE_ESCUCHA"));
	string_append(&(coordinador->algoritmo), config_get_string_value(config, "ALGORITMO_DISTRIBUCIÓN"));
	coordinador->entradas = config_get_int_value(config, "ENTRADAS");
	coordinador->tamanioEntradas = config_get_int_value(config, "TAMANIO_ENTRADAS");
}
void destroy_coordConfig(coordinador_config* coordinadorConfig){
	free(coordinadorConfig->puerto);
	free(coordinadorConfig->algoritmo);
	free(coordinadorConfig);
}



t_instruccion* recibirInstruccionDelEsi(int sock){
	t_instruccion* instruccionAGuardar=recibirInstruccion(logger,sock);
	char operacion[80];
	if (instruccionAGuardar != NULL) {
		switch(instruccionAGuardar->instruccion){
					case INSTRUCCION_GET:
						registrarLogDeOperaciones(operacion,"GET", instruccionAGuardar->clave,"0");

						break;
					case INSTRUCCION_SET:
						registrarLogDeOperaciones(operacion,"SET", instruccionAGuardar->clave,instruccionAGuardar->valor);
						break;

					case INSTRUCCION_STORE:
						registrarLogDeOperaciones(operacion,"STORE", instruccionAGuardar->clave,"0");
					break;
	}
	}

	return instruccionAGuardar;
}

