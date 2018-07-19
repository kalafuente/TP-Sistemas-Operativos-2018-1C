#ifndef DEADLOCK_H_
#define DEADLOCK_H_
#include "Planificador.h"

t_list * esisEnDeadlock(t_list* esisBloqueados, t_list* clavesTomadas);
bool laClaveEstaBloqueadaPorAlguienYeseAlguienEstaBloqueadoPorEsteEsi(t_list* listaEsiClaves, struct_esiClaves* esiBloqueado);
bool esElBloqueadorDeSuBloqueador(struct_esiClaves * esiBloqueado, struct_esiClaves * bloqueador, t_list* listaEsiClaves);
struct_esiClaves * bloqueadorDelBloqueador(t_list* listaEsiClaves, struct_esiClaves* esiBloqueado);
struct_esiClaves * bloqueadorDeClave(t_list* listaEsiClaves, struct_esiClaves* esiBloqueado);


#endif /* DEADLOCK_H_ */
