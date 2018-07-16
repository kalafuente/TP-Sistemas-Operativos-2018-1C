#include "distribucion.h"
#include <math.h>

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
void mostrar(t_list* lista){
	void mostrar(char * elem){
		printf ("%s", elem);
	}
	list_iterate(lista, (void *) mostrar);
}

instancia * KeyExplicit(t_list* listaDeInstancias, t_log* logControlDeDistribucion, char * clave, t_list* letrasDeLaInstancia){
	log_info(logControlDeDistribucion, "KE");
	agregarAlfabeto();

	int bloques = (int) ceil ((double) list_size(alfabeto)/ (double) list_size(listaDeInstancias));

	for (int i=0; i <list_size(listaDeInstancias); i++){

			instanciaYSusCaracteres * elementoNuevo = malloc (sizeof(instanciaYSusCaracteres));
			elementoNuevo->instancia = list_get(listaDeInstancias,i);


			if (list_size(alfabeto)>= bloques)
				elementoNuevo->caracteres = list_take_and_remove(alfabeto,bloques);
			else
				elementoNuevo->caracteres = list_take_and_remove(alfabeto,list_size(alfabeto));


			printf("el socket de esta instancia es:::: %d\n",elementoNuevo->instancia->socket);
			printf("el bloque de esta instancia es:::: \n");
			mostrar(elementoNuevo->caracteres);

			list_add(letrasDeLaInstancia, elementoNuevo);
	}

	char inicial [1];
	obtenerInicial(clave, inicial);
	printf ("la inicial es %s\n", inicial);
	return (instanciaQueTieneLaLetra(letrasDeLaInstancia, inicial))->instancia;
	destruirAlfabeto();

}

//------------------FUNCIONES AUXILIARES------------------

void obtenerInicial(char * clave, char inicial [1]){
	inicial[0] = clave[0];
}

void agregarAlfabeto(){
	char l [1];
	alfabeto = list_create();
	for(int i =97; i<= 122; i++){
		l[0] = i;
		char * letra = string_new();
		string_append(&letra, &l[0]);
		list_add(alfabeto, letra);
	}

}

void destruirAlfabeto(){
	for(int i =0; i<= 25; i++){
		char* elemento = list_get(alfabeto,i);
		free (elemento);
	}
	list_destroy(alfabeto);
}



instanciaYSusCaracteres * instanciaQueTieneLaLetra(t_list* list, void* inicial){

	bool sublistaTieneLetra(instanciaYSusCaracteres * item) {
		return contieneLaInicial(item->caracteres, inicial);
	}
	return list_find(list, (void *) sublistaTieneLetra);

}

