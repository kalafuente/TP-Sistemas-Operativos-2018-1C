#include "deadlock.h"

void mostrarEsisEnDeadlock(t_list* esisBloqueados, t_list* clavesTomadas){
	if (list_size(esisBloqueados) == 0 || list_size(clavesTomadas) == 0){
		log_info(logger, "\n Estamos bien, no hay deadlock. \n ");
	}
	else{

		t_list * esis= esisEnDeadlock(esisBloqueados, clavesTomadas);

		log_info(logger, "\n Esis que estan en deadlock: \n ");

			if (list_size(esis)==0){
					printf("Estamos bien, no hay deadlock \n");
				}
			else {
				void mostrar(struct_esiClaves * esi){
				log_info(logger, "ESI %d, espera: %s \n", esi->ESI->ID,
						esi->clave);
					}
					list_iterate(esis, (void *) mostrar);
			}

	}
}

t_list * esisEnDeadlock(t_list* esisBloqueados, t_list* clavesTomadas){

	bool hayDeadlock(struct_esiClaves* item) {

		if (laClaveEstaBloqueadaPorAlguienYeseAlguienEstaBloqueadoPorEsteEsi(clavesTomadas, item,esisBloqueados))
			return true;

		else
			return false;

	}

	return list_filter(esisBloqueados, (void*) hayDeadlock);
}


bool laClaveEstaBloqueadaPorAlguienYeseAlguienEstaBloqueadoPorEsteEsi(t_list* clavesTomadas, struct_esiClaves* esiBloqueado, t_list* esisBloqueados){
	struct_esiClaves * bloqueadorDelBloqueador_ = bloqueadorDelBloqueador(clavesTomadas,esiBloqueado, esisBloqueados);
	if (bloqueadorDelBloqueador_->ESI->ID == esiBloqueado->ESI->ID)
		return true;
	else
		return esElBloqueadorDeSuBloqueador(esiBloqueado, bloqueadorDelBloqueador_, clavesTomadas, esisBloqueados);
}

bool esElBloqueadorDeSuBloqueador(struct_esiClaves * esiBloqueado, struct_esiClaves * bloqueador, t_list* clavesTomadas, t_list* esisBloqueados){

	int i = 0;
	bool bandera = false;
	struct_esiClaves * bloqueadorDelBloqueador_ = bloqueadorDelBloqueador(clavesTomadas,bloqueador, esisBloqueados);

	while (i<list_size(clavesTomadas) && bandera == false){
		if (bloqueadorDelBloqueador_->ESI->ID == esiBloqueado->ESI->ID)
			bandera= true;
		else
			bloqueadorDelBloqueador_ = bloqueadorDelBloqueador(clavesTomadas,bloqueadorDelBloqueador_, esisBloqueados);

		i++;
	}
	return bandera;
}
struct_esiClaves * bloqueadorDelBloqueador(t_list* clavesTomadas, struct_esiClaves* esiBloqueado, t_list * esisBloqueados){

	struct_esiClaves * bloqueador = obtenerBloqueado(esisBloqueados, bloqueadorDeClave(clavesTomadas,esiBloqueado));
	return bloqueadorDeClave(clavesTomadas,bloqueador);

}

struct_esiClaves * obtenerBloqueado(t_list* bloqueados, struct_esiClaves* esiBloqueado){

	bool obtener(struct_esiClaves* otroEsi) {

		if (esiBloqueado->ESI->ID == otroEsi->ESI->ID)
			return true;

		else
			return false;
	}
	return list_find(bloqueados,(void*)obtener);
}


struct_esiClaves * bloqueadorDeClave(t_list* clavesTomadas, struct_esiClaves* esiBloqueado){

	bool tieneClaveBloqueada(struct_esiClaves* otroEsi) {

		//printf("esi bloqueado id: %d, clave que espera: %s \n", esiBloqueado->ESI->ID, esiBloqueado->clave);
		//printf("otroEsi id: %d, clave tomada: %s \n", otroEsi->ESI->ID, otroEsi->clave);
		int rta = strcmp(esiBloqueado->clave, otroEsi->clave);
		if (rta == 0){
			//printf("devolvi true \n");
			return true;
		}

		else
			return false;
	}
	return list_find(clavesTomadas,(void*)tieneClaveBloqueada);
}
