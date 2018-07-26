#include "instancia.h"


int main(int argc,char**argv)
{

	logger= crearLogger("loggerInstancia.log","loggerInstancia");
	log_info(logger, "**************************************** NUEVA ENTRADA ****************************************");
	//t_config * config = config_create("configuracionInstancia.config");
	t_config * config = abrirArchivoConfig(argc,argv,logger,destruirLogger);

	instanciaConfig = init_instanciaConfig(); //CHECK
	crearConfiguracion(instanciaConfig, config); //CHECK
	imprimirConfiguracion(instanciaConfig); //CHECK
	conectarseAlCoordinador(); //CHECK
	handShakeConElCoordinador(); //CHECK, PERO enviarID no CHECK
	recibirConfiguracionDeEntradas(); //CHECK
	imprimirConfiguracionDeEntradas(); //CHECK
	inicializarMutex();
	inicializarEntradas(); //CHECK
	inicializarBitArray(); //CHECK
	tablaEntradas = list_create();
	crearDirectorio();
	reincorporarse();
	crearHiloParaDump();
	procesarSentencias();
	imprimirContenidoEntradas();


	close(socketCoordinador);
	destroy_instanciaConfig(instanciaConfig); //CHECK
	config_destroy(config); //CHECK
	eliminarEntradas(); //CHECK
	eliminarTablaDeEntradas(); //CHECK
	eliminarBitArray(); //CHECK


	return 0;

}

void destruirLogger(){
	log_destroy(logger);
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

	enviarID(socketCoordinador,instanciaConfig->nombre,logger); //Aca le enviamos el nombre de la instancia al Coordi para identificarnos. enviarID NO CHECKEADO
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

	entradas = (char *)malloc(cantidadEntradas * tamanioEntrada * sizeof(char));

	if(entradas == NULL)
	{
		log_error(logger, "Fallo al intentar pedir memoria para las Entradas\n");
		return;
	}

	log_info(logger, "Memoria alojada correctamente\n");
	log_info(logger, "Inicializamos los valores de las Entradas en 0\n");

	int pos;

	for(pos = 0; pos < (cantidadEntradas * tamanioEntrada); pos ++)
	{
			entradas[pos] = '0';
	}

	log_info(logger, "Inicializacion de Entradas exitosa!\n");
}

void inicializarBitArray()
{
	log_info(logger, "Pedimos memoria para el bitArray\n");

	int tamanioArrayDeBits = tamanioBitArray();

	bitArray = (int *) malloc(tamanioArrayDeBits * sizeof(int));

	if(bitArray == NULL)
	{
		log_error(logger, "Fallo al pedir memoria para el Array de Bits\n");
		return;
	}

	log_info(logger, "Memoria alojada correctamente\n");
	log_info(logger, "Inicializamos los valores del Array\n");

	int entero;

	for(entero = 0; entero < tamanioArrayDeBits; entero ++)
	{
		bitArray[entero] = 0;
	}

	log_info(logger, "Posiciones requeridas para el Array: %d", tamanioArrayDeBits);
	log_info(logger, "Inicializacion del Array de Bits exitosa!\n");

}

int tamanioBitArray()
{
	int bitsPorEntero = sizeof(int) * 8;

	return (cantidadEntradas - 1)/bitsPorEntero + 1;
}

void setBit(int numeroEntrada)
{
	int bitsPorEntero = sizeof(int) * 8;
	int i = numeroEntrada/bitsPorEntero;
	int pos = numeroEntrada%bitsPorEntero;

	unsigned int flag = 1;

	flag = flag << pos;

	bitArray[i] = bitArray[i] | flag;
}

void clearBit(int numeroEntrada)
{
	int bitsPorEntero = sizeof(int) * 8;
	int i = numeroEntrada/bitsPorEntero;
	int pos = numeroEntrada%bitsPorEntero;

	unsigned int flag = 1;

	flag = flag << pos;
	flag = ~flag;

	bitArray[i] = bitArray[i] & flag;
}

int testBit(int numeroEntrada)
{
    int bitsPorEntero = sizeof(int) * 8;
	int i = numeroEntrada/bitsPorEntero;
    int pos = numeroEntrada%bitsPorEntero;

    unsigned int flag = 1;

    flag = flag << pos;

    return  bitArray[i] & flag;
}

void eliminarBitArray()
{
	free(bitArray);
	bitArray = NULL;
}

void eliminarEntradas()
{
	free(entradas);
	entradas = NULL;
}

int procesarSentencias()
{
	int corte = 1;

	PROTOCOLO_INSTANCIA_A_COORDINADOR respuesta;

	t_instruccion* sentencia = NULL;

	log_info(logger, "Comienzo a recibir sentencias del coordinador\n");

	while(corte) //Habria que ver cuando cortar
	{
		log_info(logger, "Esperando proxima sentencia...\n");

		sentencia = recibirInstruccion(logger, socketCoordinador, "COORDINADOR");
		//Aca deberia ir un mutex para la mutua exclusion con el DUMP

		if(sentencia == NULL)
		{
			log_error(logger, "No se pudo recibir la sentencia\n");
			respuesta = ERROR_INSTRUCCION;
			corte = 0;
		}
		else
		{

			pthread_mutex_lock(&mutex);

			switch(sentencia->instruccion)
			{
				/* No le deberian llegar.
					case INSTRUCCION_GET:
					procesarGET();
					break;
				 */

				case INSTRUCCION_SET:
					if(procesarSET(sentencia) < 0)
					{
						log_error(logger, "Fallo en la operacion SET\n");
						respuesta = NO_SE_PUDO_GUARDAR_VALOR;
						corte = 0;
						break;
					}
					else
					{
						respuesta = SE_PUDO_GUARDAR_VALOR;
						break;
					}

				case INSTRUCCION_STORE:
					if(procesarSTORE(sentencia) < 0)
					{
						log_error(logger, "Fallo en la operacion STORE\n");
						respuesta = NO_SE_CREO_EL_ARCHIVO;
						corte = 0;
						break;
					}
					else
					{

						log_info(logger, "Operacion STORE exitosa!\n");
						respuesta = SE_CREO_EL_ARCHIVO;
						break;
					}

				case PEDIDO_DE_VALOR:
					peticionValor();
					respuesta = VALOR_ENVIADO;
					break;

				case COMPACTAR:
					//COMPACTACION
					respuesta = COMPACTACION_EXITOSA;
					break;

				default:

					log_error(logger, "La sentencia no puede ser interpretada\n");
					respuesta = ERROR_INSTRUCCION;
					corte = 0;
			}
		}

		enviarMensaje(logger, sizeof(respuesta), &respuesta, socketCoordinador);
		int32_t entradasEnUso = (int32_t) list_size(tablaEntradas);
		enviarMensaje(logger, sizeof(entradasEnUso), &entradasEnUso, socketCoordinador);
		if(sentencia != NULL)
		{
			destruirInstruccion(sentencia);
		}

		pthread_mutex_unlock(&mutex);
	}

	return 1;
}

