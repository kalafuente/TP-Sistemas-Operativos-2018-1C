/*#include "instancia.h"


int main(void) {

	logger= crearLogger("loggerInstancia.log","loggerInstancia");
	t_config * config = malloc(sizeof(t_config));
	instancia_config * instanciaConfig = init_instanciaConfig();
	crearConfiguracion(&instanciaConfig, &config);
	int socketCoordinador = conectarseAlServidor(logger,&instanciaConfig->ipCoordi,&instanciaConfig->puertoCoordi);
	recibirMensaje(logger, socketCoordinador);
	enviarMensaje(logger, ID_INSTANCIA, "SOYINSTANCIA", socketCoordinador);

	close(socketCoordinador);
	destroy_instanciaConfig(instanciaConfig);
	config_destroy(config);

	return 0;

}


void crearConfiguracion(instancia_config** instancia, t_config** config) {

	*config = config_create("configuracionInstancia.config");
	(*instancia)->ipCoordi = config_get_string_value(*config, "IP_COORDI");
	(*instancia)->puertoCoordi = config_get_string_value(*config, "PUERTO_COORDI");
	(*instancia)->algoritmo = config_get_string_value(*config, "ALGORITMO_DE_REEMPLAZO");
	(*instancia)->path = config_get_string_value(*config, "PATH");
	(*instancia)->nombre = config_get_string_value(*config, "NOMBRE");
	(*instancia)->intervalo = config_get_int_value(*config, "INTERVALO_DUMP");

}
instancia_config * init_instanciaConfig() {

	instancia_config * instanciaConfig = malloc(sizeof(instancia_config));
	instanciaConfig->ipCoordi = malloc(sizeof(char) * 6);
	instanciaConfig->puertoCoordi = malloc(sizeof(char) * 4);
	instanciaConfig->algoritmo = malloc(sizeof(char) * 4);
	instanciaConfig->path = malloc(sizeof(char) * 23);
	instanciaConfig->nombre = malloc(sizeof(char) * 10);
	return instanciaConfig;
}


void destroy_instanciaConfig(instancia_config * instancia) {
	free(instancia->algoritmo);
	free(instancia->ipCoordi);
	free(instancia->nombre);
	free(instancia->path);
	free(instancia->puertoCoordi);
	free(instancia);

}

************************************* CODIGO ANTERIOR ******************************** ^^^
*/

#include "instancia.h"


int main(void)
{

	logger= crearLogger("loggerInstancia.log","loggerInstancia");
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
	socketCoordinador = 0;

	while(socketCoordinador == 0)
	{
		log_info(logger, "Intento conectarme al Coordinador\n");
		socketCoordinador = conectarseAlServidor(logger,&instanciaConfig->ipCoordi, &instanciaConfig->puertoCoordi);
		if(socketCoordinador == -1)
		{
			log_error(logger, "Conexion fallida, ingresar algo para reintentar\n");
			char texto[10];
			fgets(texto, 10, stdin);
			socketCoordinador = 0;
			break; //Solo para probar, despues aca se puede hacer otra cosa
		}
	}

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

	//config = config_create("configuracionInstancia.config");
	/*instancia->ipCoordi = config_get_string_value(config, "IP_COORDI");
	instancia->puertoCoordi = config_get_string_value(config, "PUERTO_COORDI");
	instancia->algoritmo = config_get_string_value(config, "ALGORITMO_DE_REEMPLAZO");
	instancia->path = config_get_string_value(config, "PATH");
	instancia->nombre = config_get_string_value(config, "NOMBRE");
	instancia->intervalo = config_get_int_value(config, "INTERVALO_DUMP");*/

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
	/*instanciaConfig->ipCoordi = malloc(50);
	instanciaConfig->puertoCoordi = malloc(50);
	instanciaConfig->algoritmo = malloc(50);
	instanciaConfig->path = malloc(100);
	instanciaConfig->nombre = malloc(50);*/

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
	printf("El ip del Coordinador es: %s\n", instancia->ipCoordi);
	printf("El puerto del Coordinador es: %s\n", instancia->puertoCoordi);
	printf("El algoritmo es: %s\n", instancia->algoritmo);
	printf("El path es: %s\n", instancia->path);
	printf("El nombre es: %s\n", instancia->nombre);
	printf("El intervalo es: %d\n", instancia->intervalo);
}

void imprimirConfiguracionDeEntradas()
{
	printf("El tamanio de la entrada es: %d\n", tamanioEntrada);
	printf("La cantidad de entradas es: %d\n", cantidadEntradas);
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

	PROTOCOLO_INSTRUCCIONES sentencia;

	log_info(logger, "Comienzo a recibir sentencias del coordinador\n");

	while(corte) //Habria que ver cuando cortar
	{
		log_info(logger, "Esperando proxima sentencia...\n");

		if(recibirMensaje(logger, sizeof(sentencia), &sentencia, socketCoordinador) <= 0)
		{
			log_error(logger, "No se pudo recibir la sentencia\n");

			return -1;
		}

		switch(sentencia)
		{
			/* No le deberian llegar.
			case INSTRUCCION_GET:
				procesarGET();
				break;
			*/

			case INSTRUCCION_SET:
				procesarSET();
				break;

			case INSTRUCCION_STORE:
				//Mas adelante vemos
				break;

			default:

				log_error(logger, "La sentencia no puede ser interpretada\n");

				return -1;
		}
	}

	return 1;
}

