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


int main(void) {

	logger= crearLogger("loggerInstancia.log","loggerInstancia");
	t_config * config = config_create("configuracionInstancia.config");
	instanciaConfig = init_instanciaConfig();
	crearConfiguracion(instanciaConfig, config);
	imprimirConfiguracion(instanciaConfig);
	conectarseAlCoordinador();
	recibirConfiguracionDeEntradas();

	close(socketCoordinador);
	destroy_instanciaConfig(instanciaConfig);
	config_destroy(config);

	return 0;

}

void recibirConfiguracionDeEntradas(){
	int cantidadEntradas;
	int tamanio;


	PROTOCOLO_COORDINADOR_A_INSTANCIA entradas;

	if (recibirMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_INSTANCIA),&entradas,socketCoordinador)){

		recibirMensaje(logger,sizeof(int),&cantidadEntradas,socketCoordinador);
		printf("cantidadEntradas: %d\n", cantidadEntradas);
		recibirMensaje(logger,sizeof(int),&tamanio,socketCoordinador);
		printf("tamanio: %d\n", tamanio);
	}



}

void conectarseAlCoordinador(){

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
						break;
					}
				}

	log_info(logger, "Conexion exitosa!");

	PROTOCOLO_COORDINADOR_A_CLIENTES handshakeCoordi;
	recibirMensaje(logger,sizeof(PROTOCOLO_COORDINADOR_A_CLIENTES),&handshakeCoordi,socketCoordinador);
	PROTOCOLO_HANDSHAKE_CLIENTE handshakeINSTANCIA = HANDSHAKE_CONECTAR_INSTANCIA_A_COORDINADOR;
	enviarMensaje(logger,sizeof(PROTOCOLO_HANDSHAKE_CLIENTE),&handshakeINSTANCIA,socketCoordinador);
}


void crearConfiguracion(instancia_config* instancia, t_config* config) {

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
instancia_config * init_instanciaConfig() {

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


void destroy_instanciaConfig(instancia_config * instancia) {
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



