#include "alfabeto.h"


t_list*  crearAlfabeto(){
	t_list* alfabeto;
	alfabeto = list_create();
	char l [1];
	for(int i =97; i<= 122; i++){
		l[0] = i;
		char * letra = string_new();
		string_append(&letra, &l[0]);
		list_add(alfabeto, letra);
	}
	return alfabeto;

}

void destruirAlfabeto(t_list*alfabeto){
	list_destroy (alfabeto); //las letras se destruyen en destuirListaDeLEtrasDeInstancia
}
