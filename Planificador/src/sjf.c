#include "sjf.h"

bool tieneMenorRafaga(struct_esi* esi1, struct_esi* esi2) {
	return esi1->estimacion <= esi2->estimacion;
}

void ordenarPorSJF(t_list *listaAOrdenar) {
	if (sizeof(listaAOrdenar) <= 1) {
		return;
	}
	list_sort(listaAOrdenar, (void*) tieneMenorRafaga);
}

bool mayorResponseRatio(struct_esi* esi1, struct_esi* esi2) {
	return ((esi1->tiempoDeEspera + esi1->estimacion) / esi1->estimacion)
			>= ((esi2->tiempoDeEspera + esi2->estimacion) / esi2->estimacion);
}

void ordenarPorHRRN(t_list *listaAOrdenar) {
	if (sizeof(listaAOrdenar) <= 1) {
		return;
	}
	list_sort(listaAOrdenar, (void*) mayorResponseRatio);
}


void cambiarEstimacionSJF(struct_esi* esi, int alfa) {
	esi->estimacion = (esi->estimacion + esi->rafagaActual)
			* (1 - (alfa / 100.0)) + (alfa / 100.0) * esi->rafagaActual;
	esi->rafagaActual = 0;
}
