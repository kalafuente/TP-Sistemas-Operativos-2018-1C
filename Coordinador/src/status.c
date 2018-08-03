#include "status.h"

void status (int sock){
	char * claveAPedir, * valor;
	claveAPedir = recibirID(sock,logger);

	while(claveAPedir!=NULL){
		pthread_mutex_lock(&mutexCompactacion);
		printf("Se requiere status de la clave: %s \n", claveAPedir);
		if (contieneClave(listaDeClavesConInstancia,claveAPedir)){
		claveConInstancia * instanciaALlamar = instanciaQueTieneLaClave(claveAPedir,listaDeClavesConInstancia);
			if (instanciaALlamar->instancia == NULL){
				enviarID(sock,"no hay valor, pero hay get",logger);
				instancia * instanciaElegida = simulacionElegirInstanciaSegunAlgoritmo(claveAPedir,letrasDeLaInstancia);
				enviarID(sock,instanciaElegida->identificador,logger);
			}
			else{
				t_instruccion * falsa = malloc (sizeof(t_instruccion));
				falsa->instruccion = PEDIDO_DE_VALOR;
				falsa->clave = "null";
				falsa->valor = "null";
				PROTOCOLO_INSTANCIA_A_COORDINADOR rta;


				if (enviarInstruccion(logger,falsa,instanciaALlamar->instancia->socket)==-1){
					enviarID(sock,"no hay valor, se cayó la instancia",logger);
					enviarID(sock,instanciaALlamar->instancia->identificador,logger);
				}
				else {
					if (enviarID(instanciaALlamar->instancia->socket,claveAPedir,logger)==-1){
						enviarID(sock,"no hay valor, se cayó la instancia",logger);
						enviarID(sock,instanciaALlamar->instancia->identificador,logger);
					}
					else{
						valor = recibirID(instanciaALlamar->instancia->socket,logger);
						if (strcmp(valor, "null")!=0){
							enviarID(sock,valor,logger);
							log_info(logger,"Envié valor: %s \n", valor);
							free(valor);
						}
						else
							enviarID(sock,"no hay valor",logger);
							enviarID(sock,instanciaALlamar->instancia->identificador,logger);
					}
				}
				recibirMensaje(logger,sizeof(rta),&rta, instanciaALlamar->instancia->socket);
				int32_t entradasEnUsoDeLaInstancia;
				recibirMensaje(logger,sizeof(entradasEnUsoDeLaInstancia),&entradasEnUsoDeLaInstancia, instanciaALlamar->instancia->socket);
				free(falsa);


			}

		}
		else
			enviarID(sock,"ClaveInexistente",logger);

		pthread_mutex_unlock(&mutexCompactacion);
		claveAPedir = recibirID(sock,logger);

		if (claveAPedir == NULL){
			break;
		}

	}
	banderaTerminarHilos++;
	free(claveAPedir);
	//printf("salgo de estatus pues band: %d \n", banderaTerminarHilos);
	return;
	/*
	if (banderaTerminarHilos !=0){
		log_info(logger, "Planificador desconectado");
		killCoordinador();
		exit(1);
	}*/

}
