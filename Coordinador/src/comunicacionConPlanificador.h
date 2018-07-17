#ifndef COMUNICACIONCONPLANIFICADOR_H_
#define COMUNICACIONCONPLANIFICADOR_H_
#include "coordinador.h"

PROTOCOLO_PLANIFICADOR_A_COORDINADOR  estadoEsi(t_log* logger,PROTOCOLO_COORDINADOR_A_PLANIFICADOR estadoClave, int socketPlani, t_instruccion* instruccion);


#endif /* COMUNICACIONCONPLANIFICADOR_H_ */
