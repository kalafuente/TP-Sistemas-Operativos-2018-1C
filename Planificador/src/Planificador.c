#include "Planificador.h"
#define PACKAGESIZE 1024

//empezamos sin la consola, pero separando bien
int main(void) {
	logger = crearLogger("loggerPlani.log", "loggerPlani");
	sem_init(&pausarPlanificacion, 0, 1);
	pthread_t tid;
	pthread_create(&tid, NULL, consola, NULL);
	pthread_mutex_init(&mutex, NULL);

	sem_init(&cantidadEsisEnReady, 0, 0);
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
	pthread_t thread_coordi;

	pthread_create(&thread_coordi, NULL, manejarConexionCoordi,
			(void*) &socketCoordinador);
//-----------------------MANEJAR LOS ESI´s-------------------------------------
	struct_esi *esiActual;
	t_instruccion* instruccion;
	PROTOCOLO_ESI_A_PLANIFICADOR estadoEsi;

//Inicia
	while (PlanificadorON) {
		sem_wait(&cantidadEsisEnReady);
		estadoEsi = TERMINE_BIEN;

//Ordenamos la lista Esis segun criterio elegido en este momento-----------------------------------------------------------------------------
		switch (planificadorConfig->algoritmoPlanificacion) {
		case SJF_CD:
			ordenarPorSJF(listaReady);
			break;

		case SJF_SD:
			ordenarPorSJF(listaReady);
			break;

		case HRRN:
			//ordenarPorHRRN(listaReady);
			break;
		default:
			log_error(logger, "ERROR INTERNO, REVISAR EL CODIGO");
			break;
		}

		esiActual = list_remove(listaReady, 0);
		list_add(listaEjecutando, esiActual);

		while (estadoEsi == TERMINE_BIEN) {
			sem_wait(&pausarPlanificacion);

			ordenarActuar(esiActual);

			if (recibirMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),
					&estadoEsi, esiActual->socket) <= 0) {
				log_error(logger, "ERROR ESI DESCONECTADO");
				estadoEsi = ERROR;
			}
			log_info(logger, "se recibio estado %d", estadoEsi);
//estadoEsi=recibirResultado(esiActual);
			switch (estadoEsi) {
			case TERMINE_BIEN:
				esiActual->estimacion--;
				esiActual->rafagaActual++;
				instruccion = recibirInstruccion(logger, esiActual->socket, "ESI");
				procesarInstruccion(instruccion, esiActual);
//duracionRafaga++;
//cambiarEstimacion(esiActual,-1);
//sumar 1 a la espera te todos los esis en Ready para el HRRN
				destruirInstruccion(instruccion);


				break;
			case BLOQUEADO_CON_CLAVE:
				//cambiarEstimacion();
				esiActual->estimacion--;
				esiActual->rafagaActual++;
				instruccion = recibirInstruccion(logger, esiActual->socket, "ESI");
				list_remove(listaEjecutando, 0);
				agregarEnListaBloqueado(esiActual, instruccion->clave);
				destruirInstruccion(instruccion);
				log_info(logger, "esi %d bloqueado", esiActual->ID);
				cambiarEstimacionSJF(esiActual,
						planificadorConfig->alfaPlanificacion);

				break;
			case TERMINE:

				list_remove(listaEjecutando, 0);
				list_add(listaTerminados, esiActual);
				//liberar claves
				log_info(logger, "termino el esi %d", esiActual->ID);
				close(esiActual->socket);

				break;
			case ERROR:
				list_remove(listaEjecutando, 0);
				list_add(listaTerminados, esiActual);
				//liberar claves
				log_error(logger, "error con el esi %d", esiActual->ID);
				close(esiActual->socket);
				break;
			default:
				list_remove(listaEjecutando, 0);
				log_error(logger, "No deberias ver esto");
				break;
			}
			sem_post(&pausarPlanificacion);

//IF (es con desalojo && llego un nuevo esi)->devolver a la listaReady && Salir del while------------------------------------------------------------------------------------------------------------

		}