void peticionValor()
{
	list_sort(tablaEntradas, (void*)ordenarPorNumeroDeEntrada);

	char * clavePeticionada = recibirID(socketCoordinador, logger);

	log_info(logger, "Nos piden el valor de la siguiente clave: %s", clavePeticionada);

	t_link_element * actual = tablaEntradas->head;

	char * valor = string_new();

	int noEncontrado = 1;

	while(actual != NULL && noEncontrado)
	{
		t_tabla_entradas * dato = (t_tabla_entradas *)actual->data;

		if(strcmp(dato->clave, clavePeticionada) == 0)
		{
			string_append(&valor, &entradas[dato->numeroEntrada * tamanioEntrada]);
			noEncontrado = 0;
		}

		actual = actual->next;
	}

	if(noEncontrado)
	{
		string_append(&valor, "null");
	}

	log_info(logger, "Enviando valor: %s", valor);

	enviarID(socketCoordinador, valor, logger);

	free(valor);
	free(clavePeticionada);
}

int procesarSET(t_instruccion* inst)
{
	contadorGlobal ++;

	log_info(logger, "La operacion es SET\n");
	//Aca tengo que ordenar la lista por numero de entrada, de menor a mayor
	list_sort(tablaEntradas, (void*)ordenarPorNumeroDeEntrada);

	t_link_element * nodo = existeLaClave(inst->clave);

	int32_t tamanioNuevoValor = (int32_t) (strlen(inst->valor) + 1);
	int entradasQueOcupaNuevoValor = cuantasEntradasOcupaElValor(tamanioNuevoValor);

	if(nodo == NULL)
	{
		log_info(logger, "La clave no existe\n");

		int cantidadEntradasUsadas = list_size(tablaEntradas);
		int cantidadEntradasLibres = cantidadEntradas - cantidadEntradasUsadas;

		int contEntradasLibres = 0;
		int pos = 0;
		int centinela = 1;

		if(cantidadEntradasLibres >= entradasQueOcupaNuevoValor){ //Llave corrida

			//Tenemos suficientes entradas libres

		log_info(logger, "Hay suficientes entradas libres para almacenar el valor\n");

		//ALGORITMO TURBIO

		log_info(logger, "Empieza ALGORITMO TURBIO\n");

		//Tratamos de buscar que esten todas juntas

		log_info(logger, "Recorremos el bitArray\n");

		while(pos < cantidadEntradas && centinela)
		{
			log_info(logger, "Estamos en la pos: %d", pos);

			if(testBit(pos))
			{
				//Quiere decir que la entrada esta ocupada
				log_info(logger, "La entrada %d esta ocupada, seguimos buscando\n", pos);
				pos++;
			}
			else
			{
				//La entrada esta libre! Tengo que verificar que hayan (entradasQueOcupaNuevoValor - 1) entradas libres contiguas a la misma
				//Aunque primero aumentamos el contador

				log_info(logger, "La entrada %d esta libre\n", pos);
				log_info(logger, "Tenemos que ver que las %d entradas necesarias sean contiguas\n", entradasQueOcupaNuevoValor);

				contEntradasLibres++;

				if(contEntradasLibres == entradasQueOcupaNuevoValor)
				{
					//Son suficientes, entonces salgo del ciclo manteniendo el valor de pos
					log_info(logger, "El valor es atomico y tenemos una entrada libre\n");
					centinela = 0;
					break;
				}

				log_info(logger, "El valor ocupa mas de una entrada. Hay que buscar mas contiguas\n");

				while(contEntradasLibres < entradasQueOcupaNuevoValor && pos < cantidadEntradas && centinela)
				{

					pos++;
					if(testBit(pos))
					{
						//La siguiente esta ocupada. Salgo del 2do while y reseteo el contador de entradas libres
						log_info(logger, "La siguiente esta ocupada. Las entradas libres vuelven a cero y seguimos buscando contiguas\n");

						contEntradasLibres = 0;
						pos++;
						break;
					}
					else
					{
						contEntradasLibres++;

						log_info(logger, "La siguiente esta libre. Sumamos uno a las libres: %d", contEntradasLibres);

						if(contEntradasLibres == entradasQueOcupaNuevoValor)
						{
							//Son suficientes, entonces salgo del ciclo manteniendo el valor de pos
							log_info(logger, "Las %d entradas libres alcanzan y estan juntas\n", contEntradasLibres);
							centinela = 0;
							break;
						}
						else
						{
							//No son suficientes. No hago nada porque al principio del while avanza la posicion
							log_info(logger, "Todavia no alcanzan las %d libres. Seguimos buscando\n", contEntradasLibres);
						}
					}
				}

			}

		}

		//Hay que corroborar por que motivo salimos del ciclo.

		log_info(logger, "Salimos del ciclo. Hay que verificar por que\n");

		if(pos >= cantidadEntradas)
		{
			//Significa que no hay suficientes entradas JUNTAS libres.
			//Deberiamos buscar las primeras n entradas libres que necesitemos
			//Sabemos que existen, asi que al encontrarlas deberemos compactar
			// PARA QUE LAS QUIERO ENCONTRAR SI YA SE QUE HAY QUE COMPACTAR?

			/*
			int entradasLibres[entradasQueOcupaNuevoValor];
			pos = 0;
			contEntradasLibres = 0;

			while(pos < cantidadEntradas && contEntradasLibres < entradasQueOcupaNuevoValor)
			{
				if(testBit(pos))
				{
					//Esta ocupada. No hacemos nada
				}
				else
				{
					//Hay una libre. La guardamos
					entradasLibres[contEntradasLibres] = pos;
					contEntradasLibres ++;
				}

				pos++;
			}

			//Tengo que saber por que sali del ciclo

			if(pos >= cantidadEntradas)
			{
				//No es posible que esto suceda. Corroboramos antes con las entradas usadas que habia espacio suficiente
				perror("Error inesperado. Pareciera que no hay entradas en las que guardar. Probablemente el Algoritmo no funciona bien\n");
				return -10;
			}
			else
			{
				//Encontramos todas las entradas libres necesarias, pero no estan JUNTAS
				// ************* COMPACTACION ***************
			}

			*/

			log_info(logger, "Hay suficientes entradas libres pero no estan JUNTAS\n");
			log_error(logger, "Hay que COMPACTAR. ROMPE TODO\n");


			//********** compactacion derecho ****************

			PROTOCOLO_INSTANCIA_A_COORDINADOR respuesta = SE_NECESITA_COMPACTAR;
			enviarMensaje(logger, sizeof(respuesta), &respuesta, socketCoordinador);
			int32_t entradasEnUso = (int32_t) list_size(tablaEntradas);
			enviarMensaje(logger, sizeof(entradasEnUso), &entradasEnUso, socketCoordinador);

			t_instruccion * sentencia = NULL;
			sentencia = recibirInstruccion(logger, socketCoordinador, "COORDINADOR");

			if(sentencia->instruccion != COMPACTAR)
			{
				return -42;
			}

			destruirInstruccion(sentencia);
			//ACA COMPACTARIA Y GUARDARIA EL VALOR EN LA NUEVA ENTRADA LIBRE Y QUE SIGA NOMA

			respuesta = COMPACTACION_EXITOSA;
			enviarMensaje(logger, sizeof(respuesta), &respuesta, socketCoordinador);
			entradasEnUso = (int32_t) list_size(tablaEntradas);
			enviarMensaje(logger, sizeof(entradasEnUso), &entradasEnUso, socketCoordinador);
		}
		else
		{
			//Tenemos las entradas libres CONTIGUAS suficientes para guardar el nuevo valor.
			//pos apunta a la ultima entrada libre del conjunto.

			log_info(logger, "Tenemos suficientes entradas CONTIGUAS libres disponibles\n");
			pos = pos - (entradasQueOcupaNuevoValor - 1);
			log_info(logger, "La posicion actual es: %d", pos);
			log_info(logger, "El tamanio del valor a guardar es: %d", tamanioNuevoValor);
			log_info(logger, "El valor ocupa %d entradas\n", entradasQueOcupaNuevoValor);

			//Ahora tenemos la posicion de la primera entrada libre
			log_info(logger, "Se procede a guardar el valor en las entradas\n");
			guardarValorEnEntradas(inst->clave, inst->valor, pos);

		}
	}
		else
		{
			//No hay suficientes entradas libres. Debemos lanzar el Algoritmo de Reemplazo
			//Tenemos que guardar las entradas libres en un array para desp saber si estan contiguas con las liberadas por el algoritmo.
			log_info(logger, "No tenemos suficientes entradas libres. Hay que reemplazar existentes\n");
			log_info(logger, "Cantidad de entradas que ocupa el valor: %d", entradasQueOcupaNuevoValor);
			log_info(logger, "Entradas libres encontradas: %d", contEntradasLibres);

			int cantidadReemplazos = entradasQueOcupaNuevoValor - contEntradasLibres;

			log_info(logger, "Cantidad de entradas a reemplazar: %d", cantidadReemplazos);
			int libres[cantidadReemplazos];
			int j, encontradas = 0;

			log_info(logger, "Tenemos que buscar otra vez las entradas libres y guardarlas\n");

			while(encontradas < cantidadEntradasLibres && j < cantidadEntradas)
			{
				if(testBit(j))
				{
					//Esta ocupada
					log_info(logger, "La entrada esta ocupada. Seguimos\n");
				}
				else
				{
					log_info(logger, "La entrada esta libre. La guardamos y aumentamos las encontradas\n");
					libres[encontradas] = j;
					encontradas ++;
				}

				j++;
			}

			//Ya guardamos las entradas libres, ahora a buscar las que quedan
			log_info(logger, "Entradas libres guardadas. Ahora buscamos las que tenemos que reemplazar\n");
			int limite = encontradas; //Nos indica a partir de que posicion del array guardamos las entradas que se tienen que reemplazar. No las libres

			while(cantidadReemplazos > 0)
			{
				log_info(logger, "Buscamos la victima segun el algoritmo\n");
				int victima = eleccionDeVictima();

				if(victima < 0)
				{
					//Quiere decir que no pudo encontrar un valor para reemplazar porque no hay mas valores atomicos
					log_info(logger, "No hay mas victimas posibles. Buscar otra Instancia\n");
					return -1;
				}

				log_info(logger, "La encontramos. La guardamos en el array\n");

				libres[encontradas] = victima;
				encontradas ++;
				cantidadReemplazos --;
			}

			//Si salimos del while es porque pudimos encontrar suficientes valores para reemplazar
			//Tenemos que liberar dichos valores
			//O verificar que sean contiguos y despues decidir que hacer -- no me conviene, no puedo usar algunas funciones
			//NOTA: EL ALGORITMO PUEDE REORDENAR LA LISTA Y LA NECESITAMOS POR NUMERO DE ENTRADA

			log_info(logger, "Encontramos suficientes valores para reempalzar");

			if(strcmp(instanciaConfig->algoritmo, "CIRC") != 0)
			{
				list_sort(tablaEntradas, (void*)ordenarPorNumeroDeEntrada);
			}

			t_link_element * actual = tablaEntradas->head;
			t_link_element * anterior = NULL;

			log_info(logger, "Hay que eliminar los nodos victima");

			while(actual != NULL && limite < encontradas)
			{
				t_tabla_entradas * datos = (t_tabla_entradas *)actual->data;

				if(datos->numeroEntrada == libres[limite])
				{
					//Encontramos el nodo. Debemos liberarlo y linkearlo

					log_info(logger, "Encontramos el nodo");

					if(strcmp(instanciaConfig->algoritmo, "CIRC") == 0)
					{

						if(punteroReempAlgCirc == actual)
						{
							moverPunteroReempAlgCirc();
						}
					}

					int entradaActual = datos->numeroEntrada; //Para actualizar el bitArray
					log_info(logger, "Eliminamos el campo data del nodo");
					eliminarDatosTablaDeEntradas((void *) datos);
					if(actual == tablaEntradas->head)
					{
						log_info(logger, "Es la cabecera de la lista");
						tablaEntradas->head = actual->next;
						anterior = actual;
						actual = actual->next;
						anterior->next = NULL;
						free(anterior);
						log_info(logger, "Cosa turbia no rompio (linkeo)");
					}
					else
					{
						anterior->next = actual->next; //linkeo
						actual->next = NULL;
						free(actual);
						actual = anterior->next;
						log_info(logger, "Cosa turbia 2 no rompio (linkeo)");
					}

					clearBit(entradaActual);
					log_info(logger, "Actualizamos el array de bits");
					limite ++;

				}
				else
				{
					anterior= actual;
					actual = actual->next;
				}
			}

			//Ya estan todas liberadas
			//Hay que verificar si son todas contiguas
			log_info(logger, "Ya las liberamos todas. Hay que ver que sean contiguas");

			if(sonEntradasContiguas(encontradas, libres))
			{
				//Son contiguas! Guardamos el valor y todo lo demas
				//Sabemos que estan ordenados por la funcion anterior

				log_info(logger, "Son contiguas. Se suponen ordenadas por la funcion");
				log_info(logger, "Intentamos guardar el valor en la primera entrada del conjunto");

				guardarValorEnEntradas(inst->clave, inst->valor, libres[0]);

				log_info(logger, "Almacenamiento exitoso");
			}
			else
			{
				//Ya tenemos todo lo que necesitamos, pero no son contiguas

				log_info(logger, "Tenemos todo pero no son contiguas");
				log_error(logger, "HAY QUE COMPACTAR. ROMPE TODO");
				//************* COMPACTACION ***************

				PROTOCOLO_INSTANCIA_A_COORDINADOR respuesta = SE_NECESITA_COMPACTAR;
				enviarMensaje(logger, sizeof(respuesta), &respuesta, socketCoordinador);
				int32_t entradasEnUso = (int32_t) list_size(tablaEntradas);
				enviarMensaje(logger, sizeof(entradasEnUso), &entradasEnUso, socketCoordinador);

				t_instruccion * sentencia = NULL;
				sentencia = recibirInstruccion(logger, socketCoordinador, "COORDINADOR");

				if(sentencia->instruccion != COMPACTAR)
				{
					return -42;
				}

				destruirInstruccion(sentencia);

				//ACA COMPACTARIA Y GUARDARIA EL VALOR EN LA NUEVA ENTRADA LIBRE Y QUE SIGA NOMA

				respuesta = COMPACTACION_EXITOSA;
				enviarMensaje(logger, sizeof(respuesta), &respuesta, socketCoordinador);
				entradasEnUso = (int32_t) list_size(tablaEntradas);
				enviarMensaje(logger, sizeof(entradasEnUso), &entradasEnUso, socketCoordinador);
			}

		}

	}
	else
	{
		log_info(logger, "La clave existe\n");
		t_tabla_entradas * datos = (t_tabla_entradas *) nodo->data;
		int entradasQueOcupaViejoValor = cuantasEntradasOcupaElValor(datos->tamanioValor);

		if(entradasQueOcupaViejoValor < entradasQueOcupaNuevoValor)
		{
			//Quiere decir que se necesitan mas entradas para almacenar el nuevo valor. No alcanza con las que tenia antes, lo que no se puede hacer
			//Hay que avisar al Coordi que el nuevo valor no entra en las entradas asignadas al anterior
			return -2;
		}
		else
		{
			//El valor se puede reemplazar, ya sea que ocupen la misma cantidad o el nuevo valor ocupe menos
			log_info(logger, "El valor se puede reemplazar por el nuevo\n");
			actualizarValorEnEntradas(nodo, inst->valor, entradasQueOcupaViejoValor, entradasQueOcupaNuevoValor);
		}

	}

	log_info(logger, "Operacion SET exitosa!\n");
	//Enviar un mensaje al coordinador avisando de que se guardo todo bien?

	return 1;
}

