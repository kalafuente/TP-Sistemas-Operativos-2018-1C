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
	int socketEscucha = crearSocketQueEscucha(&puertoEscucha, &entradas);
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

	if (enviarmensaje("Soy el planificador y me he conectado contido mi lord coordinador \n", socketCordinador)) {
		printf("Yo, plani, envie mensaje a lord Coordinador \n");
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



void *manejaconexionconESI(void * socket_desc) {
	//Get the socket descriptor

	int sock = *(int*) socket_desc;

	//Send some messages to the client

	if (enviarmensaje("Yo soy el planificador \n", sock))
		printf("Envie mensaje al esi\n");



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





