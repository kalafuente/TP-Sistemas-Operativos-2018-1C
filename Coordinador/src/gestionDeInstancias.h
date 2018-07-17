#ifndef GESTIONDEINSTANCIAS_H_
#define GESTIONDEINSTANCIAS_H_
#include "coordinador.h"

void registrarEntradasOcupadasDeLaInstancia(int entradasOcupadas, instancia * instancia);
void registrarInstancia(int sock);
void eliminarClave(int socket, t_list* listaDeClavesConInstancia);
void eliminarInstancia(int socket, t_list* lista);
void destruirInstancia(instancia* instancia);
void destruirClaveConInstancia(claveConInstancia* claveConInstancia);
claveConInstancia* nuevaClaveConInstancia(char* clave);
void destruirLetrasDeLaInstancia(t_list* letrasDeLaInstancia);
claveConInstancia* instanciaQueTieneLaClave(char* clave, t_list* listaDeClavesConInstancia);
void modificarInstanciaListaDeClavesConInstancia(char* clave, instancia* instanciaNueva, t_list* listaDeClavesConInstancia);
instancia*  elegirInstanciaSegunAlgoritmo(char * clave, t_log* logger, t_log* logControlDeDistribucion, t_list* letrasDeLaInstancia);
void destruirLetrasDeLaInstancia(t_list* letrasDeLaInstancia);
void destruirListaDeLetras(instanciaYSusCaracteres* elementoLista);
void destruirChar(char* letra);
#endif /* GESTIONDEINSTANCIAS_H_ */