void guardarValorEnEntradas(char * clave, char * valor, int posicionInicial)
{
	//Guardamos el valor primero
	almacenarValor(posicionInicial, valor);
	int32_t tamanioValor = (int32_t) strlen(valor) + 1;
	int32_t tamanioRestante = tamanioValor;

	while(tamanioRestante >= 0)
	{
		tamanioRestante -= tamanioEntrada;
		//Actualizamos el bitArray
		setBit(posicionInicial);
		crearyAgregarElementoTDE(clave, tamanioValor, (int32_t)posicionInicial);
		posicionInicial ++;
	}

}

void crearyAgregarElementoTDE(char * clave, int32_t tamanioValor, int32_t numeroEntrada)
{

	t_tabla_entradas * nuevoCampoData = (t_tabla_entradas *) malloc(sizeof(t_tabla_entradas));
	nuevoCampoData->clave = string_new();
	string_append(&(nuevoCampoData->clave), clave);
	nuevoCampoData->momentoReferencia = contadorGlobal;
	nuevoCampoData->numeroEntrada = numeroEntrada;
	nuevoCampoData->tamanioValor = tamanioValor;
	list_add(tablaEntradas, nuevoCampoData);

	if(list_size(tablaEntradas) == 1)
	{
		punteroReempAlgCirc = tablaEntradas->head;
	}
}

