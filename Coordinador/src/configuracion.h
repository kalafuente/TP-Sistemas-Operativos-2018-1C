

#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_
#include "coordinador.h"

coordinador_config * init_coordConfig();
void destroy_coordConfig(coordinador_config* coord);
void crearConfiguracion(coordinador_config* coordinador, t_config* config);

#endif /* CONFIGURACION_H_ */
