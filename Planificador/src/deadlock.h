#ifndef DEADLOCK_H_
#define DEADLOCK_H_
#include "Planificador.h"

void mostrarEsisEnDeadlock(t_list* esisBloqueados, t_list* clavesTomadas);
t_list * esisEnDeadlock(t_list* esisBloqueados, t_list* clavesTomadas);
bool laClaveEstaBloqueadaPorAlguienYeseAlguienEstaBloqueadoPorEsteEsi(t_list* clavesTomadas, struct_esiClaves* esiBloqueado, t_list* esisBloqueados);
bool esElBloqueadorDeSuBloqueador(struct_esiClaves * esiBloqueado, struct_esiClaves * bloqueador, t_list* clavesTomadas, t_list* esisBloqueados);
struct_esiClaves * bloqueadorDeClave(t_list* listaEsiClaves, struct_esiClaves* esiBloqueado);
struct_esiClaves * bloqueadorDelBloqueador(t_list* clavesTomadas, struct_esiClaves* esiBloqueado, t_list * esisBloqueados);
struct_esiClaves * obtenerBloqueado(t_list* bloqueados, struct_esiClaves* esiBloqueado);
#endif /* DEADLOCK_H_ */
