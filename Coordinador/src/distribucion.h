#ifndef DISTRIBUCION_H_
#define DISTRIBUCION_H_

#include "coordinador.h"


instancia * LSU(t_list* listaDeInstancias, t_log* logControlDeDistribucion);
instancia * EquitativeLoad(t_list* listaDeInstancias);
instancia * KeyExplicit(t_list* listaDeInstancias, t_log* logControlDeDistribucion, char * clave, t_list* letrasDeLaInstancia, t_list* alfabeto);
instanciaYSusCaracteres * instanciaQueTieneLaLetra(t_list* list, void* inicial);
void obtenerInicial(char * clave, char inicial [1]);


#endif /* DISTRIBUCION_H_ */
