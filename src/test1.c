#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include <assert.h>
#include <math.h>
#include <sysexits.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

/*
 * Autores:
 *
 *
 * Galan Galiano, Cristian
 *
 * */


/**
 * Funciones
 * */

char* obtenerIpServer(char* server);

/**
 * Atributos Globales:
 * */

struct inicial {
/**
 * Para todos los casos el valor 1 significa que se ha incluido de inicio. 0 que no.
 */
    unsigned int serv:1;
    unsigned int nick:1;
    unsigned int channel:1;
    unsigned int debug:1;
} estado;


void usage(char *program_name) {
	printf("Usage: %s [-s host:port] [-n nick] [-c channel] [-d] \n", program_name);
}

/* Obtiene la ip a traves de su hostname */
char* obtenerIpServer(char* server) {
	struct sockaddr_in host;

	host.sin_addr = * (struct in_addr*) gethostbyname(server)->h_addr;
	return inet_ntoa(host.sin_addr);
}

int main(int argc, char *argv[]){
	char *program_name = argv[0];
	int opt, port=0;
	char *server, *channel, *nick;
	struct sockaddr_in serverIn;
	int serverConnected;

	/* Parse command-line arguments */
	while ((opt = getopt(argc, argv, "ds:p:")) != -1) {
		switch (opt) {
			case 'd':
				estado.debug = 1;
				break;
			case 's':
				estado.serv = 1;
				server = optarg;
				break;
			case 'c':
				estado.channel = 1;
				channel = optarg;
				break;
			case 'n':
				estado.nick = 1;
				nick = optarg;
				break;
			case '?':
				if ((optopt == 's') || (optopt == 'p'))
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				usage(program_name); 
				exit(EX_USAGE);
		}
	}
	
	if (estado.debug)
		printf("SERVER: %s PORT: %d\n",server, port);
	
	/* Se crea el socket */
	serverConnected = socket(AF_INET, SOCK_STREAM, 0);

	if(serverConnected < 0){
		perror("Error creando socket");
	}

	/* Obtiene la direccion del servidor */
	serverIn.sin_family = AF_INET;
	serverIn.sin_port = htons(port);
	serverIn.sin_addr.s_addr = inet_addr(server);

	/* Comprueba si se puede conectar */
	if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
		char* ip = obtenerIpServer(server);
		serverIn.sin_addr.s_addr = inet_addr(ip);
		if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
			fprintf(stderr, "Error en la conexion con el servidor %s:%i", server, port);
			exit(-1);
		}
	}

	close(serverConnected);

	exit(EXIT_SUCCESS);
}

