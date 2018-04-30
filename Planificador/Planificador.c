/*
 ============================================================================
 Name        : Planificador.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "Planificador.h"
#define PACKAGESIZE 1024
int main(void) {

	int entradas = 20;
	char * puertoEscucha = malloc(sizeof(char) * 20);
	char * ipcoordinador = malloc(sizeof(char) * 20);
	char * puertocoordinador = malloc(sizeof(char) * 20);
	t_config *config;
	crearConfiguracion(&puertoEscucha, &ipcoordinador, &puertocoordinador,
			&config);

	//conexion coordinador
	int socketCordi = conectarseAlServidor(&ipcoordinador, &puertocoordinador);

	pthread_t hiloCoordinador;

	pthread_create(&hiloCoordinador, NULL, comunicacionCoordinador,
			(void*) &socketCordi);

// conexion Esi´s
	int socketEscucha = crearServidor(&puertoEscucha, &entradas);
	pthread_t thread_id;
	
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketPersonal;


	while ((socketPersonal = accept(socketEscucha, (struct sockaddr *) &addr,
			&addrlen))) {
		puts(
				"Esi conectado. aca no recibimos mas mensajes que los hardcodeados:\n");

		if (pthread_create(&thread_id, NULL, manejaconexionconESI,
				(void*) &socketPersonal) < 0) {
			perror("No se pudo crear el hilo");
			exit(1);
		}

		if (socketPersonal < 0) {
			perror("falló la aceptación");
			exit(1);
		}

		puts("Planificador asignado(Al ESI)");




	}
	//Hay que cambiar el coordinador sus entradas son de la configuracion a las instancias
	/* prints !!!Hello World!!! */
	close(socketCordi);
	close(socketEscucha);
	free(puertoEscucha);
	free(ipcoordinador);
	free(puertocoordinador);
		config_destroy(config);
	return EXIT_SUCCESS;
	
}

void *comunicacionCoordinador(void *sock) {
	int socketCordinador = *(int*) sock;
	if (recibirmensaje(socketCordinador)) {
		printf("Recibi mensaje del lord Coordinador");
	}

	if (enviarmensaje("Soy el planificador :)", socketCordinador)) {
		printf("Envie mensaje a lord Coordinador");
	}

		char message[PACKAGESIZE];
		int flag = 1;
		while (flag) {
			fgets(message, PACKAGESIZE, stdin);
			if (!strcmp(message, "exit\n"))
				flag = 0;
			if (flag)
				enviarmensaje(message, socketCordinador);
		}
		return NULL;
}

void crearConfiguracion(char ** puertoescucha, char ** ipcordi,
		char ** puertocordi, t_config ** config) {
	*config = config_create("configPlanificador");
	*ipcordi = config_get_string_value(*config, "IP_COORDINADOR");
	*puertocordi = config_get_string_value(*config, "PUERTO_COORDINADOR");
	*puertoescucha = config_get_string_value(*config, "PUERTO_DE_ESCUCHA");

}

int conectarseAlServidor(char ** ip, char ** puerto) {
	struct addrinfo hints;
	struct addrinfo *serverInfo = malloc(sizeof(struct addrinfo));

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(*ip, *puerto, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion, 	Ya se quien y a donde me tengo que conectar.

	int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol); //Obtiene un socket (un file descriptor -todo en linux es un archivo-), utilizando la estructura serverInfo que generamos antes.

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	return serverSocket;

}

void *manejaconexionconESI(void * socket_desc) {
	//Get the socket descriptor

	int sock = *(int*) socket_desc;

	//Send some messages to the client

	if (enviarmensaje("Yo soy el planificador \n", sock))
		printf("Envie mensaje al esi\n");

	if (enviarmensaje("Tu peor pesadilla \n",
			sock))
		printf("envie mensaje al Esi");

	if (recibirmensaje(sock)) {
		printf("recibi mensaje del ESI\n");
	} else {
		printf("erroralrecibir");
	}
	
	printf("termino el hilo");
	//while ((read_size = recv(sock, client_message, 50, 0)) > 0) {
	//end of string marker
	/*for (int i = 0; i < 5; i++) {
	 printf("Paso1");


	 recibirmensaje(sock);

	 }*/

	close(sock);
	return NULL;
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
