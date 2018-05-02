
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
#include <pthread.h>
#include <manejoDeSockets/enviarYRecibirMensaje.c>
#include <manejoDeSockets/conectarseAlServidor.c>


typedef struct esi_config{
	char *ipCoordi ;
	char *puertoCoordi;
	char *idPlanificador;
	char *puertoPlanificador;
}esi_config;

/*void crearConfiguracion(esiConfig** esiConfig, t_config ** config);
void destroy_esiConfig(esiConfig * esi);
esiConfig* init_esiConfig();
*/


void crearConfiguracion(char ** ipCoordi, char ** puertoCoordi,
		char ** idPlanificador, char ** puertoPlanificador, t_config ** config);


void *conexionPlanificador(void *socketplanificador);




#endif /* ESI_H_ */