int eleccionDeVictima()
{
	if(strcmp(instanciaConfig->algoritmo, "CIRC") == 0)
	{
		log_info(logger, "Se reemplazara por Algoritmo CIRC\n");
		return victimaCIRC();
	}
	else
	{
		if(strcmp(instanciaConfig->algoritmo, "LRU") == 0)
		{
			log_info(logger, "Se reemplazara por Algoritmo LRU\n");
			return victimaLRU();
		}
		else
		{
			if(strcmp(instanciaConfig->algoritmo, "BSU") == 0)
			{
				log_info(logger, "Se reemplazara por Algoritmo BSU\n");
				return victimaBSU();
			}
			else
			{
				log_error(logger, "Algoritmo incompatible. Se debe revisar la configuracion inicial\n");
				return -5;
			}
		}
	}
}

int victimaCIRC()
{
	 t_link_element * posicionInicial = punteroReempAlgCirc;

	 if(punteroReempAlgCirc == NULL)
	 {
		 log_error(logger, "El puntero de Reemplazo por Alg CIRC es NULO!\n");
		 perror("Se viene SF");
	 }

	do
	{
		//Verificamos si el valor es atomico
		t_tabla_entradas * dato = (t_tabla_entradas *)punteroReempAlgCirc->data;
		if(cuantasEntradasOcupaElValor(dato->tamanioValor) == 1)
		{
			//Es atomico! Lo podemos reemplazar
			return dato->numeroEntrada;
		}

		moverPunteroReempAlgCirc();

	}while(posicionInicial != punteroReempAlgCirc);

	//Dio toda una vuelta. No hay entradas atomicas para reemplazar

	return -1;
}

