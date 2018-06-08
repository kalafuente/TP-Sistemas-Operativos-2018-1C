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



void enviarInstruccionAlCoordinador(t_instruccion* instruccion){
	//PROTOCOLO_ESI_A_PLANIFICADOR resultado = TERMINE_BIEN;
	log_info(logger,"Enviando instruccion al Coordinador");
	enviarInstruccion(logger,instruccion,socketCoordinador);
	log_info(logger,"Se envió la instrucción");
	//enviarMensaje(logger, sizeof(PROTOCOLO_ESI_A_PLANIFICADOR), &resultado,
	//					socketPlani);

}

void recibirOrdenDelPlanificador(PROTOCOLO_PLANIFICADOR_A_ESI* orden){
	log_info(logger,"Esperando la orden del planificador");
	recibirMensaje(logger, sizeof(PROTOCOLO_PLANIFICADOR_A_ESI),
					&orden, socketPlani);
	log_info(logger,"Orden recibida");
}


void evaluarRespuestaDelCoordinador(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI resultado,t_instruccion*instruccion){
	PROTOCOLO_ESI_A_PLANIFICADOR estado;
	if(resultado==TODO_OK_ESI){
	estado=TERMINE_BIEN;
	enviarResultadoAlPlanificador(estado);
	enviarInstruccion(logger,instruccion,socketPlani);
	}
	else if(resultado==BLOQUEATE){
		estado=BLOQUEADO_CON_CLAVE;
		enviarResultadoAlPlanificador(estado);
		enviarInstruccion(logger,instruccion,socketPlani);
	}
	else{
		estado=ERROR;
		if(resultado==ERROR_CLAVE_INACCESIBLE){

		log_error(logger,"Clave %s inaccesible",instruccion->clave);

		}
		else if(resultado==ERROR_CLAVE_NO_BLOQUEADA){
			log_error(logger,"Clave %s no bloquead",instruccion->clave);

		}
		else{
			log_error(logger,"Clave %s no identificada",instruccion->clave);
		}

		enviarResultadoAlPlanificador(estado);
		destruirInstruccion(instruccion);
		abortarEsi();
	}
}

void recibirResultadoDelCoordiandor(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI * resultado){
	log_info(logger,"Esperando resultado de la operación");
	recibirMensaje(logger,sizeof(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI),resultado,socketCoordinador);
	log_info(logger,"Resultado recibido");
}

void enviarResultadoAlPlanificador(PROTOCOLO_ESI_A_PLANIFICADOR resultado){
	log_info(logger,"Enviando resultado de la operacion al Planificador ");
	enviarMensaje(logger,sizeof(resultado),&resultado,socketCoordinador);
	log_info(logger,"Resultado enviado");
}


void abortarEsi(){
	if(script!=NULL){
		fclose(script);
	}
	log_error(logger,"ESI ABORTADO");
	cerrarConexion();
	killEsi();
	exit(1);
}
