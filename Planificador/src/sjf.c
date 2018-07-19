#include "sjf.h"

bool tieneMenorRafaga(struct_esi* esi1, struct_esi* esi2) {
	return esi1->estimacion < esi2->estimacion;
}

void ordenarPorSJF(t_list *listaAOrdenar) {
	if (sizeof(listaAOrdenar) == 1) {
		return;
	}
	list_sort(listaAOrdenar, (void*) tieneMenorRafaga);
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



void cambiarEstimacionSJF(struct_esi* esi, int alfa) {
	esi->estimacion = (esi->estimacion + esi->rafagaActual) * (1 - (alfa / 100))
			+ (alfa / 100) * esi->rafagaActual;
}