int victimaLRU()
{
	list_sort(tablaEntradas, (void*)ordenarPorMomentoDeReferencia);

	//PUEDEN EMPATAR SI FUERON RECUPERADOR POR LOS ARCHIVOS DEL DUMP

	t_link_element * actual = tablaEntradas->head;

	int posiblesVictimas[cantidadEntradas];
	int cantVictimas = 0;
	int victima;

	while(actual != NULL)
	{

		t_tabla_entradas * dato = (t_tabla_entradas *)actual->data;

		if(1 != cuantasEntradasOcupaElValor(dato->tamanioValor))
		{
			//No es atomico. Hay que ir al siguiente
			actual = actual->next;
		}
		else
		{
			//Es atomico

			victima = dato->numeroEntrada;

			if(actual->next == NULL)
			{
				return victima;
			}

			t_link_element * siguiente = actual->next;
			t_tabla_entradas * datoSig = (t_tabla_entradas *)siguiente->data;

			if(1 != cuantasEntradasOcupaElValor(datoSig->tamanioValor))
			{
				return victima;
			}

			//El siguiente tambien es atomico. Hay que checkear el momento de referencia

			if(dato->momentoReferencia != datoSig->momentoReferencia)
			{
				return victima;
			}

			//En este caso ambos son atomicos y ambos fueron referenciados en el mismo instante
			//Hay que buscar si hay mas

			posiblesVictimas[cantVictimas] = victima;
			cantVictimas++;
			posiblesVictimas[cantVictimas] = datoSig->numeroEntrada;
			cantVictimas++;

			siguiente = siguiente->next;

			while(siguiente != NULL)
			{
				datoSig = (t_tabla_entradas *)siguiente->data;

				if(1 != cuantasEntradasOcupaElValor(datoSig->tamanioValor))
				{
					//No es atomico. Paramos de buscar porque no hay mas victimas
					break;
				}

				if(dato->momentoReferencia != datoSig->momentoReferencia)
				{
					//No es el mismo valor de ref. No hay mas victimas posibles
					break;
				}

				//Hay una posible victima mas. La agregamos al array

				posiblesVictimas[cantVictimas] = datoSig->numeroEntrada;
				cantVictimas++;
				siguiente = siguiente->next;

			}

			//Tenemos todas las victimas posibles. Hay que ordenarlas y ver cual esta mas cerca del puntero de reempl CIRC

			ordenarArray(cantVictimas, posiblesVictimas);

			return desempatePorCIRC(cantVictimas, posiblesVictimas);

		}
	}

	return -1;
}

