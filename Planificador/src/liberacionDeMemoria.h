/*
 * liberacionDeMemoria.h
 *
 *  Created on: 19 jul. 2018
 *      Author: utnso
 */

#ifndef LIBERACIONDEMEMORIA_H_
#define LIBERACIONDEMEMORIA_H_
#include "Planificador.h"
void destruirStructEsiClaveSinEsi(struct_esiClaves* elemento);
void destruirStructEsiClave(struct_esiClaves* elemento);
void destruirStructEsi(struct_esi* elemento);
void destruirListaEsi(t_list * lista);
void destruirListaBloqueados(t_list * lista);
void destruirListaEsiClave(t_list*lista);

#endif /* LIBERACIONDEMEMORIA_H_ */
