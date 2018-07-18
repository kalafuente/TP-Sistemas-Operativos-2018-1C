#include "Planificador.h"
#define PACKAGESIZE 1024

//empezamos sin la consola, pero separando bien
int main(void) {
	PlanificadorON = 1;
	IdDisponible =0;
	logger = crearLogger("loggerPlani.log", "loggerPlani");
	sem_init(&pausarPlanificacion, 0, 1);
	pthread_t tid;
	pthread_create(&tid, NULL, consola, NULL);
	pthread_mutex_init(&mutex, NULL);

	sem_init(&cantidadEsisEnReady, 0, 0);
	prepararConfiguracion();
	crearListas();
//-------------CONEXION AL COORDINADOR------------------
	int socketCoordinador = conectarseAlCoordinador(planiConfig);

//-----------RECEPTOR DE ESI´S----------------------

	int listenningSocket = crearSocketQueEscucha(&planiConfig->puertoEscucha, &planiConfig->entradas);
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
				switch (planiConfig->algoritmoPlanificacion) {
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
						planiConfig->alfaPlanificacion);

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
			actualizarBloqueado();
			sem_post(&pausarPlanificacion);

//IF (es con desalojo && llego un nuevo esi)->devolver a la listaReady && Salir del while------------------------------------------------------------------------------------------------------------

		}
		actualizarBloqueado();
//Calcular estimacion para la proxima vez.
	}
//Cerrar sockets de los esis que quedaron en el valhalla
	close(listenningSocket);
	close(socketCoordinador);
	destroy_planificadorConfig(planiConfig);
	config_destroy(config);
	return EXIT_SUCCESS;

}

void agregarEnListaBloqueado(struct_esi *esiActual, char*clave) {
	struct_esiClaves *elemento = calloc(1, sizeof(struct_esiClaves));
	elemento->clave = string_new();
	string_append(&elemento->clave, clave);
	elemento->ESI = esiActual;
	list_add(listaBloqueado, elemento);
}

