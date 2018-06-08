#include "instancia.h"


int main(void)
{

	logger= crearLogger("loggerInstancia.log","loggerInstancia");
	log_info(logger, "**************************************** NUEVA ENTRADA ****************************************");
	t_config * config = config_create("configuracionInstancia.config");
	instanciaConfig = init_instanciaConfig();
	crearConfiguracion(instanciaConfig, config);
	imprimirConfiguracion(instanciaConfig);
	conectarseAlCoordinador();
	handShakeConElCoordinador();
	recibirConfiguracionDeEntradas();
	imprimirConfiguracionDeEntradas();
	inicializarEntradas();
	tablaEntradas = list_create();
	procesarSentencias();
	imprimirContenidoEntradas();


	close(socketCoordinador);
	destroy_instanciaConfig(instanciaConfig);
	config_destroy(config);
	eliminarEntradas();
	eliminarTablaDeEntradas();


	return 0;

}

int recibirConfiguracionDeEntradas()
{
	PROTOCOLO_COORDINADOR_A_INSTANCIA entradas;

	log_info(logger, "Esperando configuracion de entradas\n");

	if (recibirMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&entradas,socketCoordinador) <= 0)
	{
		log_error(logger, "No se pudo recibir la configuracion de las Entradas\n");

		return -1;
	}

	if(recibirMensaje(logger,sizeof(int32_t),&cantidadEntradas,socketCoordinador) <= 0)
	{
		log_error(logger, "No se pudo establecer la cantidad de entradas\n");

		return -1;
	}

	if(recibirMensaje(logger,sizeof(int32_t),&tamanioEntrada,socketCoordinador) <= 0 )
	{
		log_error(logger, "No se pudo establecer el tamanio de las entradas\n");

		return -1;
	}

	log_info(logger, "La configuracion de las Entradas se recibio correctamente!\n");

	return 1;

}

int conectarseAlCoordinador()
{
	//socketCoordinador = 0;

	//while(socketCoordinador == 0)
	//{
		log_info(logger, "Intento conectarme al Coordinador\n");
		socketCoordinador = conectarseAlServidor(logger,&instanciaConfig->ipCoordi, &instanciaConfig->puertoCoordi);
		if(socketCoordinador == -1)
		{
			log_error(logger, "Conexion fallida, no se pudo crear el socket\n");
			return -1;
		}
	//}

	log_info(logger, "Conexion exitosa!");

	return 1;

}

int handShakeConElCoordinador()
{
	log_info(logger, "Intentando establecer un handshake con el Coordinador\n");

	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;

	if(recibirMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES),&handshakeCoordi,socketCoordinador) <= 0)
	{
		log_error(logger, "Handshake fallido\n");
		return -1;
	}

	PROTOCOLO_HANDSHAKE_CLIENTE handshakeINSTANCIA = HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR;

	if(enviarMensaje(logger,sizeof(PROTOCOLO_HANDSHAKE_CLIENTE),&handshakeINSTANCIA,socketCoordinador) <= 0)
	{
		log_error(logger, "Handshake fallido\n");
		return -1;
	}

	log_info(logger, "Handshake Exitoso!\n");

	return 1;
}


void crearConfiguracion(instancia_config* instancia, t_config* config)
{
	string_append(&(instancia->ipCoordi), config_get_string_value(config, "IP_COORDI"));
	string_append(&(instancia->puertoCoordi), config_get_string_value(config, "PUERTO_COORDI"));
	string_append(&(instancia->algoritmo), config_get_string_value(config, "ALGORITMO_DE_REEMPLAZO"));
	string_append(&(instancia->path), config_get_string_value(config, "PATH"));
	string_append(&(instancia->nombre), config_get_string_value(config, "NOMBRE"));
	instancia->intervalo = config_get_int_value(config, "INTERVALO_DUMP");

}


instancia_config * init_instanciaConfig()
{

	instancia_config * instanciaConfig = malloc(sizeof(instancia_config));
	instanciaConfig->ipCoordi = string_new();
	instanciaConfig->puertoCoordi = string_new();
	instanciaConfig->algoritmo = string_new();
	instanciaConfig->path = string_new();
	instanciaConfig->nombre = string_new();
	instanciaConfig->intervalo = 0;

	return instanciaConfig;
}


void destroy_instanciaConfig(instancia_config * instancia)
{
	free(instancia->algoritmo);
	free(instancia->ipCoordi);
	free(instancia->nombre);
	free(instancia->path);
	free(instancia->puertoCoordi);
	free(instancia);

}


