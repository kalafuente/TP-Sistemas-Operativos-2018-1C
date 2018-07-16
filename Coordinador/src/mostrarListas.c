#include "mostrarListas.h"

void mostrarListaDeClaves(t_list* lista){
	printf ("\n La lista de claves es\n ");

	if (list_size(lista)==0){
			printf("No hay más claves \n");
		}
	else {
		void mostrar(claveConInstancia * elem){
				printf ("clave: %s, instancia: aqui error \n", elem->clave);
			}
			list_iterate(lista, (void *) mostrar);
	}

}

void mostrarListaIntancias(t_list* listaDeInstancias){
	if (list_size(listaDeInstancias)==0){
		printf("No hay más instancias\n");
	}
	else{
		void mostrar(instancia * elem){
				printf ("sock instancia: %d entradas ocupadas: %d \n", elem->socket,  elem->cantEntradasOcupadas );
			}
			list_iterate(listaDeInstancias, (void *) mostrar);
	}
}



void mostrarEntradasOcupadasActualmente(t_list* listaDeInstancias, t_log* logControlDeDistribucion){
	void mostrar(instancia * elem){
		log_info(logControlDeDistribucion, "sock instancia: %d entradas ocupadas: %d", elem->socket,  elem->cantEntradasOcupadas );
	}
	list_iterate(listaDeInstancias, (void *) mostrar);
}

