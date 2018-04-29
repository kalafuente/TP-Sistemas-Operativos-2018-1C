#include "coordinador.h"

int main() {

	t_config *config = malloc(sizeof(t_config));
	char *puerto = malloc(sizeof(char) * 6);
	int entradas;
	int tamanioEntradas;
	crearConfiguracion(&puerto, &entradas, &tamanioEntradas, &config);
	int listenningSocket = crearSocketQueEscucha(&puerto, &entradas);
	crearServidorMultiHilo(listenningSocket);
	close(listenningSocket);
	free(puerto);
	config_destroy(config);
	return 0;
}

void crearConfiguracion(char ** puerto, int * entradas, int * tamanioEntradas,
		t_config ** config) {
	*config = config_create("configuracion.config");
	*puerto = config_get_string_value(*config, "PUERTO_DE_ESCUCHA");
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
	//Get the socket descriptor

	int sock = *(int*) socket_desc;

	//Send some messages to the client

	if (enviarmensaje("Greetings! I am your connection handler, decime algo que te lo repito\n", sock))
		printf("envie1\n");


	if (recibirmensaje(sock)) {
		printf("recibi1\n");
	}
	else
	{
		printf("erroralrecibir");
	}

	//while ((read_size = recv(sock, client_message, 50, 0)) > 0) {
	//end of string marker
	/*for (int i = 0; i < 5; i++) {
	 printf("Paso1");


	 recibirmensaje(sock);

	 }*/

	while (1) {
		recibirmensaje(sock);
	};


}

