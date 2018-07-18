#ifndef COMUNICACIONCONESIS_H_
#define COMUNICACIONCONESIS_H_
#include "Planificador.h"

void ordenarActuar(struct_esi* esi);
PROTOCOLO_ESI_A_PLANIFICADOR recibirResultado(struct_esi* esi);
void * recibirEsi(void* socketEscucha);

#endif /* COMUNICACIONCONESIS_H_ */