int victimaBSU()
{
	list_sort(tablaEntradas, (void*)ordenarPorEspacioUsado);

	t_link_element * actual = tablaEntradas->head;

	int posiblesVictimas[cantidadEntradas];
	int cantVictimas = 0;
	int victima;

	while(actual != NULL)
	{

		t_tabla_entradas * dato = (t_tabla_entradas *)actual->data;

		if(1 != cuantasEntradasOcupaElValor(dato->tamanioValor))
		{
			//Si no es atomico, al estar ordenados por atomicidad quiere decir que no hay valores.
			return -1;
		}
		else
		{
			//Es atomico

			victima = dato->numeroEntrada;

			if(actual->next == NULL)
			{
				return victima;
			}

			t_link_element * siguiente = actual->next;
			t_tabla_entradas * datoSig = (t_tabla_entradas *)siguiente->data;

			if(1 != cuantasEntradasOcupaElValor(datoSig->tamanioValor))
			{
				return victima;
			}

			//El siguiente tambien es atomico. Hay que checkear el espacio usado

			if(dato->tamanioValor != datoSig->tamanioValor)
			{
				return victima;
			}

			//En este caso ambos son atomicos y ambos ocupan el mismo espacio
			//Hay que buscar si hay mas

			posiblesVictimas[cantVictimas] = victima;
			cantVictimas++;
			posiblesVictimas[cantVictimas] = datoSig->numeroEntrada;
			cantVictimas++;

			siguiente = siguiente->next;

			while(siguiente != NULL)
			{
				datoSig = (t_tabla_entradas *)siguiente->data;

				if(1 != cuantasEntradasOcupaElValor(datoSig->tamanioValor))
				{
					//No es atomico. Paramos de buscar porque no hay mas victimas
					break;
				}

				if(dato->tamanioValor != datoSig->tamanioValor)
				{
					//No es el mismo espacio usado. No hay mas victimas posibles
					break;
				}

				//Hay una posible victima mas. La agregamos al array

				posiblesVictimas[cantVictimas] = datoSig->numeroEntrada;
				cantVictimas++;
				siguiente = siguiente->next;

			}

			//Tenemos todas las victimas posibles. Hay que ordenarlas y ver cual esta mas cerca del puntero de reempl CIRC

			ordenarArray(cantVictimas, posiblesVictimas);

			return desempatePorCIRC(cantVictimas, posiblesVictimas);

		}
	}

	return -1;

}

int desempatePorCIRC(int tamanio, int array[tamanio])
{
	int i = 0;
	int entradaCIRC = ((t_tabla_entradas *)punteroReempAlgCirc->data)->numeroEntrada;

	while(i < tamanio)
	{
		if(array[i] < entradaCIRC)
		{
			i++;
		}
		else
		{
			return array[i];
		}
	}

	//Todas las entradas estan antes, entonces devolvemos la primera del array

	return array[0];
}

void ordenarArray(int tamanio, int array[tamanio])
{
	int i, j, auxiliar;

	for(i = 0; i < tamanio; i ++)
	{
		for(j = 1; j < (tamanio - 1); j ++)
		{
			if(array[j] < array[j-1])
			{
				auxiliar = array[j-1];
				array[j-1] = array[j];
				array[j] =  auxiliar;
			}
		}
	}
}

int sonEntradasContiguas(int cantidad, int entradasParaComprobar[cantidad])
{
	//Primero las tengo que ordenar - Bubble Sort porque es facil... (y tampoco va a ser muy grande el array)

	ordenarArray(cantidad, entradasParaComprobar);

	int i = 0;

	while(i < (cantidad - 2))
	{
		if(entradasParaComprobar[i + 1] == (entradasParaComprobar[i] + 1))
		{
			//La segunda es el siguiente de la primera
			i++;
		}
		else
		{
			//No son contiguas
			return 0;
		}
	}

	return 1;
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


t_link_element * existeLaClave(char * clave) //Si existe la clave devuelve un puntero al nodo de la lista
{
	t_link_element * actual = tablaEntradas->head;

	while(actual != NULL)
	{
		t_tabla_entradas * datos = (t_tabla_entradas *) actual->data;

		if(strcmp(clave, datos->clave) == 0) //Encontramos el primero. Devolvemos el puntero al mismo y si no es atomico podemos ir a los siguientes
		{
			return actual;
		}

		actual = actual->next;
	}

	return NULL;
}

/*

int esAtomicoElValor(int32_t longitudDelValor)
{
	if(cuantasEntradasOcupaElValor(longitudDelValor) - 1)
	{
		return 0;
	}

	return 1;
}
*/

int cuantasEntradasOcupaElValor(int32_t longitudDelValor)
{
	double entradasQueOcupa = (double)(longitudDelValor) / (double)tamanioEntrada;

	return (int) ceil(entradasQueOcupa);
}

/*
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

	log_error(logger, "Hay que guardar en entradas libres y a la vez reemplazar usadas para que entre el valor. No contemplado. Ver\n");

	return -5;


}
*/

//******************************* PROBABLEMENTE NO FUNCIONE COMO ESPERABA *******************************
//La lista debe estar ordenada por numero de entrada
void actualizarValorEnEntradas(t_link_element * nodo, char * nuevoValor, int entradasViejoValor, int entradasNuevoValor)
{
	log_info(logger, "Actualizando valor y estructuras administrativas\n");

	t_link_element * actual = nodo;
	t_link_element * anterior = NULL;
	int i;
	int entradaInicial = ((t_tabla_entradas *) nodo->data)->numeroEntrada;

	for(i = 0; i < entradasNuevoValor; i++)
	{
		t_tabla_entradas * datos = (t_tabla_entradas *) actual->data;
		datos->tamanioValor = strlen(nuevoValor) + 1;
		datos->momentoReferencia = contadorGlobal;
		anterior = actual;
		actual = actual->next;
	}

	for(; i < entradasViejoValor; i++)
	{
		if(punteroReempAlgCirc == actual)
		{
			moverPunteroReempAlgCirc();
		}

		t_tabla_entradas * datos = (t_tabla_entradas *) actual->data;
		int entradaActual = datos->numeroEntrada; //Para actualizar el bitArray
		eliminarDatosTablaDeEntradas((void *) datos);
		anterior->next = actual->next; //linkeo
		actual->next = NULL;
		free(actual);
		actual = anterior->next;
		clearBit(entradaActual);
	}

	almacenarValor(entradaInicial, nuevoValor);

	log_info(logger, "Valor almacenado correctamente\n");

}

void almacenarValor(int entradaInicial, char * valor)
{
	int posicion = entradaInicial * tamanioEntrada;
	strcpy(&entradas[posicion], valor);
}

/* NO LA NECESITO MAS
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

*/

/* NO SE USA
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
*/

/*
void moverPunteroAlgCIRC()
{
	if(punteroAlgCIRC == (cantidadEntradas - 1))
	{
		punteroAlgCIRC = 0;
		return;
	}

	punteroAlgCIRC ++;
}
*/

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

		log_info(logger, "La clave es: %s\n", datos->clave);
		log_info(logger, "Su valor asociado es: %s\n", &entradas[datos->numeroEntrada * tamanioEntrada]);

		t_tabla_entradas * siguiente = datos;

		while(lista != NULL && (strcmp(siguiente->clave, datos->clave) == 0))
		{
			lista = lista->next;
			siguiente = ((t_tabla_entradas *)lista->data);
		}

	}

	log_info(logger, "Los valores se han impreso correctamente\n");
}

