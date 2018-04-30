
/*
 * esi.c
 *
 *  Created on: 25 abr. 2018
 *      Author: utnso
 */



#include "esi.h"

#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

int main() {
	t_config *config = malloc(sizeof(t_config));

	char *ipCoordi = malloc(sizeof(char) * 5);
	char *puertoCoordi = malloc(sizeof(char) * 20);
	char *idPlanificador = malloc(sizeof(char) * 20);
	char *puertoPlanificador = malloc(sizeof(char) * 5);

	crearConfiguracion(&ipCoordi, &puertoCoordi, &idPlanificador,
			&puertoPlanificador, &config);
	printf("ipCoordi: %s", ipCoordi);

	int coordinadorSocket;
	coordinadorSocket = conectarseAlServidor(&ipCoordi, &puertoCoordi);


	int planificadorSocket = conectarseAlServidor(&idPlanificador,
			&puertoPlanificador);


	pthread_t thread_id;
	pthread_create(&thread_id, NULL, conexionPlanificador,
			(void*) &planificadorSocket);



	printf(
			"Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");

	if (recibirmensaje(coordinadorSocket)) {
			printf("recibimensaje del coordinador");
	};

		if (enviarmensaje("nos conectamos con el cordinador :)",
				coordinadorSocket)) {
		printf("enviemensaje3\n");
	};

	
		// Confirmar que nos conectamo
		/*

		fgets(message, PACKAGESIZE, stdin);	// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.

		
		if (!strcmp(message, "exit\n"))
	
			enviar = 0;			// Chequeo que el usuario no quiera salir
		if (enviar) {

			enviarmensaje(message, serverSocket);
		} // Solo envio si el usuario no quiere salir.
		 */
	char message[50];
		int flag = 1;
	printf("alcanzo esto\n");

	while (flag) {
		printf("entro al while \n");

		fgets(message, 50, stdin);
		if (!strcmp(message, "exit\n")) {
			flag = 0;
		}
		if (flag)
		{
			enviarmensaje(message, coordinadorSocket);
		}
	}


	printf("\n termine\n");

	free(ipCoordi);
	free(puertoCoordi);
	free(idPlanificador);
	free(puertoPlanificador);
	pthread_join(thread_id, NULL);
	close(planificadorSocket);
	close(coordinadorSocket);
	config_destroy(config);
	return 0;

}

void crearConfiguracion(char ** ipCoordi, char ** puertoCoordi,
		char ** idPlanificador, char ** puertoPlanificador, t_config ** config) {

	*config = config_create("configuracion.config");
	*ipCoordi = config_get_string_value(*config, "IP_COORDINADOR");
	*puertoCoordi = config_get_string_value(*config, "PUERTO_COORDINADOR");
	*idPlanificador = config_get_string_value(*config, "IP_PLANIFICADOR");
	*puertoPlanificador = config_get_string_value(*config,
			"PUERTO_PLANIFICADOR");

}

void *conexionPlanificador(void *sock) {
	int socketplanificador = *(int*) sock;
	printf("\n inicio el hilo");

	if (recibirmensaje(socketplanificador)) {
		printf("\nrecibimensaje de sir planificador");
	}
	if (recibirmensaje(socketplanificador)) {
		printf("recibi another mensaje de sir planificador");
	} else
		printf("error al recibir");

	if (enviarmensaje("nos conectamos sir planificador:)",
			socketplanificador)) {
		printf("envie mensaje a sir planificador");
	}
	// Confirmar que nos conectamo
	/*

	 fgets(message, PACKAGESIZE, stdin);	// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.


	 if (!strcmp(message, "exit\n"))

	 enviar = 0;			// Chequeo que el usuario no quiera salir
	 if (enviar) {

	 enviarmensaje(message, serverSocket);
	 } // Solo envio si el usuario no quiere salir.
	 */
	return NULL;
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
int enviarmensaje(char*mensaje, int unsocket) {
	ContentHeader * cabeza = malloc(sizeof(ContentHeader));
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
