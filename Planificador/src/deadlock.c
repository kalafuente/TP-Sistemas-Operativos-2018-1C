#include "deadlock.h"

t_list * esisEnDeadlock(t_list* esisBloqueados, t_list* clavesTomadas){

	bool hayDeadlock(struct_esiClaves* item) {
		if (laClaveEstaBloqueadaPorAlguienYeseAlguienEstaBloqueadoPorEsteEsi(clavesTomadas, item))
			return true;
		else
			return false;
	}

	return list_filter(esisBloqueados, (void*) hayDeadlock);
}


bool laClaveEstaBloqueadaPorAlguienYeseAlguienEstaBloqueadoPorEsteEsi(t_list* clavesTomadas, struct_esiClaves* esiBloqueado){

	struct_esiClaves * bloqueadorDelBloqueador_ = bloqueadorDelBloqueador(clavesTomadas,esiBloqueado);
	if (bloqueadorDelBloqueador_->ESI->ID == esiBloqueado->ESI->ID)
		return true;
	else
		return esElBloqueadorDeSuBloqueador(esiBloqueado, bloqueadorDelBloqueador_, clavesTomadas);
}

bool esElBloqueadorDeSuBloqueador(struct_esiClaves * esiBloqueado, struct_esiClaves * bloqueador, t_list* clavesTomadas){
	int i = 0;
	bool bandera = false;
	struct_esiClaves * bloqueadorDelBloqueador_ = bloqueadorDelBloqueador(clavesTomadas,bloqueador);
	while (i<list_size(clavesTomadas) && bandera == false){
		if (bloqueadorDelBloqueador_->ESI->ID == esiBloqueado->ESI->ID)
			bandera= true;
		else
			bloqueadorDelBloqueador_ = bloqueadorDelBloqueador(clavesTomadas,bloqueadorDelBloqueador_);

		i++;
	}
	return bandera;
}
struct_esiClaves * bloqueadorDelBloqueador(t_list* clavesTomadas, struct_esiClaves* esiBloqueado){
	struct_esiClaves * bloqueador = bloqueadorDeClave(clavesTomadas,esiBloqueado);
	return bloqueadorDeClave(clavesTomadas,bloqueador);

}

struct_esiClaves * bloqueadorDeClave(t_list* clavesTomadas, struct_esiClaves* esiBloqueado){
	bool tieneClaveBloqueada(struct_esiClaves* otroEsi) {
		int rta = strcmp(esiBloqueado->clave, otroEsi->clave);
		if (rta == 0)
			return true;
		else
			return false;
	}
	return list_find(clavesTomadas,(void*)tieneClaveBloqueada);
}
