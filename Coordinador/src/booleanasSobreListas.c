#include "booleanasSobreListas.h"

bool contieneString(t_list* list, void* value){

	bool equals(void* item) {
		int rta = strcmp(value, item);
		if (rta == 0)
				return true;
		else
				return false;
	}

	return list_any_satisfy(list, equals);
}

bool contieneClave(t_list* list, void* value){

	bool equals(claveConInstancia* item) {
		int rta = strcmp(value, item->clave);
		if (rta == 0)
				return true;
		else
				return false;
	}

	return list_any_satisfy(list, (void *) equals);
}

bool contieneLaInicial(t_list* list, void* inicial){

	bool equals(char * item) {
		int rta = strcmp(inicial, item);
		if (rta == 0)
				return true;
		else
				return false;
	}

	return list_any_satisfy(list, (void *) equals);
}


