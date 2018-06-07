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

		//printf("instrucción guardada, clave: %s, valor: %s", instruccionAGuardar.clave, instruccionAGuardar.valor);
		procesarInstruccion(instruccionAGuardar,sock);
		destruirInstruccion(instruccionAGuardar);
		break;

	}

	//close(sock)
	printf("\n termino el hilo\n ");
	return NULL;


}

void procesarInstruccion(t_instruccion * instruccion, int sock){

	switch(instruccion->instruccion){
			case INSTRUCCION_GET:
				printf ("llegó get \n");
				if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
					printf ("contiene este get \n");
					printf ("\n mostrar lista\n ");
										mostrarLista(listaDeClavesConInstancia);
					/*
					if (preguntar al coordi el estado de esta clave !== BLOQUEADA){
							PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rta = ERROR_CLAVE_NO_BLOQUEADA;
							enviarMensaje(logger, sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rta , sock);
					}
					*/

				}

				else{
					printf("no contiene este get");
					claveConInstancia* clavenueva =  nuevaClaveConInstancia(instruccion->clave, nuevaInstanciaNula());
					list_add(listaDeClavesConInstancia, clavenueva);
					printf("\n se agrego %s", clavenueva->clave);
					printf ("\n mostrar lista\n ");
					mostrarLista(listaDeClavesConInstancia);
					/*
					 * aviso al plani que bloquee esta clave
					 devuelvo al esi todo ok
					 *
					 */
				}

			break;
					case INSTRUCCION_SET:
						printf ("llegó set \n");
						if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
							printf ("contiene esta clave \n");
							instancia* instanciaALlamar = elegirInstanciaSegunAlgoritmo();
							enviarInstruccion(logger, instruccion,instanciaALlamar->socket);

							/*
							 * if (preguntar al plani el estado de esta clave == BLOQUEADA){
								PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rta = ERROR_CLAVE_NO_BLOQUEADA;
								enviarMensaje(logger, sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rta , sock);
								}
								else
								elegirInstanciaMedianteAlgoritmo(instruccion.clave)

								PRIMERO MANDAR LA LONGITUD DE LA CLAVE
								DESPUES LA CLAVE
								DESPUES LA LONGITUD DEL VALOR
								DESPUÉS EL VALOR

								esperar respuesta

							 */
						}

						else{
							printf("no contiene esta clave");
							/*PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rta = ERROR_CLAVE_NO_IDENTIFICADA;
							  enviarMensaje(logger, sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rta , sock);
							 */
						}

						break;

					case INSTRUCCION_STORE:
						printf ("llegó store \n");
						if (contieneClave(listaDeClavesConInstancia,instruccion->clave)){
						printf ("contiene esta clave \n");
						/*
						 * if (preguntar al coordi el estado de esta clave == BLOQUEADA){
							PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rta = ERROR_CLAVE_NO_BLOQUEADA;
							enviarMensaje(logger, sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rta , sock);
							}
							else{

							decirle a la instancia que la archive - MANDAR STORE

							}
						 */
						}

						else{
						printf("no contiene esta clave");
						/*PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rta = ERROR_CLAVE_NO_IDENTIFICADA;
						 enviarMensaje(logger, sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI), &rta , sock);
						 */
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

claveConInstancia* nuevaClaveConInstancia(char* clave, instancia _instancia){
	claveConInstancia* nueva=malloc(sizeof(claveConInstancia));
	nueva-> clave = string_new();
	string_append(&(nueva->clave), clave);

	nueva->instancia = _instancia;
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

/*
void recibirInstruccion(int sock, instruccion * instruccionAGuardar){
	char operacion[80];
	PROTOCOLO_INSTRUCCIONES instruccion;
	char * clave=  string_new();
	char * valor = string_new();
	recibirMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion,sock);
	clave = recibirContenido(logger, sock);

			switch(instruccion){
				case INSTRUCCION_GET:
					registrarLogDeOperaciones(operacion,"GET", clave,"0");
					//guardo instruccion
					instruccionAGuardar->instruccion= INSTRUCCION_GET;
					instruccionAGuardar->clave= string_new();
					instruccionAGuardar->clave=clave;
					instruccionAGuardar->valor="0";
					break;
				case INSTRUCCION_SET:
					valor = recibirContenido(logger, sock);
					registrarLogDeOperaciones(operacion,"SET", clave,valor);

					//guardo instruccion
					instruccionAGuardar->instruccion= INSTRUCCION_SET;
					instruccionAGuardar->clave= string_new();
					instruccionAGuardar->clave=clave;
					instruccionAGuardar->valor=string_new();
					instruccionAGuardar->valor=valor;
					break;

				case INSTRUCCION_STORE:
					registrarLogDeOperaciones(operacion, "STORE", clave,"0");

					//guardo instruccion
					instruccionAGuardar->instruccion= INSTRUCCION_STORE;
					instruccionAGuardar->clave= string_new();
					instruccionAGuardar->clave=clave;
					instruccionAGuardar->valor="0";
				break;
	free(clave);
	free(valor);
}
}
*/

void registrarLogDeOperaciones(char* operacion, char* instruccion, char * clave, char * valor ){

	if (!(strcmp(valor,"0")==0))
		sprintf(operacion, "ESI % d SET %s %s", cantEsi, clave, valor);
	else
		sprintf(operacion, "ESI % d %s %s", cantEsi,instruccion,clave);

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


	return instruccionAGuardar;
}

