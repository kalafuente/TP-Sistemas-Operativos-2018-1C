#include "coordinador.h"

int main(int argc, char **argv) {
	logger= crearLogger("loggerCoordi.log","loggerCoordi");
	logDeOperaciones = crearLogger("logDeOperaciones.log", "logDeOperaciones");
	t_config *config = malloc(sizeof(t_config));
	coordinador_config * coordConfig = init_coordConfig();
	crearConfiguracion(&coordConfig,&config);
	int listenningSocket = crearSocketQueEscucha(&coordConfig->puerto, &coordConfig->entradas);
	crearServidorMultiHilo(listenningSocket);
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
	int id;
	char operacion[80];

	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi = HANDSHAKE_CONECTAR_COORDINADOR_A_CLIENTES;
	enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES), &handshakeCoordi , sock); //Saludamos
	PROTOCOLO_ESI_A_COORDINADOR handshakeESI;
	//PROTOCOLO_INSTANCIA_A_COORDINADOR handshakeInstancia;

	if (recibirMensaje(logger,sizeof(PROTOCOLO_ESI_A_COORDINADOR),&handshakeESI,sock)){

		PROTOCOLO_INSTRUCCIONES instruccion;

		log_info(logger, "Se me conectó un Esi");
		cantEsi++;
		char * clave = calloc(1,sizeof(char*));
		char * valor = calloc(1,sizeof(char*));
		recibirMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion,sock);

		switch(instruccion){
		case INSTRUCCION_GET:
			clave = recibirIDyContenido(&id, logger, sock);
			sprintf(operacion, "ESI % d GET %s", cantEsi,clave);
			log_info(logDeOperaciones, operacion);
			break;
		case INSTRUCCION_SET:

			sprintf(operacion, "ESI % d SET %s %s", cantEsi, clave, valor);
			log_info(logDeOperaciones, operacion);
			break;

		case INSTRUCCION_STORE:
			sprintf(operacion, "ESI % d STORE %s", cantEsi,clave);
			log_info(logDeOperaciones, operacion);
			break;
		}
		free(clave);
		free(valor);

	} //else{
		//recibirMensaje(logger,sizeof(PROTOCOLO_INSTANCIA_A_COORDINADOR),&handshakeInstancia,sock)
	//}


	close(sock);
	printf("\n termino el hilo");
	return NULL;


}



coordinador_config * init_coordConfig(){
	coordinador_config* coord = calloc(1, sizeof (coordinador_config));
	coord->puerto=calloc(4,sizeof(char));
	return coord;


}

void destroy_coordConfig(coordinador_config* coord){
	free(coord->puerto);
	free(coord);

}

void crearConfiguracion(coordinador_config** coord, t_config ** config){

	*config = config_create("configuracion.config");
	(*coord)->puerto = config_get_string_value(*config, "PUERTO_DE_ESCUCHA");
	(*coord)->entradas = config_get_int_value(*config, "ENTRADAS");
	(*coord)->tamanioEntradas = config_get_int_value(*config, "TAMANIO_ENTRADAS");

}