void imprimirConfiguracion(instancia_config* instancia)
{
	log_info(logger, "El ip del Coordinador es: %s\n", instancia->ipCoordi);
	log_info(logger, "El puerto del Coordinador es: %s\n", instancia->puertoCoordi);
	log_info(logger, "El algoritmo es: %s\n", instancia->algoritmo);
	log_info(logger, "El path es: %s\n", instancia->path);
	log_info(logger, "El nombre es: %s\n", instancia->nombre);
	log_info(logger, "El intervalo es: %d\n", instancia->intervalo);
}


void imprimirConfiguracionDeEntradas()
{
	log_info(logger, "El tamanio de la entrada es: %d\n", tamanioEntrada);
	log_info(logger, "La cantidad de entradas es: %d\n", cantidadEntradas);
}

void inicializarEntradas()
{
	log_info(logger, "Pedimos memoria para la totalidad de las Entradas\n");

	entradas = (char **)malloc(cantidadEntradas * sizeof(char*));

	if(entradas == NULL)
	{
		log_error(logger, "Fallo al intentar pedir memoria para las Entradas\n");
		return;
	}

	int fila, columna;

	for(fila = 0; fila < cantidadEntradas; fila ++)
	{
		entradas[fila] = (char*)malloc(tamanioEntrada * sizeof(char));

		if(entradas[fila] == NULL)
		{
			log_error(logger, "Fallo al intentar pedir memoria para la fila de las Entradas\n");
			return;
		}
	}

	log_info(logger, "Memoria alojada correctamente\n");
	log_info(logger, "Inicializamos los valores de las Entradas en 0\n");


	for(fila = 0; fila < cantidadEntradas; fila ++)
	{
		for(columna = 0; columna < tamanioEntrada; columna ++)
		{
			entradas[fila][columna] = '0';
		}
	}

	log_info(logger, "Inicializacion exitosa!\n");
}

void eliminarEntradas()
{
	int fila;

	for(fila = 0; fila < cantidadEntradas; fila ++)
	{
		free(entradas[fila]);
	}

	free(entradas);
	entradas = NULL;
}

int procesarSentencias()
{
	int corte = 1;

	t_instruccion* sentencia;

	log_info(logger, "Comienzo a recibir sentencias del coordinador\n");

	while(corte) //Habria que ver cuando cortar
	{
		log_info(logger, "Esperando proxima sentencia...\n");

		sentencia = recibirInstruccion(logger, socketCoordinador);

		if(sentencia == NULL)
		{
			log_error(logger, "No se pudo recibir la sentencia\n");

			return -1;
		}

		switch(sentencia->instruccion)
		{
			/* No le deberian llegar.
			case INSTRUCCION_GET:
				procesarGET();
				break;
			*/

			case INSTRUCCION_SET:
				procesarSET(sentencia);
				break;

			case INSTRUCCION_STORE:
				//Mas adelante vemos
				break;

			default:

				log_error(logger, "La sentencia no puede ser interpretada\n");
				destruirInstruccion(sentencia);
				return -1;
		}
	}

	destruirInstruccion(sentencia);

	return 1;
}

void procesarSET(t_instruccion* inst)
{

	t_tabla_entradas * datos = NULL;

	switch(existeLaClave(inst->clave, datos))
	{
		case 0: //La clave no existe
			//Guardamos el valor en las Entradas.
			//Creamos un nuevo nodo en la Tabla de Entradas con su correspondiente clave, numero de entrada y tamanio del valor
			log_info(logger, "La clave no existe. Intentamos guardar su valor en las Entradas\n");
			if(guardarValorEnEntradas(inst->clave, inst->valor, (strlen(inst->valor)+1)) < 0)
			{
				log_error(logger, "Fallo en la operacion SET. No se pudo almacenar el valor\n");
				//Enviar un mensaje avisando al coordinador del fallo?
				return;
			}
			log_info(logger, "Se pudo almacenar el valor\n");
			break;

		case 2: //La clave existe y su valor almacenado es atomico
			//Guardamos el valor en las entradas en la posicion que nos indica el nodo
			//Actualizamos el nodo en el que estan los datos de la clave
			log_info(logger, "La clave existe. Se intenta actualizar su valor\n");
			actualizarValorEnEntradas(datos, inst->valor, (strlen(inst->valor)+1));
			log_info(logger, "Valor actualizado\n");
			break;

		default: //Cuando devuelve 1, quiere decir que el valor almacenado de esa clave no es atomico
			log_error(logger, "El valor de la clave en cuestion no es atomico, por lo que no se puede reemplazar\n");
			return;
	}

	log_info(logger, "Operacion SET exitosa!\n");
	//Enviar un mensaje al coordinador avisando de que se guardo todo bien?

	return;
}

