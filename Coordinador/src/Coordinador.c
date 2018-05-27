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
	cantEsi=0;

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


	//----------LA EDUCACIÓN ANTE TODO, VAMOS A SALUDAR A TODO AQUEL QUE SE CONECTE A MÍ----------
	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi = HANDSHAKE_CONECTAR_COORDINADOR_A_CLIENTES;
	enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES), &handshakeCoordi , sock); //Saludamos

	//----------VAMOS A PREPARARME PARA RECIBIR SALUDOS DE LOS DEMÁS----------
	PROTOCOLO_HANDSHAKE_CLIENTE handshake;

	//----------RECIBO SALUDO DE LOS DEMÁS----------
	recibirMensaje(logger,sizeof(PROTOCOLO_HANDSHAKE_CLIENTE),&handshake,sock);

	//----------IDENTIFICO A QUIEN SE ME CONECTA
	switch(handshake){

	//----------SI SE ME CONECTA INSTANCIA ENTONCES
	case HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR:
		log_info(logger, "Se me conectó una Instancia");
		mandarConfiguracionAInstancia(sock);
		registrarInstancia(sock);
		break;

	//----------SI SE ME CONECTA ESI ENTONCES
	case HANDSHAKE_CONECTAR_ESI_A_COORDINADOR:
		log_info(logger, "Se me conectó un Esi");
		cantEsi++;

		recibirInstruccion(sock);
		break;

	}

	close(sock);
	printf("\n termino el hilo\n ");
	return NULL;


}

void recibirInstruccion(int sock){
	char operacion[80];
	PROTOCOLO_INSTRUCCIONES instruccion;
	char * clave = calloc(1,sizeof(char*));
	char * valor = calloc(1,sizeof(char*));
	recibirMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion,sock);
			clave = recibirContenido(logger, sock);

			switch(instruccion){
				case INSTRUCCION_GET:
								sprintf(operacion, "ESI % d GET %s", cantEsi,clave);
								log_info(logDeOperaciones, operacion);
								break;
				case INSTRUCCION_SET:
								valor = recibirContenido(logger, sock);
								sprintf(operacion, "ESI % d SET %s %s", cantEsi, clave, valor);
								log_info(logDeOperaciones, operacion);
								break;

				case INSTRUCCION_STORE:
								sprintf(operacion, "ESI % d STORE %s", cantEsi,clave);
								log_info(logDeOperaciones, operacion);
								break;
	free(clave);
	free(valor);
}
}

void registrarInstancia(int sock){
	instancia registrarInstancia;
	registrarInstancia.socket=sock;
	registrarInstancia.cantEntradas = coordConfig->entradas;
	registrarInstancia.tamanioEntradas= coordConfig->tamanioEntradas;
	registrarInstancia.tamanioOcupado=0;
	list_add(listaDeInstancias,&registrarInstancia);
	log_info(logger,"Se registro instancia");

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

