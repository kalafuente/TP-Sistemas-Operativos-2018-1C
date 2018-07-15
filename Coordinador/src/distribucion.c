#include "distribucion.h"

instancia * EquitativeLoad(t_list* listaDeInstancias) {


	instancia * aux = list_remove(listaDeInstancias,0);
	list_add(listaDeInstancias, aux);
	return aux;

}

instancia * LSU(t_list* listaDeInstancias, t_log* logControlDeDistribucion) {

	log_info(logControlDeDistribucion, "LSU");
	mostrarEntradasOcupadasActualmente(listaDeInstancias,logControlDeDistribucion);

	int size = list_size(listaDeInstancias);
	int i = 1;


	instancia * aux = list_remove(listaDeInstancias,0);
	instancia * instanciaConMenosEntradasUtilizadas = aux;

	int minimo= aux->cantEntradasOcupadas;

	list_add(listaDeInstancias, aux);

	while (i<size){

		aux = list_remove(listaDeInstancias,0);
		if (aux->cantEntradasOcupadas < minimo  ){
			minimo = aux->cantEntradasOcupadas;
			instanciaConMenosEntradasUtilizadas = aux;
		}
		list_add(listaDeInstancias, aux);

		i++;
	}
	return instanciaConMenosEntradasUtilizadas;
}



