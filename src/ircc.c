#include "libRedes.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


/*
 * Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */

/**
 * Atributos Globales:
 * */

struct inicio {
/**
 * Para todos los casos el valor 1 significa que se ha incluido de inicio. 0 que no.
 */
    unsigned int serv:1;
    char* servername;
    char* port;

    unsigned int nick:1;
    char* nickname;

    unsigned int channel:1;
    char* channelname;
    unsigned int debug:1;
} estado;

int serverConnected;
fd_set descriptorLectura;
int fdmax;

/**
 * Funciones
 * */

/* Obtiene el host y el port que se le va a pasar por parametro */
void obtenerHostPort(char* host) {
	char* aux;

	estado.serv = 1;
	estado.servername = strtok(host, ":");
	aux = strtok(NULL, ":");
	if(aux != NULL){
		estado.port = aux;
	}else{
		estado.serv = 0;
		estado.servername = NULL;
		printf("Formato equivocado de <server:port> \n");
	}
}

void c_connect2(fd_set* descriptorLectura ,int* fdmax, char* servername, char* port){
	int status;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(servername, port, &hints, &servinfo)) != 0) {
	    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
	}

	/* Se crea el socket no bloqueante */
	serverConnected = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if(serverConnected < 0){
		perror("Error creando socket");
	}

	/* Comprueba si se puede conectar */
	if(connect(serverConnected, servinfo->ai_addr, servinfo->ai_addrlen) <0){
		fprintf(stderr, "Error en la conexion con el servidor %s:%s", servername, port);
		exit(-1);
	}else{
		printf("*** Connected to server %s:%s", servername, port);
		FD_SET(serverConnected, descriptorLectura);
	}

	if (*fdmax < serverConnected){
			*fdmax = serverConnected;
	}

	freeaddrinfo(servinfo);
}

int main(int argc, char *argv[]){
	int opt;
	int i;

	printf("Inicio \n");

	/* Parse command-line arguments */
	while ((opt = getopt(argc, argv, "n:s:dc:")) != -1) {
		switch (opt) {
			case 'n':
				estado.nick = 1;
				estado.nickname = optarg;
				break;
			case 'd':
				estado.debug = 1;
				break;
			case 's':
				obtenerHostPort(optarg);
				break;
			case 'c':
				estado.channel = 1;
				estado.channelname = optarg;
				break;
			case '?':
				if (optopt == 's')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				exit(-1);
		}
	}

	if(estado.debug){
		printf("MODO DEBUG \n");
	}

	FD_ZERO(&descriptorLectura);
	FD_SET(0, &descriptorLectura);

	if(estado.serv){
		c_connect2(&descriptorLectura, &fdmax, estado.servername, estado.port);
		printf("Server:%s \n", estado.servername);
		printf("Port: %s \n", estado.port);
	}else{
		fdmax = 0;
	}

	printf("Descriptor socket: %i \n", serverConnected);
	printf("fdmax: %i \n",fdmax);
	while(1) {
		if (select(fdmax+1, &descriptorLectura, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		for(i = 0; i <= fdmax; i++) {

			if (FD_ISSET(i, &descriptorLectura)) {
				if (i == 0) {
					fprintf(stdout, "c> ");
					printf("Descriptor de la entrada");
				} else if (i == serverConnected) {
					printf("Descriptor de la salida");
				}else{

				}
			}
		}
	}

	printf("Fin \n");

	if(estado.serv){
		printf("%i \n", serverConnected);
		close(serverConnected);
		exit(EXIT_SUCCESS);
	}else{
		exit(-1);
	}
}
