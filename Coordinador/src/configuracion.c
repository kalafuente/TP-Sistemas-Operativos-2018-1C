#include "configuracion.h"

coordinador_config * init_coordConfig(){
	coordinador_config* coordinadorConfig = malloc(sizeof (coordinador_config));
	coordinadorConfig->puerto=string_new();
	coordinadorConfig->algoritmo=string_new();
	coordinadorConfig->entradas=0;
	coordinadorConfig->tamanioEntradas=0;

	return coordinadorConfig;
}
void crearConfiguracion(coordinador_config* coordinador, t_config* config){
	string_append(&(coordinador->puerto), config_get_string_value(config, "PUERTO_DE_ESCUCHA"));
	string_append(&(coordinador->algoritmo), config_get_string_value(config, "ALGORITMO_DISTRIBUCIÓN"));
	coordinador->entradas = config_get_int_value(config, "ENTRADAS");
	coordinador->tamanioEntradas = config_get_int_value(config, "TAMANIO_ENTRADAS");
	coordinador->retardo=config_get_int_value(config,"RETARDO_MILISEGUNDOS");
}
void destroy_coordConfig(coordinador_config* coordinadorConfig){
	free(coordinadorConfig->puerto);
	free(coordinadorConfig->algoritmo);
	free(coordinadorConfig);
}
