#ifndef COMUNICACIONCONINSTANCIA_H_
#define COMUNICACIONCONINSTANCIA_H_

#include "coordinador.h"

bool enviarSETaInstancia(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible);
bool enviarSETaInstanciaConSETPrevio(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible);
void mandarConfiguracionAInstancia(int sock);
void enviarClavesCorrespondientes(int sock,char * id, t_list* listaDeClavesConInstancia);
t_list* clavesDeLaInstancia(t_list* list, void* value);
void pedirCompactar(t_list* lista);

#endif /* COMUNICACIONCONINSTANCIA_H_ */
