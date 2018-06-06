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



void enviarInstruccionAlCoordinador(instruccion* instruccion){
	PROTOCOLO_ESI_A_PLANIFICADOR resultado = TERMINE_BIEN;
	enviarInstruccion(logger,instruccion,socketCoordinador);
	enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
						socketPlani);
}



