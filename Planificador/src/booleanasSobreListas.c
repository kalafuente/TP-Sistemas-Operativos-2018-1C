#include "booleanasSobreListas.h"


int tieneAlgunEsiLaClave(t_list* lista, char *claveBuscada) {
	int _soy_la_clave_buscada(struct_esiClaves * elemento) {

		return (string_equals_ignore_case(elemento->clave, claveBuscada));

	}
	return (list_any_satisfy(lista, (void*) _soy_la_clave_buscada));
}

int perteneceClaveAlEsi(t_list *lista, char* claveBuscada) {
	int _soy_la_clave_buscada(struct_esiClaves * elemento) {

		struct_esi* esiEjecutando = list_get(listaEjecutando, 0);
		return (string_equals_ignore_case(elemento->clave, claveBuscada))
				&& (elemento->ESI->ID == esiEjecutando->ID);
	}
	return (list_any_satisfy(lista, (void*) _soy_la_clave_buscada));
}
