
#include "Esi.h"

#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

int main() {
	t_config *config = malloc(sizeof(t_config));

	/*
	esi_config* esiConfig = init_esiConfig();
	crearConfiguracion(&esiConfig,&config);


	*/
	char *ipCoordi = malloc(sizeof(char) * 5);
	char *puertoCoordi = malloc(sizeof(char) * 20);
	char *idPlanificador = malloc(sizeof(char) * 20);
	char *puertoPlanificador = malloc(sizeof(char) * 5);

	crearConfiguracion(&ipCoordi, &puertoCoordi, &idPlanificador, &puertoPlanificador, &config);

	int coordinadorSocket;
	//coordinadorSocket = conectarseAlServidor(&esiConfig->ipCoordi, &esiConfig->puertoCoordi);
	coordinadorSocket = conectarseAlServidor(&ipCoordi, &puertoCoordi);


	int planificadorSocket = conectarseAlServidor(&idPlanificador,&puertoPlanificador);


	pthread_t thread_id;
	pthread_create(&thread_id, NULL, conexionPlanificador,(void*) &planificadorSocket);



	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");

	if (recibirmensaje(coordinadorSocket)) {
		printf("recibimensaje del coordinador");
	};

	if (enviarmensaje("Soy el esi y me he conectado contigo mi lord coordinador :)",coordinadorSocket)) {
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

	//destroy_esiConfig(esiConfig);
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
		printf("\nrecibimensaje de sir planificador \n");
	}
	if (recibirmensaje(socketplanificador)) {
		printf("recibi another mensaje de sir planificador \n");
	} else
		printf("error al recibir");

	if (enviarmensaje("nos conectamos sir planificador, soy el esi para servirle \n",
			socketplanificador)) {
		printf("io esi, envie mensaje a sir planificador");
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
/*
void crearConfiguracion(esiConfig** esiConfig, t_config ** config) {

	*config = config_create(
			"configuracion.config");
	(*esiConfig)->ipCoordi = config_get_string_value(*config, "IP_COORDINADOR");
	(*esiConfig)->puertoCoordi = config_get_string_value(*config, "PUERTO_COORDINADOR");
	(*esiConfig)->idPlanificador = config_get_string_value(*config, "IP_PLANIFICADOR");
	(*esiConfig)->puertoPlanificador = config_get_string_value(*config,
			"PUERTO_PLANIFICADOR");
}

void destroy_esiConfig(esiConfig * esi){

	free(esi->idPlanificador);
	free(esi->ipCoordi);
	free(esi->puertoCoordi);
	free(esi->puertoPlanificador);
	free(esi);
}

esiConfig* init_esiConfig(){
	esiConfig* esiConfig=malloc(sizeof(esiConfig));

	esiConfig->ipCoordi = malloc(sizeof(char) * 5);
	esiConfig->puertoCoordi = malloc(sizeof(char) * 20);
	esiConfig->idPlanificador = malloc(sizeof(char) * 20);
	esiConfig->puertoPlanificador = malloc(sizeof(char) * 5);
	return esiConfig;
}
*/

