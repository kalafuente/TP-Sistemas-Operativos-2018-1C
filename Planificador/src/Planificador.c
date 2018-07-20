#include "Planificador.h"

int main(int argc, char **argv) {
	inicializar();
	prepararLogger();
	prepararConfiguracion(argc,argv);
	inicializarSemaforos();
	crearListas();

	pthread_t tid;
	pthread_create(&tid, NULL, consola, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutexKillEsi, NULL);
	pthread_mutex_lock(&mutexKillEsi);
//-------------CONEXION AL COORDINADOR------------------
	int socketCoordinador = conectarseAlCoordinador(planiConfig);
//-----------RECEPTOR DE ESI´S----------------------

	int listenningSocket = crearSocketQueEscucha(&planiConfig->puertoEscucha, &planiConfig->entradas);
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, recibirEsi, (void*) &listenningSocket);
	pthread_t thread_coordi;
	pthread_create(&thread_coordi, NULL, manejarConexionCoordi,(void*) &socketCoordinador);
	pthread_t thread_bloqueados;
	pthread_create(&thread_bloqueados, NULL, actualizarBloqueado, NULL);

	planificarESIs();

//Cerrar sockets de los esis que quedaron en el valhalla
	close(listenningSocket);
	close(socketCoordinador);
	destuirListas();
	destroy_planificadorConfig(planiConfig);
	config_destroy(config);
	log_destroy(logger);
	return EXIT_SUCCESS;

}


void destuirListas() {
	destruirListaEsiClave(listaEsiClave);
	destruirListaEsi(listaReady);
	destruirListaEsi(listaTerminados);
	destruirListaEsi(listaEjecutando);
	destruirListaEsi(listaEjecutando);
	destruirListaEsiClave(listaBloqueado);

}


void* actualizarBloqueado(){
	while(PlanificadorON){
		sem_wait(&huboDesalojoClaves);
		pthread_mutex_lock(&mutex);
		int i = 0;
		int j = list_size(listaBloqueado);
		while(i<j){
			struct_esiClaves* esiBloqueado = calloc(1, sizeof(struct_esiClaves));
			esiBloqueado = list_get(listaBloqueado, i);
			char* clave = string_new();
			//printf("EN BLOQUEADO LA CLAVE DEL ESI ES %s \n", esiBloqueado->clave);
			string_append(&clave, esiBloqueado->clave);
			//printf("EN BLOQUEADO LA CLAVE ES %s \n", clave);
			int esSuClaveIgual(struct_esiClaves*elesi) {
				return string_equals_ignore_case(clave, elesi->clave);
			}
			if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
				list_remove(listaBloqueado, i);
				list_add(listaReady, esiBloqueado->ESI);
				sem_post(&cantidadEsisEnReady);
				j = list_size(listaBloqueado);
				i = 0;
			}else{
			i++;
			}
		}
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}


