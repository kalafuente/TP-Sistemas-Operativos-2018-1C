#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_
#include "Planificador.h"

planificador_config * init_planificadorConfig();
void crearConfiguracion(planificador_config* planificador, t_config* config);
void destroy_planificadorConfig(planificador_config* planificador_config);
ALGORITMO_PLANIFICACION traducir(char* algoritmo);
#endif /* CONFIGURACION_H_ */