void actualizarBloqueado(){
	int i = 0;
	int j = list_size(listaBloqueado);
	while(i<j){
		struct_esiClaves* esiBloqueado = calloc(1, sizeof(struct_esiClaves));
		esiBloqueado = list_get(listaBloqueado, i);
		if(indexOfString(list_map(listaEsiClave, claveEsiClaves), esiBloqueado->clave) == -1){
			list_remove(listaBloqueado, i);
			list_add(listaReady, esiBloqueado->ESI);
			sem_post(&cantidadEsisEnReady);
		}
		i++;
	}
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
	listaClaves = list_create();
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


void listar(t_list* lista, char* clave){
	log_info(logger, "Entre al listar");
	int i = 0;
	int j = sizeof(lista);
	log_info(logger, "Hice el sizeof");
	while(i<j){
		struct_esiClaves* esiClave = list_get(lista, i);
		log_info(logger, "agarre el esiClave");
		char* claveEsi = calloc(1, sizeof(esiClave->clave));
		strcpy(claveEsi, esiClave->clave);
		log_info(logger, "Copie la clave");
		if(string_equals_ignore_case(claveEsi, clave)){
			log_info(logger, "Entre al if");
			printf("El esi numero %i necesita la clave %s \n", esiClave->ESI->ID, clave);
		}
		i++;
	}
	return;

}

char* claveEsiClaves(struct_esiClaves* esiClave){
	return esiClave->clave;
}

int indexOf(t_list* lista, void* valorBuscado){
	int i = 0;
	int j = sizeof(lista);
	while(i<j){
		if(valorBuscado == list_get(lista, i)){
			return i;
		}
		i++;
	}
	return -1;
}

int indexOfString(t_list* lista, char* valorBuscado){
	int i = 0;
	int j = sizeof(lista);
	while(i<j){
		if(string_equals_ignore_case(valorBuscado, list_get(lista, i))){
			return i;
		}
		i++;
	}
	return -1;
}

void desbloquear(t_list* listaBloqueado, t_list* listaReady, char* clave){
	int i = 0;
	int j = list_size(listaBloqueado);
	printf("Hice el list_size \n");
	list_remove(listaEsiClave, indexOfString(list_map(listaEsiClave, claveEsiClaves), clave));
	while(i<j){
		printf("Entre al while \n");
		struct_esiClaves* esiClave = list_get(listaBloqueado, i);
		if(string_equals_ignore_case(esiClave->clave, clave)){
			list_remove(listaBloqueado, i);
			list_add(listaReady, esiClave->ESI);
			sem_post(&cantidadEsisEnReady);
			return;
			}
		i++;
		}
	}

int idEsi(struct_esi* esi){
	return esi->ID;
}

bool esIgualA(void* elem1, void* elem2){
	return elem1 == elem2;
}

bool contains(t_list* lista, void* elemento){
	int i = 0;
	int j = sizeof(lista);
	while (i<j){
		if (elemento == list_get(lista, i)){
			return true;
		}
		i++;
	}
	return false;
}


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
		if (string_equals_ignore_case(comando, "pausar")) {
			sem_wait(&pausarPlanificacion);
			printf("La planificacion se detuvo \n");
			//El Planificador no le dará nuevas órdenes de ejecución a NINGÚN ESI mientras se encuentre pausado.
		}
		if (string_equals_ignore_case(comando, "resumir")) {
			sem_post(&pausarPlanificacion);
			printf("Resumiendo planificacion \n");
			//Resume la planificación
		}
		if (string_equals_ignore_case(comando, "bloquear")) {
			char* clave = strtok(parametros, " ");
			char* id = strtok(NULL, " ");
			int esSuClaveIgual(struct_esiClaves*elesi) {
					return string_equals_ignore_case(clave, elesi->clave);
				}
			int ID = strtol(id, NULL, 10);
			if(!contains((list_map(listaReady, idEsi)), ID) && !contains((list_map(listaEjecutando, idEsi)), ID)){
				strcpy(id, "ESI NO EXISTENTE");
				//list_remove_by_condition(listaEsiClave, (void*) esSuClaveIgual)
			    if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
			    	list_add(listaEsiClave, crearEsiClave(NULL, clave));
			    		}
			    	}else{
			    	 		if(contains((list_map(listaReady, idEsi)), ID)){
			    				//bloquear(id, clave);
			    	 			int index = indexOf((list_map(listaReady, idEsi)), ID);
			    	 			struct_esi* esiBloqueado = list_remove(listaReady, index);
			    	 			struct_esiClaves* esiClavesBloqueado = crearEsiClave(esiBloqueado, clave);
			    	 			list_add(listaBloqueado, esiClavesBloqueado);
			    	 			/*if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
			    	 				list_add(listaEsiClave, esiClavesBloqueado);
			    	 			}*/
			    			}
			    	 		if(contains((list_map(listaEjecutando, idEsi)), ID)){
			    				//bloquear(id, clave);
			    	 			int index = indexOf((list_map(listaEjecutando, idEsi)), ID);
			    	 			struct_esi* esiBloqueado = list_remove(listaEjecutando, index);
			    	 			struct_esiClaves* esiClavesBloqueado = crearEsiClave(esiBloqueado, clave);
			    	 			list_add(listaBloqueado, esiClavesBloqueado);
			    	 			/*if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
			    	 				list_add(listaEsiClave, esiClavesBloqueado);
			    	 			}*/
			    			}
			    		}
			printf("Se bloqueo la Clave %s para el ESI %s \n", clave, id);
			//Se bloqueará el proceso ESI hasta ser desbloqueado, especificado por dicho ID en la cola del recurso clave.
		}
		if (string_equals_ignore_case(comando, "desbloquear")) {
			char* clave = strtok(parametros, " ");
			int esSuClaveIgual(struct_esiClaves*elesi) {
					return string_equals_ignore_case(clave, elesi->clave);
			}
			if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
				strcpy(clave, "NO EXISTE LA CLAVE");
			}else{
				desbloquear(listaBloqueado, listaReady, clave);
			}

			printf("Se desbloqueo la clave %s \n", clave);
			//Se desbloqueara el primer proceso ESI bloquedo por la clave especificada.
		}
/*		if (string_equals_ignore_case(comando, "listar")) {

			 t_list listaEsperando = list_filter(listaBloqueado);
			 char* esperando = strcpy(toString(listaEsperando));

			printf("El recurso %s esta siendo esperado por: \n", parametros);
			//Lista los procesos encolados esperando al recurso.
		}*/
		if (string_equals_ignore_case(comando, "kill")) {
			printf("Se elimino el proceso %s \n", parametros);
			//Finaliza el proceso. Al momento de eliminar el ESI, se debloquearan las claves que tenga tomadas.
		}
		if (string_equals_ignore_case(comando, "estado")) {
			printf("La siguiente clave %s , esta en el siguiente estado: \n",
					parametros);
			//Conocer el estado de una clave y de probar la correcta distribución de las mismas
		}
		if (string_equals_ignore_case(comando, "deadlock")) {
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
void prepararConfiguracion(){
	config = config_create("configPlanificador.config");
	planiConfig=  init_planificaorConfig();
	crearConfiguracion(planiConfig,config);
}

