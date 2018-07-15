#ifndef DISTRIBUCION_H_
#define DISTRIBUCION_H_

#include "coordinador.h"


typedef struct instanciaYSusCaracteres {
	instancia * instancia;
	t_list* caracteres;
}instanciaYSusCaracteres;

t_list* alfabeto;
t_list* letrasDeLaInstancia;
instancia * LSU(t_list* listaDeInstancias, t_log* logControlDeDistribucion);
instancia * EquitativeLoad(t_list* listaDeInstancias);
instancia * KeyExplicit(t_list* listaDeInstancias, t_log* logControlDeDistribucion, char * clave);
instanciaYSusCaracteres * instanciaQueTieneLaLetra(t_list* list, void* inicial);
void obtenerInicial(char * clave, char inicial [1]);
void agregarAlfabeto();
void destruirAlfabeto();
#endif /* DISTRIBUCION_H_ */
