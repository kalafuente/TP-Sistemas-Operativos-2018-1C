#include "planificacionDeEsis.h"
void planificarESIs(){

	struct_esi *esiActual;
	t_instruccion* instruccion = calloc(1, sizeof(t_instruccion));
	PROTOCOLO_ESI_A_PLANIFICADOR estadoEsi;


	while (PlanificadorON) {
		pthread_mutex_unlock(&mutexKillEsi);
		sem_wait(&cantidadEsisEnReady);
		pthread_mutex_lock(&mutexKillEsi);

		estadoEsi = TERMINE_BIEN;
		pthread_mutex_lock(&mutex);
		if (EsisNuevos || planiConfig->algoritmoPlanificacion == HRRN) {

			EsisNuevos = 0;
			ordenarListaDeReady();
		}
		pthread_mutex_unlock(&mutex);

		pthread_mutex_unlock(&mutexKillEsi);
		sem_wait(&pausarPlanificacion);
		sem_post(&pausarPlanificacion);
		pthread_mutex_lock(&mutexKillEsi);

		if (list_size(listaReady) == 0) {
			continue;
		}

		pthread_mutex_lock(&mutex);

		esiActual = list_remove(listaReady, 0);
		list_add(listaEjecutando, esiActual);
		pthread_mutex_unlock(&mutex);

		while (estadoEsi == TERMINE_BIEN) {
			sem_wait(&pausarPlanificacion);
			ordenarActuar(esiActual);
			if (recibirMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR),&estadoEsi, esiActual->socket) <= 0) {
				log_error(logger, "ERROR ESI DESCONECTADO");
				estadoEsi = ERROR;
			}
			log_info(logger, "se recibio estado %d", estadoEsi);
				//estadoEsi=recibirResultado(esiActual);

			pthread_mutex_lock(&mutex);

			sumarEspera();

			pthread_mutex_unlock(&mutex);

			switch (estadoEsi) {
				case TERMINE_BIEN:
					esiActual->estimacion--;
					esiActual->rafagaActual++;
					instruccion = recibirInstruccion(logger, esiActual->socket, "ESI");
					procesarInstruccion(instruccion, esiActual);
					destruirInstruccion(instruccion);
					break;
				case BLOQUEADO_CON_CLAVE:
					esiActual->estimacion--;
					esiActual->rafagaActual++;
					instruccion = recibirInstruccion(logger, esiActual->socket, "ESI");
					cambiarEstimacionSJF(esiActual,planiConfig->alfaPlanificacion);
					pthread_mutex_lock(&mutex);
					list_remove(listaEjecutando, 0);
					agregarEnListaBloqueado(esiActual, instruccion->clave);
					pthread_mutex_unlock(&mutex);
					destruirInstruccion(instruccion);
					log_info(logger, "esi %d bloqueado", esiActual->ID);

					break;
				case TERMINE:
					pthread_mutex_lock(&mutex);
					list_remove(listaEjecutando, 0);
					list_add(listaTerminados, esiActual);
					pthread_mutex_unlock(&mutex);
				liberarTodasLasClavesDeEsi(esiActual);
					log_info(logger, "termino el esi %d", esiActual->ID);
					close(esiActual->socket);
					break;
				case ERROR:
					pthread_mutex_lock(&mutex);
					list_remove(listaEjecutando, 0);
					list_add(listaTerminados, esiActual);
				pthread_mutex_unlock(&mutex);
				//liberar claves(?
					log_error(logger, "error con el esi %d", esiActual->ID);
					close(esiActual->socket);
					break;
				default:
					list_remove(listaEjecutando, 0);
					log_error(logger, "No deberias ver esto");
					break;
					}
		//IF (es con desalojo && llego un nuevo esi)->devolver a la listaReady && Salir del while------------------------------------------------------------------------------------------------------------
			if (planiConfig->algoritmoPlanificacion == SJF_CD
					&& estadoEsi == TERMINE_BIEN && EsisNuevos) {
				estadoEsi = TERMINE;
				list_remove(listaEjecutando, 0);
				list_add(listaReady, esiActual);
				sem_post(&cantidadEsisEnReady);
				sem_post(&pausarPlanificacion);
				break;
			}

			sem_post(&pausarPlanificacion);
			pthread_mutex_unlock(&mutexKillEsi);
			pthread_mutex_lock(&mutexKillEsi);

			if (list_size(listaEjecutando) == 0) {
				estadoEsi = TERMINE;
			}

		}
	//Calcular estimacion para la proxima vez.
	}
	free(instruccion);
}

void ordenarListaDeReady(){
	switch (planiConfig->algoritmoPlanificacion) {
		case SJF_CD:
			ordenarPorSJF(listaReady);
			break;

		case SJF_SD:
			ordenarPorSJF(listaReady);
			break;

		case HRRN:
		ordenarPorHRRN(listaReady);
			break;
		default:
			log_error(logger, "ERROR INTERNO, REVISAR EL CODIGO");
			break;
	}

}


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
		sem_post(&huboDesalojoClaves);
		//liberar la correspondiente
		break;
	default:
		log_error(logger, "!!!!! ERRRORRRR  !!!!  INTRUCCION INVALIDA");
	}
}
