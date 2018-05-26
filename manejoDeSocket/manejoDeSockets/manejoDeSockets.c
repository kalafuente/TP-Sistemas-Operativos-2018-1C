#include "manejoDeSockets.h"


t_log* crearLogger(char*archivo, char*nombre) {

  return log_create(archivo, nombre, true, LOG_LEVEL_INFO);
}

int conectarseAlServidor(t_log* logger, char ** ip, char ** puerto) {
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(*ip, *puerto, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion, 	Ya se quien y a donde me tengo que conectar.

	int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol); //Obtiene un socket (un file descriptor -todo en linux es un archivo-), utilizando la estructura serverInfo que generamos antes.

	int resultado = connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas
	if (resultado < 0){
			exitWithError(logger, serverSocket, "No me pude conectar al servidor", NULL);
		}

	return serverSocket;
}



int crearSocketQueEscucha(char ** puerto, int * entradas) {
	struct addrinfo hints;
	struct addrinfo *serverInfo;
	printf("Puerto: %s\n", *puerto);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, *puerto, &hints, &serverInfo);// Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);

	bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	printf("Esperando conexiones entrantes!!\n");
	listen(listenningSocket, *entradas);
	return listenningSocket;
}



/*
int enviarMensaje(t_log* logger, int id, char*mensaje, int unsocket) {

	log_info(logger, "Enviando mensaje con enviarMensaje"); //Indicamos que vamos a enviar el mensaje

	ContentHeader * cabeza = calloc(1, sizeof(ContentHeader));

	cabeza->id=id;
	cabeza->len = strlen(mensaje); //Indicamos que la longitud es la del mensaje (incluyendo el fin de string)

	//char *message = calloc(cabeza->len, sizeof(char));
	//strcpy(message, mensaje);
	//No hace falta


	//Intentamos enviar el header
	if (send(unsocket, cabeza, sizeof(ContentHeader), 0) <= 0) {
		//exitWithError(logger, unsocket, "No se pudo enviar el header", cabeza);
		return 0;
	}

	//Intentamos enviar el mensaje
	if (send(unsocket, mensaje, strlen(mensaje), 0) <= 0) {
		//exitWithError(logger, unsocket, "No se pudo enviar el mensaje", cabeza);
		return 0;
	}

	log_info(logger, "Se envió el msj");
	free(cabeza);
	//free(message);

	return 1;
}

*/





int librocket_enviarMensaje(int sockfd, const void * datos, size_t bytesAenviar,
		t_log* t_log) {

	int bytes_enviados = send(sockfd, datos, bytesAenviar, 0);

	if (bytes_enviados == -1) { //ERROR
		perror("send:");
		log_info(t_log, "No se pudieron enviar datos");
		return -1;
	} else if (bytes_enviados == 0) { //conexion cerrada
		char* mensaje = string_new();
		string_append(&mensaje, "Se desconecto el cliente con el socket  ");
		string_append(&mensaje, sockfd);
		log_info(t_log, mensaje);
		free(mensaje);
		return 0;
	} else if (bytes_enviados < bytesAenviar) {
		char* mensaje = malloc(200);
		sprintf(mensaje, "Se enviaron %d bytes de %d esperados", bytes_enviados,
				bytesAenviar);
		log_error(t_log, mensaje);
		log_error(t_log,
				"Entrando en recursion para enviar mensaje completo \n");
		free(mensaje);
		int nuevosBytes = bytesAenviar - bytes_enviados;
		librocket_enviarMensaje(sockfd, datos + bytes_enviados, nuevosBytes,
				t_log);
	} else if (bytes_enviados == bytesAenviar) {

		return bytes_enviados;
	}

	return bytes_enviados;

}