void crearListas() {
	listaReady = list_create();
	listaBloqueado = list_create();
	listaEjecutando = list_create();
	listaTerminados = list_create();
	listaEsiClave = list_create();
	listaClaves = list_create();
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


void listar(char* clave){
	//log_info(logger, "Entre al listar");
	int i = 0;
	int j = list_size(listaBloqueado);
	//log_info(logger, "Hice el sizeof");
	while(i<j){
		//printf("El valor de i es %i y el valor de j es %i \n", i, j);
		struct_esiClaves* esiClave = list_get(listaBloqueado, i);
		//log_info(logger, "agarre el esiClave");
		char* claveEsi = string_new();
		string_append(&claveEsi, esiClave->clave);
		//log_info(logger, "Copie la clave");
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

int indexOf(t_list* lista, int valorBuscado){
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
	//printf("Hice el list_size \n");
	list_remove(listaEsiClave, indexOfString(list_map(listaEsiClave, (void *) claveEsiClaves), clave));
	sem_post(&huboDesalojoClaves);
	while(i<j){
		//printf("Entre al while \n");
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

bool contains(t_list* lista, int elemento){
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
	int IDaux;
	struct_esi * Esiaux;
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
			signal(SIGINT, SIG_DFL);
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
			IDaux = (int) strtol(id, (char**) NULL, 10);
			pthread_mutex_lock(&mutex);
			if (!contains((list_map(listaReady, idEsi)), IDaux)
					&& !contains((list_map(listaEjecutando, idEsi)), IDaux)) {
				strcpy(id, "ESI NO EXISTENTE");
				//list_remove_by_condition(listaEsiClave, (void*) esSuClaveIgual)
			    if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
			    	struct_esi* esiNoExistente = calloc(1, sizeof(struct_esi));
			    	esiNoExistente->ID = -1;
			    	list_add(listaEsiClave, crearEsiClave(esiNoExistente, clave));
			    		}
			    	}else{
				if (contains((list_map(listaReady, idEsi)), IDaux)) {
			    				//bloquear(id, clave);
					int index = indexOf((list_map(listaReady, idEsi)), IDaux);
			    	 			struct_esi* esiBloqueado = list_remove(listaReady, index);
			    	 			struct_esiClaves* esiClavesBloqueado = crearEsiClave(esiBloqueado, clave);
			    	 			list_add(listaBloqueado, esiClavesBloqueado);
			    	 			/*if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
			    	 				list_add(listaEsiClave, esiClavesBloqueado);
			    	 			}*/
			    			}
				if (contains((list_map(listaEjecutando, idEsi)), IDaux)) {
			    				//bloquear(id, clave);
					int index = indexOf((list_map(listaEjecutando, idEsi)),
							IDaux);
			    	 			struct_esi* esiBloqueado = list_remove(listaEjecutando, index);
			    	 			struct_esiClaves* esiClavesBloqueado = crearEsiClave(esiBloqueado, clave);
			    	 			list_add(listaBloqueado, esiClavesBloqueado);
			    	 			/*if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
			    	 				list_add(listaEsiClave, esiClavesBloqueado);
			    	 			}*/
			    			}
			    		}
			pthread_mutex_unlock(&mutex);
			printf("Se bloqueo la Clave %s para el ESI %s \n", clave, id);
			//Se bloqueará el proceso ESI hasta ser desbloqueado, especificado por dicho ID en la cola del recurso clave.
		}
		if (string_equals_ignore_case(comando, "desbloquear")) {
			char* clave = strtok(parametros, " ");
			int esSuClaveIgual(struct_esiClaves*elesi) {
					return string_equals_ignore_case(clave, elesi->clave);
			}
			pthread_mutex_lock(&mutex);
			if(!list_any_satisfy(listaEsiClave, (void*) esSuClaveIgual)){
				strcpy(clave, "NO EXISTE LA CLAVE");
			}else{
				desbloquear(listaBloqueado, listaReady, clave);
			}
			pthread_mutex_unlock(&mutex);

			printf("Se desbloqueo la clave %s \n", clave);
			//Se desbloqueara el primer proceso ESI bloquedo por la clave especificada.
		}
		if (string_equals_ignore_case(comando, "listar")) {

			char* clave = string_new();
			string_append(&clave, parametros);
			printf("El recurso %s esta siendo esperado por: \n", parametros);
			listar(clave);
			//Lista los procesos encolados esperando al recurso.
		}
		if (string_equals_ignore_case(comando, "kill")) {
			char* id = strtok(parametros, " ");
			IDaux = (int) strtol(id, (char**) NULL, 10);

			int claveIgual(struct_esi* esi) {
				return esi->ID == IDaux;
			}
			pthread_mutex_lock(&mutexKillEsi);
			pthread_mutex_lock(&mutex);

			Esiaux = list_remove_by_condition(listaReady, (void*) claveIgual);
			pthread_mutex_unlock(&mutex);

			if (Esiaux == NULL) {

				Esiaux = list_remove_by_condition(listaEjecutando,
						(void*) claveIgual);

			}

			if (Esiaux != NULL) {
				list_add(listaTerminados, Esiaux);

				ordenarFinalizar(Esiaux);
				log_info(logger, "Se elimino el Esi %s \n", parametros);
			} else {
				log_error(logger, "No se encontro el Esi");
			}
			pthread_mutex_unlock(&mutexKillEsi);

			//Finaliza el proceso. Al momento de eliminar el ESI, se debloquearan las claves que tenga tomadas.
		}
		if (string_equals_ignore_case(comando, "estado")) {

			log_info(logger,
					"La siguiente clave %s , esta en el siguiente estado: \n",
					parametros);
			//Conocer el estado de una clave y de probar la correcta distribución de las mismas
		}
		if (string_equals_ignore_case(comando, "deadlock")) {
			printf("Detección de deadlocks \n");

			/* para testear enviarle a la fc mostrarEsisEnDeadlock(simulacionBloqueados, simulacionTomadas);
			t_list* simulacionTomadas= list_create();

			struct_esi *ESI1 = calloc(1, sizeof(struct_esi));
						ESI1->estimacion = 5;
						ESI1->rafagaActual = 0;
						ESI1->socket = 1;
						ESI1->tiempoDeEspera = 0;
						ESI1->ID = 10;

			struct_esi *ESI2 = calloc(1, sizeof(struct_esi));
						ESI2->estimacion = 5;
						ESI2->rafagaActual = 0;
						ESI2->socket = 2;
						ESI2->tiempoDeEspera = 0;
						ESI2->ID = 20;

			struct_esi *ESI3 = calloc(1, sizeof(struct_esi));
						ESI3->estimacion = 5;
						ESI3->rafagaActual = 0;
						ESI3->socket = 2;
						ESI3->tiempoDeEspera = 0;
						ESI3->ID = 30;

			struct_esi *ESI4 = calloc(1, sizeof(struct_esi));
						ESI4->estimacion = 5;
						ESI4->rafagaActual = 0;
						ESI4->socket = 2;
						ESI4->tiempoDeEspera = 0;
						ESI4->ID = 50;

			struct_esiClaves *nuevoEsiTomador1 = calloc(1, sizeof(struct_esi));
					nuevoEsiTomador1->ESI = ESI1;
					nuevoEsiTomador1->clave= "diosa";

			struct_esiClaves *nuevoEsiTomador2 = calloc(1, sizeof(struct_esi));
					nuevoEsiTomador2->ESI = ESI2;
					nuevoEsiTomador2->clave= "mesi";

			struct_esiClaves *nuevoEsiTomador3 = calloc(1, sizeof(struct_esi));
					nuevoEsiTomador3->ESI = ESI3;
					nuevoEsiTomador3->clave= "karen";


			list_add(simulacionTomadas, nuevoEsiTomador1);
			list_add(simulacionTomadas, nuevoEsiTomador2);
			list_add(simulacionTomadas, nuevoEsiTomador3);

			t_list* simulacionBloqueados= list_create();


			struct_esiClaves *nuevoEsiBloqueado1= calloc(1, sizeof(struct_esi));
			nuevoEsiBloqueado1->ESI = ESI1;
			nuevoEsiBloqueado1->clave = "karen";

			struct_esiClaves *nuevoEsiBloqueado2= calloc(1, sizeof(struct_esi));
			nuevoEsiBloqueado2->ESI = ESI2;
			nuevoEsiBloqueado2->clave = "diosa";

			struct_esiClaves *nuevoEsiBloqueado3= calloc(1, sizeof(struct_esi));
				nuevoEsiBloqueado3->ESI = ESI3;
				nuevoEsiBloqueado3->clave = "mesi";

			struct_esiClaves *nuevoEsiBloqueado4= calloc(1, sizeof(struct_esi));
				nuevoEsiBloqueado4->ESI = ESI4;
				nuevoEsiBloqueado4->clave = "mesi";


			list_add(simulacionBloqueados, nuevoEsiBloqueado1);
			list_add(simulacionBloqueados, nuevoEsiBloqueado2);
			list_add(simulacionBloqueados, nuevoEsiBloqueado3);
			list_add(simulacionBloqueados, nuevoEsiBloqueado4);
*/

			mostrarEsisEnDeadlock(listaBloqueado, listaEsiClave);
		}
		printf("%s\n", linea);
		free(linea);
	}
	return 0;
}



//pthread_t tid;
//pthread_create(&tid, NULL, consola, NULL);
void prepararConfiguracion(int argc, char **argv){
	config=abrirArchivoConfig(argc,argv,logger,destruirLogger);
	//config = config_create("configPlanificador.config");
	planiConfig=  init_planificaorConfig();
	crearConfiguracion(planiConfig,config);
}

void destruirLogger(){ //Poner
	log_destroy(logger);
}

void inicializar(){
	EsisNuevos = 0;
	PlanificadorON = 1;
	IdDisponible =0;
}

void prepararLogger(){
	logger = crearLogger("loggerPlani.log", "loggerPlani");
}
void inicializarSemaforos(){
	sem_init(&pausarPlanificacion, 0, 1);
	sem_init(&cantidadEsisEnReady, 0, 0);
	sem_init(&huboDesalojoClaves, 0, 0);
}
