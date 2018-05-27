/*
 * Esi.h
 *
 *  Created on: 2 may. 2018
 *      Author: utnso
 */
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <manejoDeSockets/manejoDeSockets.h>
#include <protocolos/protocolos.h>
#include <string.h>
#include <parsi/parser.h>

#ifndef ESI_H_
#define ESI_H_


typedef struct esi_config{
	char *ipCoordi ;
	char* puertoCoordi;
	char *ipPlanificador;
	char* puertoPlanificador;
}esi_config;


void crearConfiguracion(esi_config* esiConfig, t_config* config);
void destroy_esiConfig();
esi_config * init_esiConfig();
void conectarseAlCoordinador();
void conectarseAlPlanificador();
void abrirScript(char **argv);
void enviarResultado(PROTOCOLO_ESI_A_PLANIFICADOR);
void enviarInstruccion(t_esi_operacion);
void procesarScript();

//------------------VARIABLES GLOBALES
t_log* logger;
esi_config * esiConfig;
t_config *config;
FILE* script;
int socketCoordinador;
int socketPlani;

#endif /* ESI_H_ */
