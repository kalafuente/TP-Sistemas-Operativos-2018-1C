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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"



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
void cerrarConexion();

//---PLANIFICADOR
void conectarseAlPlanificador();
void evaluarRespuestaDelCoordinador(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI resultado,
		t_instruccion*instruccion);
void recibirOrdenDelPlanificador(PROTOCOLO_PLANIFICADOR_A_ESI* orden,char*line);
void enviarResultadoAlPlanificador(PROTOCOLO_ESI_A_PLANIFICADOR respuesta);
//---COORDINADOR
void conectarseAlCoordinador();
void recibirResultadoDelCoordiandor(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI * resultado);
void enviarInstruccionAlCoordinador(t_instruccion* instruccion);
void avisarAlCoordi(PROTOCOLO_ESI_A_COORDI mensaje);


//---------SCRIPT
t_instruccion* leerInstruccion(char* line);
void procesarScript();
void abrirScript(int argc,char **argv);
void abortarEsi(const char* causa);

void cargarLogger();
//------------------VARIABLES GLOBALES
t_log* logger;
esi_config * esiConfig;
t_config *config;
FILE* script;
int socketCoordinador;
int socketPlani;




#endif /* ESI_H_ */