void eliminarDatosTablaDeEntradas(void * elemento)
{
	free(((t_tabla_entradas *)elemento)->clave);
	free(((t_tabla_entradas *)elemento));
}

void eliminarTablaDeEntradas()
{
	void (*borrarDatos)(void *);
	borrarDatos = eliminarDatosTablaDeEntradas;
	list_destroy_and_destroy_elements(tablaEntradas, borrarDatos);
}

int existeLaClave(char * clave, t_tabla_entradas * info) //Si existe la clave devuelve si el valor es atomico (2) o no (1). Ademas devuelve el campo de datos del nodo donde esta.
{
	/*
	if(list_is_empty(tablaEntradas)) //Quizas no haga falta, pero por las dudas
	{
		return 0;
	}
	*/

	t_link_element * actual = tablaEntradas->head;

	while(actual != NULL)
	{
		t_tabla_entradas * datos = (t_tabla_entradas *) actual->data;

		if(strcmp(clave, datos->clave) == 0)
		{
			info = datos;
			return esAtomicoElValor(datos->tamanioValor) + 1;
		}

		actual = actual->next;
	}

	return 0;

}

/* Ya no la uso

int esAtomicoElValorDeLaClave(char * clave, t_link_element * nodo)
{
	t_link_element * siguiente = nodo->next;
	t_tabla_entradas * datos = (t_tabla_entradas *) siguiente->data;

	if(strcmp(clave, datos->clave) == 0)
	{
		return 0;
	}

	return 1;
}

*/

int esAtomicoElValor(int32_t longitudDelValor)
{
	if(cuantasEntradasOcupaElValor(longitudDelValor) - 1)
	{
		return 0;
	}

	return 1;
}

int cuantasEntradasOcupaElValor(int32_t longitudDelValor)
{
	double entradasQueOcupa = (double)(longitudDelValor - 1) / (double)(tamanioEntrada - 1);

	return (int) ceil(entradasQueOcupa);
}

int guardarValorEnEntradas(char * clave, char * valor, int32_t longitudDelValor)
{
	int cuantasEntradasDeboEscribir = cuantasEntradasOcupaElValor(longitudDelValor);

	if(comenzarReemplazoDeValores)
	{
		//Tengo que implementar algun algoritmo para comenzar a reemplazar
		if(implementarAlgoritmoDeReemplazo(clave, valor, longitudDelValor, cuantasEntradasDeboEscribir) < 0)
		{
			log_error(logger, "Fallo en la implementacion del Algoritmo de Reemplazo\n");
			return -1;
		}
	}


	//Lo guardo en el espacio siguiente libre
	int cantidadEntradasLibres = cantidadEntradas - tablaEntradas->elements_count;

	if(cuantasEntradasDeboEscribir <= cantidadEntradasLibres) //Sabemos que las entradas son contiguas porque todavia no dimos una vuelta. Siempre almacenamos un valor al lado del otro
	{
		if(cuantasEntradasDeboEscribir == 1)
		{
			escribirValorAtomico(clave, valor, longitudDelValor);
			return 1;
		}

		char valorPartido[cuantasEntradasDeboEscribir][tamanioEntrada];
		separarStringEnNPartesIguales(valor, longitudDelValor, cuantasEntradasDeboEscribir, tamanioEntrada, valorPartido);

		int parte;

		for(parte = 0; parte < (cuantasEntradasDeboEscribir - 1); parte ++)
		{
			escribirValorAtomico(clave, valorPartido[parte], longitudDelValor);
		}

		return 2;

	}

	//Este seria el caso en el que, por ej, tengo 1 entrada libre (y es la ultima) pero tengo que escribir dos

	return 0;


}

void actualizarValorEnEntradas(t_tabla_entradas * info, char * valor, int32_t longitudDelValor)
{
	strcpy(entradas[info->numeroEntrada], valor);
	info->tamanioValor = longitudDelValor;
	//Capaz tenga que agregar la hora en la que se modifico por ultima vez como un campo en la Tabla de entradas
	//Habria que actualizarlo tambien
}

//La tuve que ajustar para que al final de cada pedazo de valor agregue un "\0". Parece que funciona, pero puede traer problemas...

