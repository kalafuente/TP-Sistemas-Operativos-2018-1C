#ifndef ALGORITMOSPLANIFICACION_H_
#define ALGORITMOSPLANIFICACION_H_
#include "Planificador.h"

bool tieneMenorRafaga(struct_esi* esi1, struct_esi* esi2);
void ordenarPorSJF(t_list *listaAOrdenar);
bool mayorResponseRatio(struct_esi* esi1, struct_esi* esi2);
void ordenarPorHRRN(t_list *listaAOrdenar);
float actualizarDuracionDeRafagaSJF(struct_esi esi);
double calcularSiguienteRafagaSJF(int t , int t0, double alfa);
double calcularRafagaSJF(struct_esi* esi, double alfa);
void actualizarEstimacionSJF(struct_esi* esi, double alfa);
void cambiarEstimacionSJF(struct_esi* esi, int alfa);

#endif /* ALGORITMOSPLANIFICACION_H_ */
