#include "Planificador.h"
#define PACKAGESIZE 1024

//empezamos sin la consola, pero separando bien
t_list *listaReady, *listaBloqueado, *listaEjecutando, *listaTerminados; //creamos listas para situacion de los Esi's

int IdDisponible = 0;
pthread_mutex_t mutex; //puede que despues se necesitan mas(por ahora solo protege la cola Ready, de cuando llegan y cuando la usa)
int main(void) {

//definir struct de las claves guardadas---------------------------<Falta>--------------------------------------------------------------------------
	pthread_mutex_init(&mutex, NULL);
	logger = crearLogger("loggerPlani.log", "loggerPlani");
	planificador_config * planificadorConfig = init_planificaorConfig();
	t_config * config;
	crearConfiguracion(&planificadorConfig, &config);
	crearListas();
//-------------CONEXION AL COORDINADOR------------------
	int socketCoordinador = conectarseAlServidor(logger,
			&planificadorConfig->ipCoordinador,
			&planificadorConfig->puertoCoordinador);
	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;
	recibirMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES),
			&handshakeCoordi, socketCoordinador);
	PROTOCOLO_HANDSHAKE_CLIENTE handshakePlani =
			HANDSHAKE_CONECTAR_PLANIFICADOR_A_COORDINADOR;
	enviarMensaje(logger, sizeof(PROTOCOLO_HANDSHAKE_CLIENTE), &handshakePlani,
			socketCoordinador);

	
//-----------RECEPTOR DE ESI´S----------------------

	int listenningSocket = crearSocketQueEscucha(
			&planificadorConfig->puertoEscucha, &planificadorConfig->entradas);
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, recibirEsi, (void*) &listenningSocket);

//-----------------------MANEJAR LOS ESI´s-------------------------------------
	struct_esi *esiActual;
	PROTOCOLO_ESI_A_PLANIFICADOR estadoEsi;
	int duracionRafaga;
	int podemosActuar = 1;
	//Inicia

	while (podemosActuar) {
		while (list_size(listaReady) > 0) {
			sleep(5);
	duracionRafaga = 0;
			estadoEsi = TERMINE_BIEN;

			//Ordenamos la lista Esis segun criterio elegido-------------------------------<Falta>-----------------------------------------------------

	esiActual = list_remove(listaReady, 0);
		while (estadoEsi == TERMINE_BIEN) {
				log_info(logger, "Se esta Por enviar accion al esi");

				ordenarActuar(esiActual);
				log_info(logger, "Se envio accion al esi");
		estadoEsi = recibirResultado(esiActual); //en realidad estadoEsi sera solo parte del struct que recibira

		duracionRafaga++;
//IF (es con desalojo&llego un nuevo esi)devolver a la lista&&Salir del while------------------------------------------------------------------------------------------------------------
	}
	//estadoEsi=recibirResultado(esiActual);
	switch (estadoEsi) {
	case BLOQUEADO:

		break;
		case TERMINE:
				log_info(logger, "termino el esi");
		break;
	}

//fin}
		}
	}

	close(listenningSocket);
	close(socketCoordinador);
	destroy_planificadorConfig(planificadorConfig);
	config_destroy(config);
	return EXIT_SUCCESS;

}


float actualizarDuracionDeRafagaSJF(struct_esi esi) {
	float duracionEstimada;
	duracionEstimada = 0; //obviamente esto se calcula con el alfa la duracion de la ultima rafaga y la duracion estimada anterior

	return duracionEstimada;
}

PROTOCOLO_ESI_A_PLANIFICADOR recibirResultado(struct_esi* esi) {
	PROTOCOLO_ESI_A_PLANIFICADOR resultado;
	recibirMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
			esi->socket);
	//recv + decodificar Mensaje
	return resultado;
}

void ordenarActuar(struct_esi* esi) {
//send al esi con la orden ACTUAR
	PROTOCOLO_PLANIFICADOR_A_ESI mensajeParaEsi = ACTUAR;

	enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI), &mensajeParaEsi,
			esi->socket);
}