/* NO SE USA
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
*/

/* NO SE USA
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

*/

void moverPunteroReempAlgCirc()
{
	//Cuando usamos esta funcion sabemos que la lista esta ordenada por numero entrada de menor a mayor
	if(punteroReempAlgCirc->next == NULL) //Quiere decir que esta en el ultimo elemento
	{
		punteroReempAlgCirc = tablaEntradas->head;
		return;
	}

	punteroReempAlgCirc = punteroReempAlgCirc->next;
}

/* OTRA ACTUALIZADA
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
*/

/* NO SE USA
******************************* PROBABLEMENTE NO FUNCIONE COMO ESPERABA *******************************
void reemplazarValorAtomico(t_tabla_entradas * dato, char * clave, char * valor, int32_t longitudValor)
{
	strcpy(entradas[filaACambiar], valor);
	dato->clave = realloc(dato->clave, strlen(clave) + 1);
	strcpy(dato->clave, clave);
	dato->numeroEntrada = filaACambiar;
	dato->tamanioValor = longitudValor;
}
*/

int procesarSTORE(t_instruccion * sentencia)
{
	contadorGlobal ++;
	list_sort(tablaEntradas, (void*)ordenarPorNumeroDeEntrada);

	log_info(logger, "La operacion es STORE\n");

	log_info(logger, "Verificamos que la clave exista en la Tabla de Entradas\n");

	t_link_element * nodito = existeLaClave(sentencia->clave);

	if(nodito == NULL)
	{
		log_error(logger, "LA CLAVE NO EXISTE. HA SIDO REEMPLAZADA. AVISAR COORDI. ABORTAR ESI?\n");
		log_error(logger, "La clave recibida es: %s", sentencia->clave);
		return -1;
	}


	t_tabla_entradas * dato = (t_tabla_entradas *)nodito->data;

	//Hay que actualizar el momento de referencia para la clave en cuestion

	char * centinela = string_new();
	string_append(&centinela, dato->clave);

	t_tabla_entradas * datito = dato;

	while(nodito != NULL && strcmp(datito->clave, centinela) == 0)
	{
		datito = (t_tabla_entradas *)nodito->data;
		datito->momentoReferencia = contadorGlobal;
		nodito = nodito->next;
	}

	free(centinela);
	//centinela = NULL;

	log_info(logger, "La clave existe!\n");

	if(almacenarArchivo(instanciaConfig->path, sentencia->clave, dato->tamanioValor, dato->numeroEntrada) < 0)
	{
		log_error(logger, "No se pudo guardar el Archivo\n");
		return -10;
	}

	return 1;

}

