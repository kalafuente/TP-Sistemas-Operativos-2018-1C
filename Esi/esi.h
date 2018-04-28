
#ifndef ESI_H_
#define ESI_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>

int conectarseAlServidor(char ** ip, char ** puerto);
void crearConfiguracion(char ** ipCoordi, char ** puertoCoordi,
		char ** idPlanificador, char ** puertoPlanificador, t_config ** config);

void crearConfiguracion(char ** ipCoordi, char ** puertoCoordi,
		char ** idPlanificador, char ** puertoPlanificador, t_config ** config) {

	*config = config_create(
			"configuracion.config");
	*ipCoordi = config_get_string_value(*config, "IP_COORDINADOR");
	*puertoCoordi = config_get_string_value(*config, "PUERTO_COORDINADOR");
	*idPlanificador = config_get_string_value(*config, "IP_PLANIFICADOR");
	*puertoPlanificador = config_get_string_value(*config,
			"PUERDO_PLANIFICADOR");

}
int recibirmensaje(int unsocket);
int enviarmensaje(char*mensaje, int unsocket);


typedef struct {
	int id;
	int len;
}__attribute__((packed)) ContentHeader;




#endif /* ESI_H_ */
