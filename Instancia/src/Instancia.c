#include "instancia.h"

#define PACKAGESIZE 1024

int main(void) {

	t_config * config = malloc(sizeof(t_config));
/*	char * ipCoordi = malloc(sizeof(char) * 6);
	char * puertoCoordi = malloc(sizeof(char) * 4);
	char * algoritmo = malloc(sizeof(char) * 4);
	char * path = malloc(sizeof(char) * 23);
	char * nombre = malloc(sizeof(char) * 10);
	int intervalo;

	crearConfiguracion(&ipCoordi, &puertoCoordi, &algoritmo, &path, &nombre,
			&intervalo, &config);
	int serverSocket;
	serverSocket = conectarseAlServidor(&ipCoordi, &puertoCoordi);
*/

	instancia_config * instanciaConfig = init_instanciaConfig();
	crearConfiguracion2(&instanciaConfig, &config);
	int serverSocket = conectarseAlServidor(&instanciaConfig->ipCoordi,
			&instanciaConfig->puertoCoordi);

	int enviar = 1;
	printf("Conectado al servidor.");
	while (enviar) {
		recibirmensaje(serverSocket);
		enviarmensaje("nos conectamos, soy instancia :)", serverSocket);
		char mensaje[PACKAGESIZE];
		while (1) {
			fgets(mensaje, PACKAGESIZE, stdin);
			enviarmensaje(mensaje, serverSocket);
		}
		enviar = 0;
	}

	printf("\n termine\n");
	/*free(ipCoordi);
	free(puertoCoordi);
	free(algoritmo);
	free(path);
	free(nombre);
*/
	destroy_instanciaConfig(instanciaConfig);
	config_destroy(config);

	return 0;

}

void crearConfiguracion(char ** ipCoordi, char ** puertoCoordi,
		char ** algoritmo, char ** path, char ** nombre, int * intervalo,
		t_config ** config) {
	*config = config_create("configuracionInstancia.config");
	*ipCoordi = config_get_string_value(*config, "IP_COORDI");
	*puertoCoordi = config_get_string_value(*config, "PUERTO_COORDI");
	*algoritmo = config_get_string_value(*config, "ALGORITMO_DE_REEMPLAZO");
	*path = config_get_string_value(*config, "PATH");
	*nombre = config_get_string_value(*config, "NOMBRE");
	*intervalo = config_get_int_value(*config, "INTERVALO_DUMP");
}

void crearConfiguracion2(instancia_config** instancia, t_config** config) {

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

