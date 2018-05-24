#include "coordinador.h"

int main(int argc, char **argv) {
	logger= crearLogger("loggerCoordi.log","loggerCoordi");
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
	enviarMensaje(logger, ID_COORDINADOR, "SoyCoordi", sock); //Saludamos
	recibirIDyContenido(&id, logger, sock);

	switch(id) {

		case ID_ESI  :
		printf("Se me conectó un ESI \n ");
		recibirIDyContenido(&id, logger, sock);
				   //Espero la instrucción proveniendo del ESI
				   //char* instruccion =recibirIDyContenido(logger, sock, &id);
				   //actualizarLogDeOperaciones();
		break;

		case ID_INSTANCIA :
		printf("Se me conectó una Instancia \n ");

		break;

			  // case ID_PLANIFICADOR:
				//pasa plani
				 //  break;

			   //default : /* Optional */
			   //Es alguien desconocido
			   //statement(s);
			}
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