//Calcular estimacion para la proxima vez.
	}
//Cerrar sockets de los esis que quedaron en el valhalla
	close(listenningSocket);
	close(socketCoordinador);
	destroy_planificadorConfig(planificadorConfig);
	config_destroy(config);
	return EXIT_SUCCESS;

}

void cambiarEstimacionSJF(struct_esi* esi, int alfa) {
	esi->estimacion = (esi->estimacion + esi->rafagaActual) * (1 - (alfa / 100))
			+ (alfa / 100) * esi->rafagaActual;
}

void agregarEnListaBloqueado(struct_esi *esiActual, char*clave) {
	struct_esiClaves *elemento = calloc(1, sizeof(struct_esiClaves));
	elemento->clave = string_new();
	string_append(&elemento->clave, clave);
	elemento->ESI = esiActual;
}


float actualizarDuracionDeRafagaSJF(struct_esi esi) {
	float duracionEstimada;
	duracionEstimada = 0; //obviamente esto se calcula con el alfa la duracion de la ultima rafaga y la duracion estimada anterior

	return duracionEstimada;
}

double calcularSiguienteRafagaSJF(int t , int t0, double alfa) {
	double t1;
	t1 = (double) (alfa/100)*t + (double) (1-(alfa/100))*t0;
	printf("Esta rafaga es de %f \n", t1);
	return t1;
}

double calcularRafagaSJF(struct_esi* esi, double alfa){
	int t0 = esi->estimacion;
	int t = 0;// esi->duracionRafaga;
	double t1 = calcularSiguienteRafagaSJF(t, t0, alfa);
	return t1;
}

void actualizarEstimacionSJF(struct_esi* esi, double alfa){
	esi->estimacion = calcularRafagaSJF(esi, alfa);
	//esi->duracionRafaga = 0;
}

bool tieneMenorRafaga(struct_esi* esi1, struct_esi* esi2) {
	return esi1->estimacion < esi2->estimacion;
}

void ordenarPorSJF(t_list *listaAOrdenar) {
	if (sizeof(listaAOrdenar) == 1) {
		return;
	}
	list_sort(listaAOrdenar, (void*) tieneMenorRafaga);
}
//--------------------------CALMANDO PREOCUPACIONES DEL CORDI--------------------------------
void * manejarConexionCoordi(void * socket) {
	int *socketCoordinador = (int*) socket;
	PROTOCOLO_PLANIFICADOR_A_COORDINADOR respuesta;
	PROTOCOLO_COORDINADOR_A_PLANIFICADOR mensajeRecibido;
	//int ID;
	char * CLAVE = string_new();
	struct_esi* ESI;
	int respuesta_bool;
	log_info(logger, "Iniciada recepcion de consultas del coordi");
	while (recibirMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_PLANIFICADOR),
			&mensajeRecibido, *socketCoordinador) > 0) {

		switch (mensajeRecibido) {
		case PEDIDO_DE_ID:
			log_info(logger, "Cordi pidio ID");
			ESI = list_get(listaEjecutando, 0);
			enviarMensaje(logger, sizeof(int), &ESI->ID, *socketCoordinador);
			log_info(logger, "Le mande ID al Coordi");
			break;

		case PREGUNTA_ESI_TIENE_CLAVE:
			log_info(logger, "PREGUNTA_ESI_TIENE_CLAVE");

			CLAVE= recibirID( *socketCoordinador,logger);
			log_info(logger, "La clave es %s", CLAVE);
			respuesta_bool = perteneceClaveAlEsi(listaEsiClave, CLAVE);

			if (respuesta_bool)
				respuesta = ESI_TIENE_CLAVE;
			else
				respuesta = ESI_NO_TIENE_CLAVE;

			enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),
					&respuesta, *socketCoordinador);
			CLAVE = string_new();
			break;
		case PREGUNTA_CLAVE_DISPONIBLE:
			log_info(logger, "PREGUNTA_CLAVE_DISPONIBLE");
			CLAVE= recibirID(*socketCoordinador, logger);
			respuesta_bool = tieneAlgunEsiLaClave(listaEsiClave, CLAVE);
			if (respuesta_bool)
				respuesta = CLAVE_NO_DISPONIBLE;
			else
				respuesta = CLAVE_DISPONIBLE;
			enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_COORDINADOR),
					&respuesta, *socketCoordinador);

			CLAVE = string_new();
			break;

		default:
			log_error(logger, "ERROR ESTE MENSAJE, MENSAJE NO ANTICIPADO ");
			break;
		}
	}
	log_error(logger, "SE CORTO LA CONEXION CON EL COORDI");

	return 0;
}

