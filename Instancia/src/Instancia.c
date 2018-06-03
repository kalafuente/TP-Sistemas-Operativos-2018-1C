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
	procesarSentencias();


	close(socketCoordinador);
	destroy_instanciaConfig(instanciaConfig);
	config_destroy(config);
	eliminarEntradas();


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
	entradas = (char **)malloc(cantidadEntradas * sizeof(char*));

	int fila, columna;

	for(fila = 0; fila < cantidadEntradas; fila ++)
	{
		entradas[fila] = (char*)malloc(tamanioEntrada * sizeof(char));
	}

	for(fila = 0; fila < cantidadEntradas; fila ++)
	{
		for(columna = 0; columna < tamanioEntrada; columna ++)
		{
			entradas[columna][fila] = '0';
		}
	}
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
	return;
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
