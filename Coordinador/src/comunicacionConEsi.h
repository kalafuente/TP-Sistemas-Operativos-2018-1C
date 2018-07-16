#ifndef COMUNICACIONCONESI_H_
#define COMUNICACIONCONESI_H_

#include "coordinador.h"

t_instruccion* recibirInstruccionDelEsi(int sock);
void enviarRespuestaAlEsi(PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI rta, int sock, t_log* logger);

#endif /* COMUNICACIONCONESI_H_ */
