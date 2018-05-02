#include "coordinador.h"

int main(int argc, char **argv) {

	t_config *config = malloc(sizeof(t_config));

	/*char *puerto = malloc(sizeof(char) * 6);
	int entradas;
	int tamanioEntradas;
	crearConfiguracion(&puerto, &entradas, &tamanioEntradas, &config);
*/



	coordinador_config * coordConfig = init_coordConfig();
	crearConfiguracion2(&coordConfig,&config);
	//int listenningSocket = crearSocketQueEscucha(&puerto, &entradas);
	int listenningSocket = crearSocketQueEscucha(&coordConfig->puerto, &coordConfig->entradas);
	crearServidorMultiHilo(listenningSocket);
	close(listenningSocket);
	//destroy_coordConfig(coordConfig);
	//free(puerto);

	config_destroy(config);
	return 0;
}

void crearConfiguracion(char ** puerto, int * entradas, int * tamanioEntradas,
		t_config ** config) {
	*config = config_create("configuracion.config");
	*puerto = config_get_string_value(*config,"PUERTO_DE_ESCUCHA");
	*entradas = config_get_int_value(*config, "ENTRADAS");
	*tamanioEntradas = config_get_int_value(*config, "TAMANIO_ENTRADAS");
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

	//Send some messages to the client

	if (enviarmensaje("Coordinador: Bienvenido a mi dominio()\n", sock))
		printf("\n envie1");


	if (recibirmensaje(sock)) {
		printf("\n recibi1");
	}
	else
	{
		printf("error al recibir");
	}
	//while ((read_size = recv(sock, client_message, 50, 0)) > 0) {
	//end of string marker
	/*for (int i = 0; i < 5; i++) {
	 printf("Paso1");


	 recibirmensaje(sock);

	 }*/
	int estado = 1;
	while (estado) {
		estado = recibirmensaje(sock);
	};
	close(sock);
	printf("\n termino el hilo");
	return NULL;


}


coordinador_config * init_coordConfig(){

	coordinador_config* coord = (coordinador_config*)malloc(sizeof (coordinador_config));
	coord->puerto=(char*)malloc(sizeof(char) * 6);
	return coord;


}

void destroy_coordConfig(coordinador_config* coord){
	free(coord->puerto);
	free(coord);

}

void crearConfiguracion2(coordinador_config** coord, t_config ** config){

	*config = config_create("configuracion.config");
	(*coord)->puerto = config_get_string_value(*config, "PUERTO_DE_ESCUCHA");
	(*coord)->entradas = config_get_int_value(*config, "ENTRADAS");
	(*coord)->tamanioEntradas = config_get_int_value(*config, "TAMANIO_ENTRADAS");

}
