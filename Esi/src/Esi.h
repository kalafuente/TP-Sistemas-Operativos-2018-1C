/*
 * Esi.h
 *
 *  Created on: 2 may. 2018
 *      Author: utnso
 */
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <library/manejoDeSockets.h>
#include <library/protocolos.h>
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


//---CONFIGURACION
void crearConfiguracion();
void destroy_esiConfig();
void init_esiConfig();
void killEsi();


//---CONEXIÓNES
void conectarseAlCoordinador();
void conectarseAlPlanificador();
void enviarResultado(PROTOCOLO_ESI_A_PLANIFICADOR);
void cerrarConexion();

//---------SCRIPT
t_instruccion* leerInstruccion(char* line);
void procesarScript();
void abrirScript(int argc,char **argv);

t_instruccion* cargarInstruccion(PROTOCOLO_INSTRUCCIONES protocolo,char*clave, char* valor);
void enviarInstruccionAlCoordinador(t_instruccion* instruccion);
//------------------VARIABLES GLOBALES
t_log* logger;
esi_config * esiConfig;
t_config *config;
FILE* script;
int socketCoordinador;
int socketPlani;

#endif /* ESI_H_ */
