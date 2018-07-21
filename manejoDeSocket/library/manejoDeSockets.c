#include "../library/manejoDeSockets.h"


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
			//exitWithError(logger, serverSocket, "No me pude conectar al servidor", NULL);
			log_error(logger, "No me pude conectar al servidor");
			return -1;
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

void exitWithError(t_log* logger, int socket, char* error_msg, void * buffer) {

	//Si el mensaje existe lo liberamos
	if (buffer != NULL) {
       free(buffer);
    }

   log_error(logger, error_msg); //Informamos que ocurrio un error
   close(socket);
   exitGracefully(logger,1); //Salimos con error
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

int enviarMensaje(t_log* logger, size_t len, const void* msg, int unsocket){
	int total=0;
	size_t bytes_left =len;
	while(total<len){
		int resultado;
		resultado=send(unsocket,msg+total,bytes_left,MSG_NOSIGNAL);
		total+=resultado;
		if(total==-1){
			log_error(logger,"ERROR AL ENVIAR");
			return -1;
		}
		if(total==0){
			log_error(logger,"SE CORTO LA CONEXION");
			return 0;

		}
		//log_info(logger,"Se enviaron %d bytes",total);
		bytes_left-=total;
	}
log_info(logger,"Se envio el mensaje");
return total;


}


int recibirMensaje(t_log* logger, size_t len, void* buffer, int unsocket){
	int bytesHeader =0;
	int aux=0;
	//log_info(logger,"Se recibieron %d bytes",bytesHeader);
	while(bytesHeader<len){
		aux= recv(unsocket, buffer + bytesHeader, len - bytesHeader,
				MSG_WAITALL);
		if(aux ==-1){
			log_error(logger,"ERROR AL RECIBIR");
			return -1;
			//exitWithError(logger,unsocket,"No se pudo enviar el mensaje",buffer);
		}
		if(aux ==0){
			log_error(logger,"recibirMensaje:: SE CORTO LA CONEXION");
			return -3;
			//exitWithError(logger,unsocket,"Se cerro la conexion",buffer);
		}
		bytesHeader+=aux;
		//log_info(logger,"Se recibieron %d bytes",aux);


	}
	log_info(logger,"Se recibieron los datos");
	return bytesHeader;
}
