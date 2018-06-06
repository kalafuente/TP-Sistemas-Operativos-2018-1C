#include <stdio.h>
#include <stdlib.h>
#include "Esi.h"
#include <parsi/parser.h>
#include <signal.h>
int main(int argc, char **argv) {

	abrirScript(argc, argv);
	logger = crearLogger("loggerEsi.log", "loggerEsi");

	//-------ARCHIVO DE CONFIGURACION

	crearConfiguracion();

	//-------ARCHIVO DE CONFIGURACION

	conectarseAlCoordinador();
	conectarseAlPlanificador();
	procesarScript();

	cerrarConexion();
	killEsi();
	return 0;

}




void destruirInstruccion(instruccion*instruccion){
	free(instruccion->clave);
	free(instruccion->valor);
	free(instruccion);
}


void enviarInstruccion2(instruccion*instruccion) {
	PROTOCOLO_ESI_A_PLANIFICADOR resultado = TERMINE_BIEN;
	int32_t lenClave = strlen(instruccion->clave)+1;
	int32_t lenValor= strlen(instruccion->valor)+1;
	enviarMensaje(logger,sizeof(PROTOCOLO_INSTRUCCIONES),&instruccion->instruccion,socketCoordinador);
	enviarMensaje(logger,sizeof(int32_t),&lenClave,socketCoordinador);
	enviarMensaje(logger,lenClave,instruccion->clave,socketCoordinador);

	enviarMensaje(logger,sizeof(int32_t),&lenValor,socketCoordinador);
	enviarMensaje(logger,lenValor,instruccion->valor,socketCoordinador);

	enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
					socketPlani);


}




