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
#include <commons/collections/list.h>
#include <manejoDeSockets/manejoDeSockets.c>
#include <protocolos/protocolos.h>
#include <math.h>

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
	char * clave;
	int32_t numeroEntrada;
	int32_t tamanioValor;

} t_tabla_entradas;

//------------Variables globales
t_log* logger;
int32_t socketCoordinador;
instancia_config * instanciaConfig;
int32_t cantidadEntradas = 20; //Lo deje en 20 para probar
int32_t tamanioEntrada = 10;// Idem
char ** entradas = NULL; //Es Filas x Columnas ... char entradas[CantidadEntradas][TamanioEntrada]
t_list* tablaEntradas = NULL;
int filaACambiar = 0; //Actua como el puntero que nos dice en que numero de entrada debemos guardar el valor
int comenzarReemplazoDeValores = 0; //Es como un booleano, nos dice si llenamos las Entradas y tenemos que comenzar a reemplazar valores segun el algoritmo


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
void eliminarDatosTablaDeEntradas(void * elemento);
void eliminarTablaDeEntradas();
int existeLaClave(char * clave, t_tabla_entradas * info);
// int esAtomicoElValorDeLaClave(char * clave, t_link_element * nodo); Por ahora ya no la uso
int esAtomicoElValor(int32_t longitudDelValor);
int cuantasEntradasOcupaElValor(int32_t longitudDelValor);
void actualizarValorEnEntradas(t_tabla_entradas * info, char * valor, int32_t longitudDelValor);
void separarStringEnNPartesIguales(char * cadena, int longitudCadena, int cantidadPartes, int tamanioParte, char strings[cantidadPartes][tamanioParte]);

#endif /* INSTANCIA_H_ */