int almacenarArchivo(char * pathAbsoluto, char * clave, int32_t tamanioValor, int32_t numeroEntrada)
{
	char * nombreDelArchivo = string_new();
	string_append(&nombreDelArchivo, pathAbsoluto); //Si lo usa el DUMP envia el path, si lo usa el STORE envia un string vacio ""
	string_append(&nombreDelArchivo, clave);
	string_append(&nombreDelArchivo, ".txt");

	int fd = open(nombreDelArchivo, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO); //Abrimos el archivo con todos los permisos para todos (777), para lectura y escritura, y le decimos que lo cree

	if(fd < 0)
	{
		log_error(logger, "No se pudo abrir o crear el archivo. Fallo el open()\n");
		log_error(logger, "Nombre archivo: %s", nombreDelArchivo);
		printf("Valor de errno: %d\n", errno);
		free(nombreDelArchivo);
		return -1;
	}

	if(ftruncate(fd, tamanioValor) < 0)
	{
		log_error(logger, "No se pudo establecer el tamanio del archivo. Fallo ftruncate()\n");
		free(nombreDelArchivo);
		close(fd);
		return -2;
	}

	char * pointermmap = mmap(NULL, tamanioValor, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(pointermmap == MAP_FAILED)
	{
		log_error(logger, "No se pudo mapear a memoria. Fallo mmap()\n");
		free(nombreDelArchivo);
		close(fd);
		return -3;
	}

	strcpy(pointermmap, &entradas[numeroEntrada * tamanioEntrada]);
	close(fd);
	free(nombreDelArchivo);

	log_info(logger, "Se pudo mapear correctamente a memoria\n");

	return 1;
}

void crearHiloParaDump()
{
	pthread_t thread_id;

	if (pthread_create(&thread_id, NULL, DUMP, NULL) < 0) {
		log_error(logger, "No se pudo crear el hilo");
	}

}

void * DUMP()
{
	while(1)
	{
		sleep(instanciaConfig->intervalo);
		pthread_mutex_lock(&mutex);

		log_info(logger, "Comienza el DUMP\n");

		list_sort(tablaEntradas, (void*)ordenarPorNumeroDeEntrada);

		t_link_element * actual = tablaEntradas->head;

		while(actual != NULL)
		{
			t_tabla_entradas * dato = (t_tabla_entradas *)actual->data;

			char * claveActual = string_new();
			string_append(&claveActual, dato->clave);

			almacenarArchivo(instanciaConfig->path, claveActual, dato->tamanioValor, dato->numeroEntrada);

			while(actual != NULL && (strcmp(claveActual, dato->clave) == 0))
			{
				dato = (t_tabla_entradas *)actual->data;
				actual = actual->next;
			}

			free(claveActual);
		}

		log_info(logger, "Finalizo el DUMP\n");

		pthread_mutex_unlock(&mutex);

	}


	return NULL;
}

void crearDirectorio()
{
	struct stat st = {0};

	if (stat(instanciaConfig->path, &st) == -1)
	{
	    log_info(logger, "El directorio de montaje no existe, se creara\n");

	    if(mkdir(instanciaConfig->path, S_IRWXU | S_IRWXG | S_IRWXO) < 0)
	    {
	    	log_error(logger, "No se pudo crear el directorio\n");
	    	return;
	    }

	    log_info(logger, "mkdir exitoso\n");
	}
}

void reincorporarse()
{
	// --------- PIDO LAS CLAVES AL COORDINADOR ------------

	PROTOCOLO_INSTANCIA_A_COORDINADOR pedidoClaves = PEDIDO_DE_CLAVES;

	enviarMensaje(logger, sizeof(pedidoClaves), &pedidoClaves, socketCoordinador);

	char * clave = recibirID(socketCoordinador, logger);
	int pos = 0;

	if(strcmp(clave, "null") == 0)
	{
		log_info(logger, "No hay nada que recuperar\n");
		free(clave);
		return;
	}

	while(strcmp(clave, "null") != 0)
	{
		//char * key = recibirID(socketCoordinador, logger); //Necesito que quede igual para guardarlo despues
		char * nombreCompleto = string_new();
		string_append(&nombreCompleto, clave);
		string_append(&nombreCompleto, ".txt");

		DIR *dir;
		struct dirent *ent;

		if ((dir = opendir (instanciaConfig->path)) != NULL)
		{
		  //El directorio existe, vamos a ir recorriendo todo lo que hay en el

		  while ((ent = readdir (dir)) != NULL)
		  {

			  if(strcmp(ent->d_name, nombreCompleto) != 0)
			  {
				  //Este no es el archivo que estoy buscando. Seguimos recorriendo el directorio
			  }
			  else
			  {
				//Encontramos el archivo. Hay que abrirlo y guardar su contenido
				  char * rutaAbsoluta = string_new();
				  string_append(&rutaAbsoluta, instanciaConfig->path);
				  string_append(&rutaAbsoluta, nombreCompleto);

				FILE *fp = fopen(rutaAbsoluta, "r+"); //Hay que pasarle el path absoluto.

				if(fp == NULL)
				{
					log_error(logger, "No se pudo abrir el archivo");
					free(clave);
					free(nombreCompleto);
					free(rutaAbsoluta);
					closedir(dir);
					return;
				}

				fseek(fp, 0, SEEK_END);
				int32_t longitudValor = ftell(fp);
				char * valor = (char *) malloc(longitudValor * sizeof(char));
				fseek(fp, 0, SEEK_SET);
				fread(valor, sizeof(char), longitudValor, fp);

				guardarValorEnEntradas(clave, valor, pos);
				pos += cuantasEntradasOcupaElValor(longitudValor);

				free(nombreCompleto);
				free(rutaAbsoluta);
				free(valor);
				fclose(fp);

				break;

			  }

		  }

		  closedir (dir);

		}
		else
		{
		  perror ("No se puede abrir el directorio\n");
		  return;
		}


		char * key = clave;
		clave =recibirID(socketCoordinador, logger);
		free(key);
	}

	// -----------------------------------------------------

	log_info(logger, "Reincorporacion terminada\n");

}

void inicializarMutex()
{
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        log_info(logger, "Fallo en la inicializacion del mutex\n");
    }
}

bool ordenarPorNumeroDeEntrada(t_tabla_entradas * primerElemento, t_tabla_entradas * segundoElemento)
{
	return (primerElemento->numeroEntrada < segundoElemento->numeroEntrada);
}

bool ordenarPorMomentoDeReferencia(t_tabla_entradas * primerElemento, t_tabla_entradas * segundoElemento)
{
	//return (primerElemento->momentoReferencia < segundoElemento->momentoReferencia);

	if(primerElemento->momentoReferencia < segundoElemento->momentoReferencia)
	{
		return true;
	}

	if(primerElemento->momentoReferencia == segundoElemento->momentoReferencia)
	{
		return (1 == cuantasEntradasOcupaElValor(primerElemento->tamanioValor));
	}

	return false;
}

bool ordenarPorEspacioUsado(t_tabla_entradas * primerElemento, t_tabla_entradas * segundoElemento)
{
	if(1 != cuantasEntradasOcupaElValor(primerElemento->tamanioValor))
	{
		return false;
	}

	if(1 != cuantasEntradasOcupaElValor(segundoElemento->tamanioValor))
	{
		return true;
	}

	return (primerElemento->tamanioValor > segundoElemento->tamanioValor);
}



