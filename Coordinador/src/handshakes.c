#include "handshakes.h"

void saludar(int sock){
	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi = HANDSHAKE_CONECTAR_COORDINADOR_A_CLIENTES;
	enviarMensaje(logger, sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES), &handshakeCoordi , sock); //Saludamos
}

PROTOCOLO_HANDSHAKE_CLIENTE recibirSaludo(int sock){
	PROTOCOLO_HANDSHAKE_CLIENTE handshake;
	recibirMensaje(logger,sizeof(PROTOCOLO_HANDSHAKE_CLIENTE),&handshake,sock);
	return handshake;
}