void separarStringEnNPartesIguales(char * cadena, int longitudCadena, int cantidadPartes, int tamanioParte, char strings[cantidadPartes][tamanioParte])
{

	int caracter, parte, posCadena;
	parte = -1;

	for(posCadena = 0; posCadena < (longitudCadena - 1); posCadena ++)
	{
		if(posCadena % (tamanioParte - 1) == 0)
		{
			parte ++;
			caracter = 0;
		}

		strings[parte][caracter] = cadena[posCadena];
		caracter ++;
	}

	int i;

	for(i = 0; i < (cantidadPartes - 1); i++)
	{
		strings[i][tamanioParte - 1] = '\0';
	}

	strings[i][caracter] = '\0';
}

void escribirValorAtomico(char * clave, char * valor, int32_t longitudValor)
{
	strcpy(entradas[filaACambiar], valor);
	t_tabla_entradas * nuevoCampoData = malloc(sizeof(t_tabla_entradas));
	nuevoCampoData->clave = string_new();
	string_append(&(nuevoCampoData->clave), clave);
	nuevoCampoData->numeroEntrada = filaACambiar;
	nuevoCampoData->tamanioValor = longitudValor;

	list_add(tablaEntradas, nuevoCampoData);
	moverPunteroAFila();

	if(list_size(tablaEntradas) == 1)
	{
		punteroReempAlgCirc = tablaEntradas->head;
	}
}

void moverPunteroAFila()
{
	if(filaACambiar == (cantidadEntradas - 1))
	{
		filaACambiar = 0;
		comenzarReemplazoDeValores = 1;
		return;
	}

	filaACambiar ++;
}

void imprimirContenidoEntradas()
{
	if(list_is_empty(tablaEntradas))
	{
		log_info(logger, "No se guardo nada en las Entradas. No hay nada que imprimir\n");
		return;
	}

	log_info(logger, "Comenzamos a imprimir los valores\n");

	t_link_element * lista = tablaEntradas->head;

	while(lista != NULL)
	{
		t_tabla_entradas * datos = ((t_tabla_entradas *)lista->data);
		int entradasQueOcupaElValor = cuantasEntradasOcupaElValor(datos->tamanioValor);

		if(entradasQueOcupaElValor == 1)
		{
			log_info(logger, "La clave es: %s\n", datos->clave);
			log_info(logger, "Su valor asociado es: %s\n", entradas[datos->numeroEntrada]);
			lista = lista->next;
		}
		else
		{
			char * valorCompleto = string_new();
			char * key = datos->clave;
			int parte;

			for(parte = 0; parte < entradasQueOcupaElValor; parte ++)
			{
				string_append(&valorCompleto, entradas[datos->numeroEntrada]);
				lista = lista->next;
				datos = ((t_tabla_entradas *)lista->data);
			}

			log_info(logger, "La clave es: %s\n", key);
			log_info(logger, "Su valor asociado es: %s\n", valorCompleto);

			free(valorCompleto);

		}
	}

	log_info(logger, "Los valores se han impreso correctamente\n");
}

int implementarAlgoritmoDeReemplazo(char * clave, char * valor, int32_t longitudValor, int cantidadEntradasAReemp)
{

	if(strcmp(instanciaConfig->algoritmo, "CIRC") == 0)
	{
		//El algoritmo es el de reemplazo circular
		//Llamamos a la funcion que lo hace
		if(algoritmoCircular(clave, valor, longitudValor, cantidadEntradasAReemp) < 0)
		{
			log_error(logger, "Error al reemplazar por Algoritmo CIRC\n");
			return -1;
		}

		return 1;
	}

	if(strcmp(instanciaConfig->algoritmo, "LRU") == 0)
	{
		//El algoritmo de reemplazo se basa en la entrada que mas tiempo ha estado sin cambios
		//Llamamos a la funcion que lo hace

	}

	if(strcmp(instanciaConfig->algoritmo, "BSU") == 0)
	{
		//Reemplaza las entradas atomicas que ocupen mas espacio
		//Llamamos a la funcion que lo hace
	}

	log_error(logger, "Algoritmo incompatible, se debe revisar la configuracion recibida\n");
	return -4;

}