int enviarMensajeGenerico(t_log* logger, int tamanio, int id, void*mensaje, int unsocket) {

	log_info(logger, "Enviando mensaje"); //Indicamos que vamos a enviar el mensaje

	ContentHeader * cabeza = calloc(1, sizeof(ContentHeader));

	cabeza->id=id;
	cabeza->len = tamanio; //Indicamos que la longitud es la del mensaje (incluyendo el fin de string)

	//char *message = calloc(cabeza->len, sizeof(char));
	//strcpy(message, mensaje);
	//No hace falta


	//Intentamos enviar el header
	if (send(unsocket, cabeza, sizeof(ContentHeader), 0) <= 0) {
		//exitWithError(logger, unsocket, "No se pudo enviar el header", cabeza);
		return 0;
	}

	//Intentamos enviar el mensaje
	if (send(unsocket, mensaje, tamanio, 0) <= 0) {
		//exitWithError(logger, unsocket, "No se pudo enviar el mensaje", cabeza);
		return 0;
	}

	free(cabeza);
	//free(message);
	return 1;
}
/*
int recibirMensaje(t_log* logger, int unsocket) {
	ContentHeader * cabeza = calloc(1, sizeof(ContentHeader));

	//Esperamos recibir el header, el cual nos indicara la longitud del mensaje
	if (recv(unsocket, cabeza, sizeof(ContentHeader), MSG_WAITALL) <= 0) {
		//exitWithError(logger, unsocket, "No se pudo recibir el header", cabeza);
		free(cabeza);
		return 0;

	}
	char *message = calloc(cabeza->len, sizeof(char));
	int id = cabeza-> id;

	//Esperamos recibir el mensaje
	if (recv(unsocket, message, cabeza->len, MSG_WAITALL) <= 0) {
		//exitWithError(logger, unsocket, "No se pudo recibir el mensaje", message);
		free(cabeza);
		return 0;
	}

	//Indicamos que recibimos el mensaje y lo mostramos
	log_info(logger, "Mensaje recibido: '%s'", message);

	free(cabeza);
	free(message);
	return id;

}
*/
void * recibirIDyContenido(int * id, t_log * logger, int socket) {

  log_info(logger, "recibirIDyContenido: Esperando el encabezado del contenido(%ld bytes)", sizeof(ContentHeader));

  ContentHeader * cabeza = (ContentHeader*) malloc(sizeof(ContentHeader));

  if (recv(socket, cabeza, sizeof(ContentHeader), 0) <= 0) {
    exitWithError(logger, socket, "No se pudo recibir el encabezado del contenido", cabeza);
  }

  *id= cabeza->id;

  log_info(logger, "Esperando el contenido (%d bytes)", cabeza->len);

  void * buf = calloc(sizeof(char), cabeza->len + 1);
  if (recv(socket, buf, cabeza->len, MSG_WAITALL) <= 0) {
    free(buf);
    exitWithError(logger, socket, "Error recibiendo el contenido", cabeza);
  }


  log_info(logger, "Contenido recibido '%s'", (char*) buf);
  free(cabeza);
  return buf;
}


void exitWithError(t_log* logger, int socket, char* error_msg, void * buffer) {

	//Si el mensaje existe lo liberamos
	if (buffer != NULL) {
       free(buffer);
    }

   log_error(logger, error_msg); //Informamos que ocurrio un error
   close(socket);
   exitGracefully(logger,1); //Salimos con error
}

void configurarElLogger(t_log* logger, char* nombreDelArchivo, char* nombreAMostrar) {

  logger = log_create(nombreDelArchivo, nombreAMostrar, 1, LOG_LEVEL_INFO);
}

int conectarAlServidor(t_log* logger, char * ip, char * puerto) {
  struct addrinfo hints;
  struct addrinfo * serverInfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    // Verifica si usamos IPv4 o IPv6
  hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

  getaddrinfo(ip, puerto, &hints, &serverInfo);  // Carga en server_info los datos de la conexion

  // Creamos serverSocket a partir de serverInfo
  int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol/* familia, socktype, protocolo */);

  // Nos conectamos al servidor mediante el socket, guardamos el valor de retorno en mePudeConectar para verificar dicha conexion
  int mePudeConectar = connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen/* socket, address, longitud de la address */);

  freeaddrinfo(serverInfo);  // No es necesario ya

  // Si no nos podemos conectar, salimos con un error
  if (mePudeConectar < 0) {
     exitWithError(logger,serverSocket, "No me pude conectar al servidor", NULL);
  }

  // Informamos que nos conectamos y devolvemos el socket
  log_info(logger, "Conectado!");
  return serverSocket;
}

void exitGracefully(t_log* logger, int valorDeRetorno) {

	log_destroy(logger);
	exit(valorDeRetorno);
}


int recibirSaludo(t_log* logger, int socket, char * saludo){

	char * buffer = calloc(strlen(saludo)+ 1, sizeof(char)); // reservamo memoria para recibir msj
	int result_recv = recv(socket, buffer, strlen(saludo), MSG_WAITALL); // recibimos msj en el buffer
	if(result_recv <= 0) {
	    exitWithError(logger, socket, "No se pudo recibir hola", buffer);
	 }
	if (strcmp(buffer, saludo) != 0) {
		 exitWithError(logger, socket, "No se pudo recibir hola", buffer);
	 }

	free(buffer);
	return 1;
}





int enviarMensaje(int unsocket, const void* msg,, size_t len,t_log* logger ){
	int total=0;
	size_t bytes_left =len;
	while(total<len){
		total+=send(unsocket,msg+total,bytes_left,0);
		if(total==-1){
			perror("No se pudo enviar el mensaje");
			log_info(logger,"Fallo el envio de mensajes");
			return -1;
		}
		if(total==0){
			log_info(logger,"Se cerro la conexion");
		}
		bytes_left-=total;
	}
log_info(logger,"Se envio el mensaje");
return 1;


}


int recibirMensaje(int unsocket,void* buffer, size_t len ,t_log* logger){
	int bytesHeader = recv(unsocket, buffer, len, 0);

	while(bytesHeader<len){
		if(bytesHeader ==-1){
			log_error(logger,"Error al recibir datos");
			return -1;
		}
		if(bytesHeader ==0){
			return 0;
		}
		bytesHeader+=recv(unsocket,buffer+bytesHeader,len-bytesHeader,0);

	}
	log_info(logger,"Se recibieron los datos");
	return bytesHeader;
}