void * recibirEsi(void* socketEscucha) {
	int listeningSocket = *(int*) socketEscucha;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente;
	PROTOCOLO_PLANIFICADOR_A_ESI handshakeEsi;
	PROTOCOLO_ESI_A_PLANIFICADOR hanshakeEP;
	while (1) {
		socketCliente = accept(listeningSocket, (struct sockaddr *) &addr,
				&addrlen);
		log_info(logger, "se conecto un esi");
		pthread_mutex_lock(&mutex);	//La condicion del while se puede reemplazar por otra cosa que permita dejar de recibir nuevos esis
		handshakeEsi = HANDSHAKE_CONECTAR_PLANIFICADOR_A_ESI;
		enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
				&handshakeEsi, socketCliente);
		recibirMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),
				&hanshakeEP, socketCliente);

		agregarEsi(socketCliente);
		pthread_mutex_unlock(&mutex);
		//signal
	}
	return NULL;

}

void agregarEsi(int socketCliente) {
	struct_esi *nuevoEsi = calloc(1, sizeof(struct_esi));
	nuevoEsi->socket = socketCliente;
	nuevoEsi->tiempoDeEspera = 0;
	nuevoEsi->ID = IdDisponible;
	IdDisponible++;
	list_add(listaReady, nuevoEsi);
}

void crearListas() {
	listaReady = list_create();
	listaBloqueado = list_create();
	listaEjecutando = list_create();
	listaTerminados = list_create();
}

planificador_config * init_planificaorConfig() {
	planificador_config * planificador = calloc(1, sizeof(planificador_config));
	planificador->puertoEscucha = calloc(5, sizeof(char));
	planificador->ipCoordinador = calloc(20, sizeof(char));
	planificador->puertoCoordinador = calloc(5, sizeof(char));
	planificador->algoritmoPlanificacion = calloc(10, sizeof(char));
	return planificador;
}

void crearConfiguracion(planificador_config** planificador, t_config** config) {
	*config = config_create("configPlanificador.config");

	(*planificador)->algoritmoPlanificacion = config_get_string_value(*config,
			"ALGORITMO");
	(*planificador)->ipCoordinador = config_get_string_value(*config,
			"IP_COORDINADOR");
	(*planificador)->puertoCoordinador = config_get_string_value(*config,
			"PUERTO_COORDINADOR");
	(*planificador)->puertoEscucha = config_get_string_value(*config,
			"PUERTO_DE_ESCUCHA");
	(*planificador)->alfaPlanificacion = config_get_int_value(*config, "ALFAP");
	(*planificador)->estimacionInicial = config_get_int_value(*config,
			"ESTIMACION");
	(*planificador)->entradas = 500;
}
void destroy_planificadorConfig(planificador_config* planificador_config) {
	free(planificador_config->ipCoordinador);
	free(planificador_config->puertoCoordinador);
	free(planificador_config->puertoEscucha);
	free(planificador_config);
}
/*
void crearServidorMultiHilo(int listenningSocket) {
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int socketCliente;
	pthread_t thread_id;

	while ((socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen))) {
		puts("Cliente conectado. Esperando mensajes prueba:\n");

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
 }*/
/*
 void *manejadorDeConexiones(void *socket_desc) {

 int sock = *(int*) socket_desc;
 int id;
 enviarMensaje(logger, ID_PLANIFICADOR, "SoyPlani", sock); //Saludamos
 recibirIDyContenido(&id, logger, sock);

 switch (id) {

 case ID_ESI:
 printf("Se me conectó un ESI \n ");
 recibirIDyContenido(&id, logger, sock);
 //Espero la instrucción proveniendo del ESI
 //char* instruccion =recibirIDyContenido(logger, sock, &id);
 //actualizarLogDeOperaciones();
 break;

 // case ID_INSTANCIA :
 //pasa lo que pasa con la instacia
 //  break;

 // case ID_PLANIFICADOR:
 //pasa plani
 //  break;

 //default :
 //Es alguien desconocido
 //statement(s);
 }
 close(sock);
 printf("\n termino el hilo");
 return NULL;

 }
 */