int algoritmoCircular(char * clave, char * valor, int32_t longitudValor, int cantidadEntradasAReemp)
{
	int contador = 0;
	int primerLoop = 0;
	t_link_element * posicionInicial = NULL;
	t_tabla_entradas * entradasParaReemplazar[cantidadEntradasAReemp];

	while(contador < cantidadEntradasAReemp && (posicionInicial != punteroReempAlgCirc)) //Tenemos que ver que no haya pegado una vuelta, porque sino se queda en un bucle infinito
	{
		if(esAtomicoElValor(((t_tabla_entradas *)punteroReempAlgCirc->data)->tamanioValor)) //Si es atomico quiere decir que lo puedo reemplazar
		{
			entradasParaReemplazar[contador] = ((t_tabla_entradas *)punteroReempAlgCirc->data);
			contador ++;
		}

		if(primerLoop == 0)
		{
			posicionInicial = punteroReempAlgCirc;
			primerLoop = 1;
		}

		moverPunteroReempAlgCirc();

	}

	if(posicionInicial == punteroReempAlgCirc) //Tenemos que corroborar si salio del ciclo porque encontro entradas o porque dio la vuelta y no hay disponibles
	{
		log_error(logger, "No se disponen de entradas suficientes para reemplazar\n");
		return -1;
	}

	if(cantidadEntradasAReemp == 1) //Como es una sola ni corroboramos que sean contiguas, ni separamos el valor en partes
	{
		filaACambiar = entradasParaReemplazar[0]->numeroEntrada;
		reemplazarValorAtomico(entradasParaReemplazar[0], clave, valor, longitudValor);
		log_info(logger, "Se ha reemplazado el valor en la entrada correctamente\n");
		return 1;
	}

	//Ahora tenemos que corroborar que las entradas sean contiguas. Sino hay que implementar compactacion

	if(sonEntradasContiguas(cantidadEntradasAReemp, entradasParaReemplazar) == 0) //Quiere decir que no son contiguas
	{
		//Al no ser contiguas hay que compactar, pero por ahora no podemos
		log_error(logger, "Las entradas no son contiguas, por lo que hay que compactar y aun no es posible\n");
		return -2;
	}

	//Si estamos aca quiere decir que son contiguas, tenemos que separar el valor y reemplazarlas por lo nuevo

	char valorPartido[cantidadEntradasAReemp][tamanioEntrada];
	separarStringEnNPartesIguales(valor, longitudValor, cantidadEntradasAReemp, tamanioEntrada, valorPartido);

	int pos;

	for(pos = 0; pos < cantidadEntradasAReemp; pos ++)
	{
		filaACambiar = entradasParaReemplazar[pos]->numeroEntrada;
		reemplazarValorAtomico(entradasParaReemplazar[pos], clave, valor, longitudValor);
	}

	log_info(logger, "Todas las entradas han sido reemplazadas correctamente\n");

	return 1;

}

void moverPunteroReempAlgCirc()
{
	if(punteroReempAlgCirc->next == NULL) //Quiere decir que esta en el ultimo elemento
	{
		punteroReempAlgCirc = tablaEntradas->head;
		return;
	}

	punteroReempAlgCirc = punteroReempAlgCirc->next;
}

int sonEntradasContiguas(int cantidad, t_tabla_entradas * entradasParaReemplazar[cantidad])
{
	//Primero las tengo que ordenar - Bubble Sort porque es facil... (y tampoco va a ser muy grande el array)

	int i, j;
	t_tabla_entradas * auxiliar;

	for(i = 0; i < cantidad; i ++)
	{
		for(j = 1; j < (cantidad - 1); j ++)
		{
			if(entradasParaReemplazar[j]->numeroEntrada < entradasParaReemplazar[j-1]->numeroEntrada)
			{
				auxiliar = entradasParaReemplazar[j-1];
				entradasParaReemplazar[j-1] = entradasParaReemplazar[j];
				entradasParaReemplazar[j] =  auxiliar;
			}
		}
	}

	//Ahora tenemos que ver que sean contiguas

	i = 1;

	while(i < cantidad)
	{
		if(entradasParaReemplazar[i]->numeroEntrada != (entradasParaReemplazar[i-1]->numeroEntrada + 1)) //Significaria que no son contiguas
		{
			return 0; //Devuelve falso porque no son contiguas
		}

		cantidad ++;
	}

	return 1; //Devuelve verdadero porque son contiguas
}

void reemplazarValorAtomico(t_tabla_entradas * dato, char * clave, char * valor, int32_t longitudValor)
{
	strcpy(entradas[filaACambiar], valor);
	dato->clave = realloc(dato->clave, strlen(clave) + 1);
	strcpy(dato->clave, clave);
	dato->numeroEntrada = filaACambiar;
	dato->tamanioValor = longitudValor;
}

/*
void procesarGET()
{
	int32_t longitudKey = 41; //Es un valor fijo que contempla el fin de string
	char key[longitudKey];

	log_info(logger, "La sentencia es GET. Pedimos la clave\n");

	if(recibirMensaje(logger, longitudKey, key, socketCoordinador) <= 0)
	{
		log_error(logger, "Imposible recibir clave\n"); //No tendria que pasar. Si nos llega GET atras deberia estar la clave

		return;
	}


}

*/
