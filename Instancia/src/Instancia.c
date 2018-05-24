#include "instancia.h"


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