int tieneAlgunEsiLaClave(t_list* lista, char *claveBuscada) {
	int _soy_la_clave_buscada(struct_esiClaves * elemento) {

		return (string_equals_ignore_case(elemento->clave, claveBuscada));

	}
	return (list_any_satisfy(lista, (void*) _soy_la_clave_buscada));
}

int perteneceClaveAlEsi(t_list *lista, char* claveBuscada) {
	int _soy_la_clave_buscada(struct_esiClaves * elemento) {

		struct_esi* esiEjecutando = list_get(listaEjecutando, 0);
		return (string_equals_ignore_case(elemento->clave, claveBuscada))
				&& (elemento->ESI->ID == esiEjecutando->ID);
	}
	return (list_any_satisfy(lista, (void*) _soy_la_clave_buscada));
}

//--------------------------------CONVERSANDO CON LOS ESIS------------------





void procesarInstruccion(t_instruccion* instruccion, struct_esi*esi) {
	char *clave = string_new();
	string_append(&clave, instruccion->clave);
	
	switch (instruccion->instruccion) {
	case INSTRUCCION_GET:

		list_add(listaEsiClave, crearEsiClave(esi, clave));
		log_info(logger, "empezamos a procesar");

		log_info(logger, "FINJAMOS QUE PROCESE LA INSTRU get");

		//bloquear la correspondiente
		break;
	case INSTRUCCION_SET:
		log_info(logger, "FINJAMOS QUE PROCESE LA INSTRU set");

		//no hacer nada
		break;
	case INSTRUCCION_STORE:
		log_info(logger, "FINJAMOS QUE PROCESE LA INSTRU store");

		sacarStructDeListaEsiClave(clave);
		liberarEsi(clave);
		//liberar la correspondiente
		break;
	}
}
//---------------------------------------------------------------STORE / LIBERAR -------------------------------------
void liberarEsi(char*clave) {
	struct_esiClaves* aux;
	int esSuClaveIgual(struct_esiClaves*elesi) {
		return string_equals_ignore_case(clave, elesi->clave);
	}

	aux = (struct_esiClaves*) list_remove_by_condition(listaBloqueado,
			(void*) esSuClaveIgual);
	if (aux != NULL) {
		list_add(listaReady, aux->ESI);
	}
}

void sacarStructDeListaEsiClave(char*clave) {
	int esSuClaveIgual(struct_esiClaves*elesi) {
		return string_equals_ignore_case(clave, elesi->clave);
	}

	list_remove_by_condition(listaEsiClave, (void*) esSuClaveIgual);
}
//--------------------------------------------------------------------------------------------------------


struct_esiClaves* crearEsiClave(struct_esi* esi, char*clave) {
	struct_esiClaves* aux = calloc(1, sizeof(struct_esiClaves));
	aux->ESI = esi;
	aux->clave = string_new();
	string_append(&aux->clave, clave);
	return aux;
}

