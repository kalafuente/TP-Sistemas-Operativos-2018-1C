#ifndef GESTIONDEESIS_H_
#define GESTIONDEESIS_H_
#include "Planificador.h"

void agregarEsi(int socketCliente);
struct_esiClaves* crearEsiClave(struct_esi* esi, char*clave);
void liberarEsi(char*clave);
void liberarTodasLasClavesDeEsi(struct_esi*esi);

void sacarStructDeListaEsiClave(char*clave);
void agregarEnListaBloqueado(struct_esi *esiActual, char*clave);
void sumarUnoDeEspera(struct_esi* esi);
void sumarEspera();
void cerrarSocketsDeLista(t_list*lista);
void cerrarSocketsDeListaBloqueado(t_list*lista);
#endif /* GESTIONDEESIS_H_ */
