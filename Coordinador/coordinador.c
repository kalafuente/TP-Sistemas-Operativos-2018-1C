/*
 * main.c
 *
 *  Created on: 19 abr. 2018
 *      Author: utnso
 *
 */

#include "coordinador.h"

int main() {

	t_config *config = malloc(sizeof(t_config));
	char *puerto = malloc(sizeof(char) * 6);
	int entradas;
	int tamanioEntradas;
	crearConfiguracion(&puerto, &entradas, &tamanioEntradas, &config);
	int listenningSocket = crearServidor(&puerto, &entradas);
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

int crearServidor(char ** puerto, int * entradas) {
	struct addrinfo hints;
	struct addrinfo *serverInfo;
	printf("Puerto: %s\n", *puerto);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, *puerto, &hints, &serverInfo);// Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);

	bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	printf("Esperando conexiones entrantes\n");
	listen(listenningSocket, *entradas);
	return listenningSocket;
}

void crearServidorMultiHilo(int listenningSocket) {
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int socketCliente;
	pthread_t thread_id;

	while ((socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen))) {
		puts("Cliente conectado. Esperando mensajes:\n");

		if (pthread_create(&thread_id, NULL, manejadorDeConexiones,
				(void*) &socketCliente) < 0) {
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

	if (enviarmensaje("Greetings! I am your connection handler\n", sock))
		printf("envie1\n");

	if (enviarmensaje("Now type something and i shall repeat what you type \n",
			sock))
		printf("envie2");

	if (recibirmensaje(sock)) {
		printf("recibi1\n");
	}
	else
	{
		printf("erroralrecibir");
	}
	printf("termino el hilo");
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

int enviarmensaje(char*mensaje, int unsocket) {
	ContentHeader * cabeza = malloc(sizeof(ContentHeader));
	//char *message = malloc(1024);

	cabeza->len = strlen(mensaje);

	char *message = calloc(cabeza->len, sizeof(char));
	strcpy(message, mensaje);

	if (send(unsocket, cabeza, sizeof(ContentHeader), 0) <= 0) {
		return 0;
	}

	if (send(unsocket, message, strlen(message), 0) <= 0) {
		return 0;
	}

	free(cabeza);
	free(message);
	return 1;
}

int recibirmensaje(int unsocket) {
	ContentHeader * cabeza = malloc(sizeof(ContentHeader));

	if (recv(unsocket, cabeza, sizeof(ContentHeader), MSG_WAITALL) <= 0) {
		return 0;

	}
	char *message = calloc(cabeza->len, sizeof(char));

	if (recv(unsocket, message, cabeza->len, MSG_WAITALL) <= 0) {
		return 0;

	}
	printf("\n %s ", message);

	free(cabeza);
	free(message);
	return 1;

}
