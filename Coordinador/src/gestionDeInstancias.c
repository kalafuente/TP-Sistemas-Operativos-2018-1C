#include "gestionDeInstancias.h"

void registrarInstancia(int sock, char * id){
	instancia * registrarInstancia = malloc(sizeof(instancia ));
	registrarInstancia->socket=sock;
	printf("\n socket de esta instancia: %d", sock);
	registrarInstancia->identificador = id;
	printf("\n id de esta instancia: %s", registrarInstancia->identificador);
	registrarInstancia->cantEntradasTotales = coordConfig->entradas;
	registrarInstancia->tamanioEntradas= coordConfig->tamanioEntradas;
	registrarInstancia->cantEntradasOcupadas=0;
	list_add(listaDeInstancias,registrarInstancia);

	printf("\n tamaño listaDeInst: %d \n:", list_size(listaDeInstancias));
	log_info(logger,"\n Se registro instancia");
	printf("instancias registradas: %d \n", list_size(listaDeInstancias));

}

void modificarInstanciaListaDeClavesConInstancia(char* clave, instancia* instanciaNueva, t_list* listaDeClavesConInstancia){
	claveConInstancia* elemento =  instanciaQueTieneLaClave(clave, listaDeClavesConInstancia);
	elemento->instancia = instanciaNueva;

}

instancia*  elegirInstanciaSegunAlgoritmo(char * clave, t_log* logger, t_log* logControlDeDistribucion, t_list* letrasDeLaInstancia){


	if (strcmp(coordConfig->algoritmo, "EL") == 0){
		log_info(logger, "ALGORITMO EQUITATIVE LOAD");
		instancia* instanciaElegida =  EquitativeLoad(listaDeInstancias);
		log_info(logControlDeDistribucion, "el socket de la instancia elegida es %d",instanciaElegida->socket);
		return instanciaElegida;
	}
	else
		{
		if (strcmp(coordConfig->algoritmo, "LSU")==0){
		log_info(logger, "ALGORITMO LSU-----------------------------------");
		mostrarListaIntancias(listaDeInstancias);
		instancia* instanciaElegida =  LSU(listaDeInstancias, logControlDeDistribucion);
		log_info(logControlDeDistribucion, "el socket de la instancia elegida es %d",instanciaElegida->socket);
		return instanciaElegida;
		}
		else {
			if (strcmp(coordConfig->algoritmo, "KE")==0){
				log_info(logger, "ALGORITMO KE-----------------------------------");
				letrasDeLaInstancia = list_create();
				alfabeto = crearAlfabeto();
				mostrarListaIntancias(listaDeInstancias);
				instancia* instanciaElegida =  KeyExplicit(listaDeInstancias, logControlDeDistribucion,clave,letrasDeLaInstancia, alfabeto);
				log_info(logControlDeDistribucion, "el socket de la instancia elegida es %d",instanciaElegida->socket);
				destruirLetrasDeLaInstancia(letrasDeLaInstancia);
				destruirAlfabeto(alfabeto);
				return instanciaElegida;
			}
			else{
				log_info(logger,"algoritmo desconocido");
			}
		}
		}
	;
	return NULL; //ESTO NO DEBERIA PASAR

}

claveConInstancia* instanciaQueTieneLaClave(char* clave, t_list* listaDeClavesConInstancia){
	bool condicionDeClave(claveConInstancia* item) {
			int rta = strcmp(clave, item->clave);
			if (rta == 0)
					return true;
			else
					return false;
		}

	return list_find(listaDeClavesConInstancia,(void *) condicionDeClave);
}


claveConInstancia* nuevaClaveConInstancia(char* clave){
	claveConInstancia* nueva=malloc(sizeof(claveConInstancia));
	nueva-> clave = string_new();
	string_append(&(nueva->clave), clave);
	nueva->instancia = NULL;
	return nueva;
}

void registrarEntradasOcupadasDeLaInstancia(int entradasOcupadas, instancia * instancia){

	instancia->cantEntradasOcupadas = entradasOcupadas;

}

void eliminarClave(int socket, t_list* listaDeClavesConInstancia ){

	bool equals(claveConInstancia* item) {

			if (item->instancia->socket == socket)
					return true;
			else
					return false;
		}

	list_remove_and_destroy_by_condition(listaDeClavesConInstancia,(void *) equals, (void *)destruirClaveConInstancia );
	printf("la nueva lista de claves es: \n");
	mostrarListaDeClaves(listaDeClavesConInstancia);

	}

void eliminarInstancia(int socket, t_list* listaDeInstancias){
	bool equals(instancia* item) {

				if (item->socket == socket)
						return true;
				else
						return false;
			}
	list_remove_and_destroy_by_condition(listaDeInstancias,(void *) equals, (void *)destruirInstancia );
	printf("la nueva lista de instancias es: \n");
	mostrarListaIntancias(listaDeInstancias);
}




void destruirInstancia(instancia* instancia){
	free(instancia);
}

void destruirClaveConInstancia(claveConInstancia* claveConInstancia){
	free(claveConInstancia->clave);
	free(claveConInstancia);
}

void destruirLetrasDeLaInstancia(t_list* letrasDeLaInstancia){
	list_destroy_and_destroy_elements(letrasDeLaInstancia, (void *)destruirListaDeLetras);
}

void destruirListaDeLetras(instanciaYSusCaracteres* elementoLista){
	list_destroy_and_destroy_elements(elementoLista->caracteres, (void *)destruirChar);
}

void destruirChar(char* letra){
	free(letra);
}



