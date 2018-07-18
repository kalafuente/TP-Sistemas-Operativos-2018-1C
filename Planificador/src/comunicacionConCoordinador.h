#ifndef COMUNICACIONCONCOORDINADOR_H_
#define COMUNICACIONCONCOORDINADOR_H_
#include "Planificador.h"

int conectarseAlCoordinador(planificador_config * planificadorConfig);
void * manejarConexionCoordi(void * socket);

#endif /* COMUNICACIONCONCOORDINADOR_H_ */
