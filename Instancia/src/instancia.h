#ifndef INSTANCIA_H_
#define INSTANCIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <library/manejoDeSockets.h>
#include <library/protocolos.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <library/archivos.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>

#include <errno.h>

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
	int momentoReferencia;

} t_tabla_entradas;

//------------Variables globales
t_log* logger;
t_log * logOperaciones;
int32_t socketCoordinador;
instancia_config * instanciaConfig;
int32_t cantidadEntradas = 20; //Lo deje en 20 para probar
int32_t tamanioEntrada = 10;// Idem
char * entradas = NULL; //Es un array grande donde la division de entradas sera logica. Es lo mismo que char entradas[cantidadEntradas x tamanioEntrada]
t_list* tablaEntradas = NULL;
//int filaACambiar = 0; //Actua como el puntero que nos dice en que numero de entrada debemos guardar el valor
//int comenzarReemplazoDeValores = 0; //Es como un booleano, nos dice si llenamos las Entradas y tenemos que comenzar a reemplazar valores segun el algoritmo
t_link_element * punteroReempAlgCirc = NULL; //Inicializado cuando se agrega el primer elemento a la lista
int * bitArray = NULL; //Es un array de bits que servira para saber cuales entradas estan libres
int contadorGlobal = 0; //Con cada operacion SET y STORE aumentara en 1. Se guardara el valor en la tabla de entradas al ser referenciada una clave
//int punteroAlgCIRC = 0;
int finInstancia = 1;

pthread_mutex_t mutex;

//ID del hilo del DUMP

pthread_t thread_id;

extern int errno ;


void destruirLogger();
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
int procesarSET(t_instruccion* inst);
void eliminarDatosTablaDeEntradas(void * elemento);
void eliminarTablaDeEntradas();
t_link_element * existeLaClave(char * clave);
// int esAtomicoElValorDeLaClave(char * clave, t_link_element * nodo); Por ahora ya no la uso
//int esAtomicoElValor(int32_t longitudDelValor);
int cuantasEntradasOcupaElValor(int32_t longitudDelValor);
void guardarValorEnEntradas(char * clave, char * valor, int posicionInicial);
void actualizarValorEnEntradas(t_link_element * nodo, char * nuevoValor, int entradasViejoValor, int entradasNuevoValor);
//void separarStringEnNPartesIguales(char * cadena, int longitudCadena, int cantidadPartes, int tamanioParte, char strings[cantidadPartes][tamanioParte]);
//void escribirValorAtomico(char * clave, char * valor, int32_t longitudValor);
//void moverPunteroAlgCIRC();
void imprimirContenidoEntradas();
//int implementarAlgoritmoDeReemplazo(char * clave, char * valor, int32_t longitudValor, int cantidadEntradasAReemp);
//int algoritmoCircular(char * clave, char * valor, int32_t longitudValor, int cantidadEntradasAReemp);
void moverPunteroReempAlgCirc();
int sonEntradasContiguas(int cantidad, int entradasParaComprobar[cantidad]);
//void reemplazarValorAtomico(t_tabla_entradas * dato, char * clave, char * valor, int32_t longitudValor);
int procesarSTORE(t_instruccion * sentencia);

void ordenarArray(int tamanio, int array[tamanio]);

void almacenarValor(int entradaInicial, char * valor);

void crearyAgregarElementoTDE(char * clave, int32_t tamanioValor, int32_t numeroEntrada);

int eleccionDeVictima();

void estructurasLuegoDeOperacion();

//VICTIMAS
int victimaCIRC();
int victimaLRU();
int victimaBSU();
int desempatePorCIRC(int tamanio, int array[tamanio]);

void peticionValor();

int almacenarArchivo(char * pathAbsoluto, char * clave, int32_t tamanioValor, int32_t numeroEntrada);

//DUMP
void crearHiloParaDump();
void * DUMP();
void crearDirectorio();

//MUTEX
void inicializarMutex();

//REINCORPORACION
void reincorporarse();


// ------------------- Funciones bitArray
void inicializarBitArray();
int tamanioBitArray();
void setBit(int numeroEntrada);
void clearBit(int numeroEntrada);
int testBit(int numeroEntrada);
void eliminarBitArray();

// ------------------ Metodos de ordenamiento de listas
bool ordenarPorNumeroDeEntrada(t_tabla_entradas * primerElemento, t_tabla_entradas * segundoElemento);
bool ordenarPorMomentoDeReferencia(t_tabla_entradas * primerElemento, t_tabla_entradas * segundoElemento);
bool ordenarPorEspacioUsado(t_tabla_entradas * primerElemento, t_tabla_entradas * segundoElemento);

#endif /* INSTANCIA_H_ */
