#include "liberacionDeMemoria.h"
void destruirStructEsiClaveSinEsi(struct_esiClaves* elemento) {

	free(elemento->clave);
	free(elemento);
}

void destruirStructEsiClave(struct_esiClaves* elemento) {
	destruirStructEsi(elemento->ESI);
	free(elemento->clave);
	free(elemento);
}

void destruirStructEsi(struct_esi* elemento) {
	free(elemento);
}

void destruirListaEsi(t_list * lista) {
	list_destroy_and_destroy_elements(lista, (void *) destruirStructEsi);
}

void destruirListaBloqueados(t_list * lista) {
	list_destroy_and_destroy_elements(lista, (void*) destruirStructEsiClave);
}

void destruirListaEsiClave(t_list*lista) {
	list_destroy_and_destroy_elements(lista,
			(void*) destruirStructEsiClaveSinEsi);
}