void ordenarActuar(struct_esi* esi) {

	PROTOCOLO_PLANIFICADOR_A_ESI mensajeParaEsi = ACTUAR;

	if (enviarMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
			&mensajeParaEsi, esi->socket) <= 0) {
		log_error(logger, "NO SE PUDO ENVIAR ACCION AL ESI");
	} else {
		log_info(logger, "Se envio accion al Esi");
	}

}
PROTOCOLO_ESI_A_PLANIFICADOR recibirResultado(struct_esi* esi) {
	PROTOCOLO_ESI_A_PLANIFICADOR resultado;
	recibirMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
			esi->socket);
	//recv + decodificar Mensaje
	return resultado;
}

//-----------------------------INICIANDO LAS LISTAS--------------

void agregarEsi(int socketCliente) {
	struct_esi *nuevoEsi = calloc(1, sizeof(struct_esi));
	nuevoEsi->estimacion = 5;
	nuevoEsi->rafagaActual = 0;
	nuevoEsi->socket = socketCliente;
	nuevoEsi->tiempoDeEspera = 0;
	nuevoEsi->ID = IdDisponible;
	IdDisponible++;
	list_add(listaReady, nuevoEsi);
	sem_post(&cantidadEsisEnReady);
}

void crearListas() {
	listaReady = list_create();
	listaBloqueado = list_create();
	listaEjecutando = list_create();
	listaTerminados = list_create();
	listaEsiClave = list_create();
}

