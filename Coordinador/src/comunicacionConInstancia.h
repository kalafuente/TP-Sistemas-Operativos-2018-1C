#ifndef COMUNICACIONCONINSTANCIA_H_
#define COMUNICACIONCONINSTANCIA_H_

#include "coordinador.h"

bool enviarSETaInstancia(instancia * instanciaALlamar, int sock, t_instruccion * instruccion, bool avisarClaveInaccesible);
void mandarConfiguracionAInstancia(int sock);


#endif /* COMUNICACIONCONINSTANCIA_H_ */
