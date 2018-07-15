#include "mostrarListas.h"

void mostrarLista(t_list* lista){
	void mostrar(claveConInstancia * elem){
		printf ("clave, %s \n", elem->clave);
	}
	list_iterate(lista, (void *) mostrar);
}

void mostrarListaIntancias(t_list* listaDeInstancias){
	void mostrar(instancia * elem){
		printf ("sock instancia: %d entradas ocupadas: %d \n", elem->socket,  elem->cantEntradasOcupadas );
	}
	list_iterate(listaDeInstancias, (void *) mostrar);
}


void mostrarEntradasOcupadasActualmente(t_list* listaDeInstancias, t_log* logControlDeDistribucion){
	void mostrar(instancia * elem){
		log_info(logControlDeDistribucion, "sock instancia: %d entradas ocupadas: %d", elem->socket,  elem->cantEntradasOcupadas );
	}
	list_iterate(listaDeInstancias, (void *) mostrar);
}