void procesarSET()
{
	int32_t longitudKey, longitudValor = 0;

	log_info(logger, "La sentencia es SET. Pedimos la longitud de la clave\n");

	if(recibirMensaje(logger, sizeof(longitudKey), &longitudKey, socketCoordinador) <= 0)
	{
		log_error(logger, "No se pudo recibir la longitud de la clave\n");
		return;
	}

	log_info(logger, "Longitud de la clave recibida: %d\n", longitudKey);
	log_info(logger, "Esperamos la clave\n");

	char key[longitudKey]; //Suponemos que el Coordinador nos envia la longitud incluyendo el caracter '\0'

	if(recibirMensaje(logger, longitudKey, key, socketCoordinador) <= 0)
	{
		log_error(logger, "No se pudo recibir la clave\n");
		return;
	}

	log_info(logger, "Clave recibida: %s\n", key);
	log_info(logger, "Esperamos la longitud del valor a almacenar\n");

	if(recibirMensaje(logger, sizeof(longitudValor), &longitudValor, socketCoordinador) <= 0)
	{
		log_error(logger, "No se pudo recibir la longitud del valor\n");
		return;
	}

	log_info(logger, "Longitud del valor recibida: %d\n", longitudValor);
	log_info(logger, "Esperamos el valor\n");

	char valor[longitudValor];

	if(recibirMensaje(logger, longitudValor, valor, socketCoordinador) <= 0)
	{
		log_error(logger, "No se pudo recibir el valor\n");
		return;
	}

	log_info(logger, "Valor recibido: %s\n", valor);

	t_tabla_entradas * datos = NULL;

	switch(existeLaClave(key, datos))
	{
		case 0: //La clave no existe
			//Guardamos el valor en las Entradas.
			//Creamos un nuevo nodo en la Tabla de Entradas con su correspondiente clave, numero de entrada y tamanio del valor
			log_info(logger, "La clave no existe. Intentamos guardar su valor en las Entradas\n");
			guardarValorEnEntradas(key, valor, longitudValor);
			log_info(logger, "Se pudo almacenar el valor\n");
			break;

		case 2: //La clave existe y su valor almacenado es atomico
			//Guardamos el valor en las entradas en la posicion que nos indica el nodo
			//Actualizamos el nodo en el que estan los datos de la clave
			log_info(logger, "La clave existe. Se intenta actualizar su valor\n");
			actualizarValorEnEntradas(datos, valor, longitudValor);
			log_info(logger, "Valor actualizado\n");
			break;

		default: //Cuando devuelve 1, quiere decir que el valor almacenado de esa clave no es atomico
			log_error(logger, "El valor de la clave en cuestion no es atomico, por lo que no se puede reemplazar\n");
			return;
	}

	log_info(logger, "Operacion SET exitosa!\n");

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
	double entradasQueOcupa = (double)longitudDelValor / (double)tamanioEntrada;

	return (int) ceil(entradasQueOcupa);
}

void guardarValorEnEntradas(char * clave, char * valor, int32_t longitudDelValor)
{

	if(comenzarReemplazoDeValores)
	{
		//Tengo que implementar algun algoritmo para comenzar a reemplazar
	}


	//Lo guardo en el espacio siguiente libre
	int cuantasEntradasDeboEscribir = cuantasEntradasOcupaElValor(longitudDelValor);
	int cantidadEntradasLibres = cantidadEntradas - tablaEntradas->elements_count;

	if(cuantasEntradasDeboEscribir <= cantidadEntradasLibres) //Sabemos que las entradas son contiguas porque todavia no dimos una vuelta. Siempre almacenamos un valor al lado del otro
	{
		if(cuantasEntradasDeboEscribir == 1)
		{
			escribirValorAtomico(clave, valor, longitudDelValor);
			return;
		}

		char valorPartido[cuantasEntradasDeboEscribir][tamanioEntrada];
		separarStringEnNPartesIguales(valor, longitudDelValor, cuantasEntradasDeboEscribir, tamanioEntrada, valorPartido);

		int parte;

		for(parte = 0; parte < cuantasEntradasDeboEscribir; parte ++)
		{
			escribirValorAtomico(clave, valorPartido[parte], longitudDelValor);
		}

		return;

	}

	//Este seria el caso en el que, por ej, tengo 1 entrada libre (y es la ultima) pero tengo que escribir dos


}

void actualizarValorEnEntradas(t_tabla_entradas * info, char * valor, int32_t longitudDelValor)
{
	strcpy(entradas[info->numeroEntrada], valor);
	info->tamanioValor = longitudDelValor;
	//Capaz tenga que agregar la hora en la que se modifico por ultima vez como un campo en la Tabla de entradas
	//Habria que actualizarlo tambien
}

void separarStringEnNPartesIguales(char * cadena, int longitudCadena, int cantidadPartes, int tamanioParte, char strings[cantidadPartes][tamanioParte])
{

	int caracter, parte, posCadena;
	parte = -1;

	for(posCadena = 0; posCadena < longitudCadena; posCadena ++)
	{
		if(posCadena % tamanioParte == 0)
		{
			parte ++;
			caracter = 0;
		}

		strings[parte][caracter] = cadena[posCadena];
		caracter ++;
	}
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
			printf("La clave es: %s\n", datos->clave);
			printf("Su valor asociado es: %s\n", entradas[datos->numeroEntrada]);
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

			printf("La clave es: %s\n", key);
			printf("Su valor asociado es: %s\n", valorCompleto);

		}
	}

	log_info(logger, "Los valores se han impreso correctamente\n");
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
