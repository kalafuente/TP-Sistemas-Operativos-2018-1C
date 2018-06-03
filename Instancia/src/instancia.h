/*
 * instancia.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */
/*
#ifndef INSTANCIA_H_
#define INSTANCIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <manejoDeSockets/manejoDeSockets.c>

typedef struct instancia_config {
	char * ipCoordi ;
	char * puertoCoordi;
	char * algoritmo;
	char * path ;
	char * nombre ;
	int intervalo;
} instancia_config;

//------------Variables globales
t_log* logger;

//------------Declaraciones de funciones
instancia_config * init_instanciaConfig();
void crearConfiguracion(instancia_config **, t_config**);
void destroy_instanciaConfig(instancia_config*);

*/

//#endif /* INSTANCIA_H_ */


/*
 * instancia.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <manejoDeSockets/manejoDeSockets.c>
#include <protocolos/protocolos.h>

typedef struct instancia_config {
	char * ipCoordi ;
	char * puertoCoordi;
	char * algoritmo;
	char * path ;
	char * nombre ;
	int intervalo;
} instancia_config;

typedef struct t_tabla_entradas
{
	char clave[40+1]; //La key tiene 40 caracteres, asi que le sumamos 1 para el fin de string
	int32_t numeroEntrada;
	int32_t tamanioValor;

} t_tabla_entradas;

//------------Variables globales
t_log* logger;
int32_t socketCoordinador;
instancia_config * instanciaConfig;
int32_t cantidadEntradas = 10; //Lo deje en 10 para probar
int32_t tamanioEntrada = 10;// Idem
char ** entradas = NULL;
t_tabla_entradas * tablaDeEntradas = NULL;


//------------Declaraciones de funciones
instancia_config * init_instanciaConfig();
void crearConfiguracion(instancia_config *, t_config*);
void destroy_instanciaConfig(instancia_config*);
void imprimirConfiguracion(instancia_config* instancia);
int conectarseAlCoordinador();
int recibirConfiguracionDeEntradas();
int handShakeConElCoordinador();
void imprimirConfiguracionDeEntradas();
void inicializarEntradas();
void eliminarEntradas();
int procesarSentencias();
// void procesarGET(); Segun los issues y los erratas la sentencia GET no debe llegar a la instancia. De eso se ocupa el Coordinador
void procesarSET();

#endif /* INSTANCIA_H_ */
