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

typedef struct instruccion{
	PROTOCOLO_INSTRUCCIONES instruccion;
	char*clave;
	char*valor;
}instruccion;


//---CONFIGURACION
void crearConfiguracion();
void destroy_esiConfig();
void init_esiConfig();

//---CONEXIÓNES
void conectarseAlCoordinador();
void conectarseAlPlanificador();
void abrirScript(int argc,char **argv);
void enviarResultado(PROTOCOLO_ESI_A_PLANIFICADOR);
void enviarInstruccion(t_esi_operacion);
void procesarScript();
void cerrarConexion();
void killEsi();

instruccion* leerInstruccion(char* line);
void enviarInstruccion2(instruccion *instruccion);
instruccion* cargarInstruccion(PROTOCOLO_INSTRUCCIONES protocolo,char*clave, char* valor);

//------------------VARIABLES GLOBALES
t_log* logger;
esi_config * esiConfig;
t_config *config;
FILE* script;
int socketCoordinador;
int socketPlani;

#endif /* ESI_H_ */
