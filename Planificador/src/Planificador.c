#include "Planificador.h"
#define PACKAGESIZE 1024
int main(void) {

	logger = crearLogger("loggerPlani.log","loggerPlani");
	planificador_config * planificadorConfig = init_planificaorConfig();
	t_config * config;
	crearConfiguracion(&planificadorConfig,&config);
	int listenningSocket = crearSocketQueEscucha(&planificadorConfig->puertoEscucha, &planificadorConfig->entradas);

	crearServidorMultiHilo(listenningSocket);

	close(listenningSocket);
	destroy_planificadorConfig(planificadorConfig);
	config_destroy(config);
	return EXIT_SUCCESS;

}


planificador_config * init_planificaorConfig(){
	planificador_config * planificador = malloc(sizeof(planificador_config));
	planificador->puertoEscucha = malloc(sizeof(char) * 20);
	planificador->ipCoordinador = malloc(sizeof(char) * 20);
	planificador->puertoCoordinador = malloc(sizeof(char) * 20);
	planificador->entradas=20;
	return planificador;
}

void crearConfiguracion(planificador_config** planificador,t_config** config){
	*config = config_create("configPlanificador.config");
	(*planificador)->ipCoordinador = config_get_string_value(*config, "IP_COORDINADOR");
	(*planificador)->puertoCoordinador = config_get_string_value(*config, "PUERTO_COORDINADOR");
	(*planificador)->puertoEscucha = config_get_string_value(*config, "PUERTO_DE_ESCUCHA");
}
void destroy_planificadorConfig(planificador_config* planificador_config){
	free(planificador_config->ipCoordinador);
	free(planificador_config->puertoCoordinador);
	free(planificador_config->puertoEscucha);
	free(planificador_config);
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
	enviarMensaje(logger, ID_PLANIFICADOR, "SoyPlani", sock); //Saludamos
	recibirIDyContenido(&id, logger, sock);

	switch(id) {

		case ID_ESI  :
		printf("Se me conectó un ESI \n ");
		recibirIDyContenido(&id, logger, sock);
				   //Espero la instrucción proveniendo del ESI
				   //char* instruccion =recibirIDyContenido(logger, sock, &id);
				   //actualizarLogDeOperaciones();
		break;

			  // case ID_INSTANCIA :
			     //pasa lo que pasa con la instacia
			    //  break; /* optional */

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
