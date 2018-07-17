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




void abortarEsi(const char * causa){
	if(script!=NULL){
		fclose(script);
	}
	log_error(logger,"%s ESI ABORTADO",causa);
	cerrarConexion();
	killEsi();
	exit(1);
}