void * recibirEsi(void* socketEscucha) {
	int listeningSocket = *(int*) socketEscucha;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente;
	PROTOCOLO_PLANIFICADOR_A_ESI handshakeEsi;
	PROTOCOLO_ESI_A_PLANIFICADOR hanshakeEP;
	while (PlanificadorON) {
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

//--------------------------FUNCIONES DE CONFIGURACION--------------------------
planificador_config * init_planificaorConfig() {
	planificador_config * planificador = calloc(1, sizeof(planificador_config));
	planificador->puertoEscucha = calloc(5, sizeof(char));
	planificador->ipCoordinador = calloc(20, sizeof(char));
	planificador->puertoCoordinador = calloc(5, sizeof(char));
	//planificador->algoritmoPlanificacion = calloc(10, sizeof(char));
	return planificador;
}

void crearConfiguracion(planificador_config** planificador, t_config** config) {
	*config = config_create("configPlanificador.config");

	ALGORITMO_PLANIFICACION i = traducir(
			config_get_string_value(*config, "ALGORITMO"));

	(*planificador)->algoritmoPlanificacion = i;
	(*planificador)->ipCoordinador = config_get_string_value(*config,
			"IP_COORDINADOR");

	(*planificador)->puertoCoordinador = config_get_string_value(*config,
			"PUERTO_COORDINADOR");
	(*planificador)->puertoEscucha = config_get_string_value(*config,
			"PUERTO_DE_ESCUCHA");
	(*planificador)->alfaPlanificacion = config_get_int_value(*config, "ALFAP");
	(*planificador)->estimacionInicial = config_get_double_value(*config,
			"ESTIMACION");
	(*planificador)->entradas = 500;
}

void destroy_planificadorConfig(planificador_config* planificador_config) {
	free(planificador_config->ipCoordinador);
	free(planificador_config->puertoCoordinador);
	free(planificador_config->puertoEscucha);
	free(planificador_config);
}

ALGORITMO_PLANIFICACION traducir(char* algoritmo) {
	if (string_equals_ignore_case(algoritmo, "SJF_SD")) {
		return SJF_SD;
	}
	if (string_equals_ignore_case(algoritmo, "HRRN")) {
		return HRRN;
	}

	return SJF_CD;

}

//----------------------------------------------
//------------------<CONSOLA>-------------------
//----------------------------------------------

void procesarLinea(char* linea, char ** comando, char ** parametros) {
	//printf("Entre a la funcion \n");
	int i = 0;
	while (linea[i] != ' ' && linea[i] != '\0' && linea[i] != '\n') {
		i++;
	}
	//printf("%i \n", i);
	//printf("Hice el malloc \n");
	char * comandoCopy = calloc(i, sizeof(char*));
	i = 0;
	while (linea[i] != ' ' && linea[i] != '\0' && linea[i] != '\n') {
		comandoCopy[i] = linea[i];
		i++;
	}
	//printf("El comando es: %s \n", comandoCopy);
	strcpy(*comando, comandoCopy);
	free(comandoCopy);
	if (linea[i] == '\n' || linea[i] == '\0') {
		return;
	}
	i++;
	int j = 0;
	int k = i;
	while (linea[i] != '\0') {
		i++;
		j++;
	}
	//printf("%i %i %i \n", i, j, k);
	char * paramCopy = calloc(j, sizeof(char*));
	j = 0;
	while (linea[k] != '\0' && linea[k] != '\n') {
		paramCopy[j] = linea[k];
		k++;
		j++;
	}
	//printf("Los parametros son: %s \n", paramCopy);
	strcpy(*parametros, paramCopy);
	free(paramCopy);
}


//void listar(t_list* lista, char* clave){
//	log_info(logger, "Entre al listar");
//	int i = 0;
//	int j = sizeof(lista);
//	log_info(logger, "Hice el sizeof");
//	while(i<j){
//		struct_esiClaves* esiClave = list_get(lista, i);
//		log_info(logger, "agarre el esiClave");
//		char* claveEsi = calloc(1, sizeof(esiClave->clave));
//		strcpy(claveEsi, esiClave->clave);
//		log_info(logger, "Copie la clave");
//		if(strcmp(claveEsi, clave) == 0){
//			log_info(logger, "Entre al if");
//			printf("El esi numero %i necesita la clave %s \n", esiClave->ESI->ID, clave);
//		}
//		i++;
//	}
//	return;
//
//}

//
//void desbloquear(t_list* listaBloqueado, t_list* listaReady, char* clave){
//	int i = 0;
//	int j = list_size(listaBloqueado);
//	printf("Hice el list_size \n");
//	while(i<j){
//		printf("Entre al while \n");
//		struct_esiClaves* esiClave = list_get(listaBloqueado, i);
//		if(strcmp(esiClave->clave, clave) == 0){
//			list_remove(listaBloqueado, i);
//			list_add(listaReady, esiClave);
//			return;
//			}
//		i++;
//		}
//	}


//int indexOf(t_list* lista, int valorBuscado){
//	int i;
//	int j = sizeof(lista);
//	while(i<j){
//		if(valorBuscado == list_get(lista, i)){
//			return i;
//		}
//		i++;
//	}
//	return -1;
//}

//int contains(t_list* lista, int elemento){
//	int i = 0;
//	int j = sizeof(lista);
//	while (i<j){
//		if (elemento == list_get(lista, i)){
//			return true;
//		}
//		i++;
//	}
//	return false;
//}


void* consola() {
	char * linea;
	char * comando = calloc(10, sizeof(char*));
	char * parametros = calloc(100, sizeof(char*));
	while (1) {
		linea = readline((">"));
		if (linea)
			add_history(linea);
		if (!strncmp(linea, "exit", 4)) {
			free(linea);
			free(comando);
			free(parametros);
			PlanificadorON = 0;
			break;
		}
		// printf("Lei la linea \n");
		procesarLinea(linea, &comando, &parametros);

		//Pausar/Resumir
		//Bloquear [Clave] [ID]
		//Desbloquear [Clave]
		//Listar [Recurso]
		//Kill [ID]
		//Estado [Clave]
		//Deadlock
		if (!strncmp(comando, "pausar", 6)) {
			sem_wait(&pausarPlanificacion);
			printf("La planificacion se detuvo \n");
			//El Planificador no le dará nuevas órdenes de ejecución a NINGÚN ESI mientras se encuentre pausado.
		}
		if (!strncmp(comando, "resumir", 7)) {
			sem_post(&pausarPlanificacion);
			printf("Resumiendo planificacion \n");
			//Resume la planificación
		}
		if (!strncmp(comando, "bloquear", 8)) {
			char* clave = strtok(parametros, " ");
			char* id = strtok(NULL, " ");
//			struct_esi* esiID = calloc(1, sizeof(struct_esi));
//			esiID->ID = strtol(id, NULL, 10);
//			if(!contains((list_map(listaReady, idEsi)), id) && !contains((list_map(listaEjecutando, idEsi)), id)){
//				strcpy(id, "ESI NO EXISTENTE");
//			    if(!contains(listaEsiClave, clave)){
//			    	list_add(listaEsiClave, crearEsiClaves(NULL, clave));
//			    		}
//			    	}else{
//			    			if(!contains(listaEsiClave, clave)){
//			    				list_add(listaEsiClave, crearEsiClaves(esiID, clave));
//			    			}
//			    	 		if(contains((list_map(listaReady, idEsi)), id)){
//			    				//bloquear(id, clave);
//			    	 			int index = indexOf((list_map(listaReady, idEsi)), id);
//			    	 			struct_esi* esiBloqueado = list_remove(listaReady, index);
//			    	 			struct_esiClaves* esiClavesBloqueado = crearEsiClaves(esiBloqueado, clave);
//			    	 			list_add(listaBloqueado, esiClavesBloqueado);
//			    			}
//			    	 		if(contains((list_map(listaEjecutando, idEsi)), id)){
//			    				//bloquear(id, clave);
//			    	 			int index = indexOf((list_map(listaEjecutando, idEsi)), id);
//			    	 			struct_esi* esiBloqueado = list_remove(listaEjecutando, index);
//			    	 			struct_esiClaves* esiClavesBloqueado = crearEsiClaves(esiBloqueado, clave);
//			    	 			list_add(listaBloqueado, esiClavesBloqueado);
//			    			}
//			    		}
			printf("Se bloqueo la Clave %s para el ESI %s \n", clave, id);
			//Se bloqueará el proceso ESI hasta ser desbloqueado, especificado por dicho ID en la cola del recurso clave.
		}
		if (!strncmp(comando, "desbloquear", 11)) {

			//if(!contains(listaClaves, parametros){
			//	parametros = strcpy("NO EXISTE LA CLAVE");
			//}else{
			//	desbloquear(listaBloqueados, clave);
			//}

			printf("Se desbloqueo la clave %s \n", parametros);
			//Se desbloqueara el primer proceso ESI bloquedo por la clave especificada.
		}
		if (!strncmp(comando, "listar", 6)) {

			// t_list listaEsperando = filter(listaBloqueados, elem.clave == parametros);
			// char* esperando = strcpy(toString(listaEsperando));

			printf("El recurso %s esta siendo esperado por: \n", parametros);
			//Lista los procesos encolados esperando al recurso.
		}
		if (!strncmp(comando, "kill", 4)) {
			printf("Se elimino el proceso %s \n", parametros);
			//Finaliza el proceso. Al momento de eliminar el ESI, se debloquearan las claves que tenga tomadas.
		}
		if (!strncmp(comando, "estado", 6)) {
			printf("La siguiente clave %s , esta en el siguiente estado: \n",
					parametros);
			//Conocer el estado de una clave y de probar la correcta distribución de las mismas
		}
		if (!strncmp(comando, "deadlock", 8)) {
			printf("El sistema no encuentra deadlocks actualmente \n");
			//Esta consola también permitirá analizar los deadlocks que existan en el sistema y a que ESI están asociados.
		}
		printf("%s\n", linea);
		free(linea);
	}
	return 0;
}

//pthread_t tid;
//pthread_create(&tid, NULL, consola, NULL);

