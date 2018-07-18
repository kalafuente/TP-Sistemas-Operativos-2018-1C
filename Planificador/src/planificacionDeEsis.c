#include "planificacionDeEsis.h"
void planificarESIs(){

	struct_esi *esiActual;
	t_instruccion* instruccion;
	PROTOCOLO_ESI_A_PLANIFICADOR estadoEsi;


	while (PlanificadorON) {
		sem_wait(&cantidadEsisEnReady);
		estadoEsi = TERMINE_BIEN;
		pthread_mutex_lock(&mutex);
		ordenarListaDeReady();
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
					//liberar claves
					log_info(logger, "termino el esi %d", esiActual->ID);
					close(esiActual->socket);
					break;
				case ERROR:
					pthread_mutex_lock(&mutex);
					list_remove(listaEjecutando, 0);
					list_add(listaTerminados, esiActual);
					pthread_mutex_lock(&mutex);
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
			//ordenarPorHRRN(listaReady);
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
		//liberar la correspondiente
		break;
	}
}
