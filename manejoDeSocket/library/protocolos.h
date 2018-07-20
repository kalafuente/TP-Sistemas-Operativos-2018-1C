#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include "manejoDeSockets.h"

#ifndef PROTOCOLOS_PROTOCOLOS_H_
#define PROTOCOLOS_PROTOCOLOS_H_

typedef enum PROTOCOLO_HANDSHAKE_CLIENTE {
	HANDSHAKE_CONECTAR_ESI_A_COORDINADOR,
	HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR,
	HANDSHAKE_CONECTAR_PLANIFICADOR_A_COORDINADOR,
	HANDSHAKE_CONECTAR_STATUS_A_COORDINADOR,
} PROTOCOLO_HANDSHAKE_CLIENTE;

typedef enum CONSULTA{
	CONSULTA_ID
}CONSULTA;

typedef enum PROTOCOLO_COORDINADOR_A_CLIENTES {
	HANDSHAKE_CONECTAR_COORDINADOR_A_CLIENTES, TODO_OK, TODO_MAL
} PROTOCOLO_COORDINADOR_A_CLIENTES;

typedef enum PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI {
	ERROR_CLAVE_NO_BLOQUEADA,
	ERROR_CLAVE_INACCESIBLE,
	ERROR_CLAVE_NO_IDENTIFICADA,
	BLOQUEATE,
	TODO_OK_ESI,

} PROTOCOLO_RESPUESTA_DEL_COORDI_AL_ESI;

typedef enum PROTOCOLO_COORDINADOR_A_INSTANCIA {
	ENTRADAS,
	PEDIDO_DE_VALOR,

} PROTOCOLO_COORDINADOR_A_INSTANCIA;

typedef enum PROTOCOLO_INSTRUCCIONES {
	INSTRUCCION_GET, INSTRUCCION_SET, INSTRUCCION_STORE, PEDIDO_VALOR, COMPACTAR
} PROTOCOLO_INSTRUCCIONES;

typedef enum PROTOCOLO_ESI_A_COORDI {
	MANDO_INTRUCCIONES,
	TERMINE_INSTRUCCIONES
} PROTOCOLO_ESI_A_COORDI;


typedef enum PROTOCOLO_ESI_A_PLANIFICADOR {
	HANDSHAKE_CONECTAR_ESI_A_PLANIFICADOR,
	TERMINE_BIEN = 1,
	BLOQUEADO_CON_CLAVE,
	TERMINE,
	ERROR //hay que ir agregando los distintos errores(aunque quizas sea mejor en otro protocolo)
} PROTOCOLO_ESI_A_PLANIFICADOR;

typedef enum PROTOCOLO_PLANIFICADOR_A_ESI {
	HANDSHAKE_CONECTAR_PLANIFICADOR_A_ESI, ACTUAR, FINALIZAR
} PROTOCOLO_PLANIFICADOR_A_ESI;

typedef enum PROTOCOLO_INSTANCIA_A_COORDINADOR{
	PEDIDO_DE_CLAVES,
	SE_PUDO_GUARDAR_VALOR,
	NO_SE_PUDO_GUARDAR_VALOR,
	SE_SOBREESCRIBIO_LA_CLAVE, //Despues deberia enviar el tamanio y la clave en cuestion (3 mensajes)
	SE_CREO_EL_ARCHIVO,
	NO_SE_CREO_EL_ARCHIVO,
	ERROR_INSTRUCCION

} PROTOCOLO_INSTANCIA_A_COORDINADOR;


typedef enum PROTOCOLO_COORDINADOR_A_PLANIFICADOR{
	PREGUNTA_CLAVE_DISPONIBLE,
	PEDIDO_DE_ID,
	PREGUNTA_ESI_TIENE_CLAVE,

 }PROTOCOLO_COORDINADOR_A_PLANIFICADOR;


 typedef enum PROTOCOLO_PLANIFICADOR_A_COORDINADOR{
	ESI_TIENE_CLAVE, //LE PERTENECE AL ESI EJECUTANDO
	ESI_NO_TIENE_CLAVE, //NO LE PERTENECE AL ESI EJECUTANDO
	CLAVE_DISPONIBLE,
	CLAVE_NO_DISPONIBLE
} PROTOCOLO_PLANIFICADOR_A_COORDINADOR;

typedef enum PROTOCOLO_SOLICITUD_ESI{
	NO_ME_QUIERO_IR_SR_COORDINADOR,
	AHORA_TE_MANDO_TODO
}PROTOCOLO_SOLICITUD_ESI;

typedef struct instruccion {
 	PROTOCOLO_INSTRUCCIONES instruccion;
 	char * clave;
 	char * valor;
}t_instruccion;

void destruirInstruccion(t_instruccion* instruccion);
int enviarInstruccion(t_log* logger,t_instruccion* instruccion, int sock);
t_instruccion * recibirInstruccion(t_log* logger,int sock, char* deQuien);
t_instruccion * crearInstruccion(PROTOCOLO_INSTRUCCIONES tipoInstruccion, char * clave, char * valor);
char* recibirID(int sock, t_log* logger);
int enviarID(int sock ,char* mensaje, t_log* logger);

#endif /* PROTOCOLOS_PROTOCOLOS_H_ */
