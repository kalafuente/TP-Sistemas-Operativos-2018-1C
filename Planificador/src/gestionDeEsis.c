#include "gestionDeEsis.h"

void agregarEsi(int socketCliente) {
	struct_esi *nuevoEsi = calloc(1, sizeof(struct_esi));
	nuevoEsi->estimacion = planiConfig->estimacionInicial;
	nuevoEsi->rafagaActual = 0;
	nuevoEsi->socket = socketCliente;
	nuevoEsi->tiempoDeEspera = 0;
	nuevoEsi->ID = IdDisponible;
	IdDisponible++;
	list_add(listaReady, nuevoEsi);
	EsisNuevos = 1;
	sem_post(&cantidadEsisEnReady);
}

struct_esiClaves* crearEsiClave(struct_esi* esi, char*clave) {
	struct_esiClaves* aux = calloc(1, sizeof(struct_esiClaves));
	aux->ESI = esi;
	aux->clave = string_new();
	string_append(&aux->clave, clave);
	return aux;
}

void agregarEnListaBloqueado(struct_esi *esiActual, char*clave) {
	struct_esiClaves *elemento = calloc(1, sizeof(struct_esiClaves));
	elemento->clave = string_new();
	string_append(&elemento->clave, clave);
	elemento->ESI = esiActual;
	list_add(listaBloqueado, elemento);
}

void liberarEsi(char*clave) {
	struct_esiClaves* aux;
	int esSuClaveIgual(struct_esiClaves*elesi) {
		return string_equals_ignore_case(clave, elesi->clave);
	}

	aux = (struct_esiClaves*) list_remove_by_condition(listaBloqueado,
			(void*) esSuClaveIgual);
	if (aux != NULL) {
		list_add(listaReady, aux->ESI);
		EsisNuevos = 1;
		sem_post(&cantidadEsisEnReady);
		sem_post(&huboDesalojoClaves);

	}
}

void liberarTodasLasClavesDeEsi(struct_esi*esi) {
	

	int tienenIgualID(struct_esiClaves*structEsi) {
		return (structEsi->ESI->ID == esi->ID);
	}
	struct_esiClaves* elementoAux = list_remove_by_condition(listaEsiClave,
			(void*) tienenIgualID);
	while (elementoAux != NULL) {

		liberarEsi(elementoAux->clave);
		destruirStructEsiClaveSinEsi(elementoAux);
		elementoAux = list_remove_by_condition(listaEsiClave,
				(void*) tienenIgualID);
		}


}



void sacarStructDeListaEsiClave(char*clave) {
	int esSuClaveIgual(struct_esiClaves*elesi) {
		return string_equals_ignore_case(clave, elesi->clave);
	}

	list_remove_by_condition(listaEsiClave, (void*) esSuClaveIgual);
}
void sumarUnoDeEspera(struct_esi* esi) {
	esi->tiempoDeEspera++;
}

void sumarEspera() {
	list_iterate(listaReady, (void*) sumarUnoDeEspera);
}


