#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
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
 * Objetivo:
 * Controlar los parametros por pantalla
 * */

void shell();
char* obtenerIpServer(char* server);

/**
 * Atributos Globales:
 * */

struct inicial {
/**
 * Para todos los casos el valor 1 significa que se ha incluido de inicio. 0 que no.
 */
    unsigned int serv:1;
    char* servername;
    unsigned int port;

    unsigned int nick:1;
    char* nickname;

    unsigned int channel:1;
    char* channelname;
    unsigned int debug:1;
} estado;

int serverConnected;

/**
 * Funciones
 * */

void usage(char *program_name) {
	printf("Usage: %s [-s host:port] [-n nick] [-c channel] [-d] \n", program_name);
}

void shell() {

}

/* Obtiene la ip a traves de su hostname */
char* obtenerIpServer(char* server) {
	struct sockaddr_in host;

	host.sin_addr = * (struct in_addr*) gethostbyname(server)->h_addr;
	return inet_ntoa(host.sin_addr);
}

void conexion(){
	struct sockaddr_in serverIn;

	/* Se crea el socket */
	serverConnected = socket(AF_INET, SOCK_STREAM, 0);

	if(serverConnected < 0){
		perror("Error creando socket");
	}

	/* Obtiene la direccion del servidor */
	serverIn.sin_family = AF_INET;
	serverIn.sin_port = htons(estado.port);
	serverIn.sin_addr.s_addr = inet_addr(estado.servername);

	/* Comprueba si se puede conectar */
	if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
		char* ip = obtenerIpServer(estado.servername);
		serverIn.sin_addr.s_addr = inet_addr(ip);
		if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
			fprintf(stderr, "Error en la conexion con el servidor %s:%i", estado.servername, estado.port);
			exit(-1);
		}
	}
}

void conectarCanal(){

}

int main(int argc, char *argv[]){
	char *program_name = argv[0];
	char* aux;
	int opt;

	printf("Inicio \n");

	/* Parse command-line arguments */
	while ((opt = getopt(argc, argv, "n:s:d:c:")) != -1) {
		switch (opt) {
			case 'n':
				estado.nick = 1;
				estado.nickname = optarg;
				break;
			case 'd':
				estado.debug = 1;
				break;
			case 's':
				estado.serv = 1;
				estado.servername = strtok(optarg, ":");
				aux = strtok(NULL, ":");
				if(aux != NULL){
					estado.port = strtol(aux, NULL, 10);
				}else{
					estado.serv = 0;
					estado.servername = NULL;
					printf("Formato equivocado de <server:port>");
				}
				break;
			case 'c':
				estado.channel = 1;
				estado.channelname = optarg;
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
				exit(-1);
		}
	}
	
	if(estado.debug){
		printf("MODO DEBUG \n");
	}
	
	if(estado.serv){
		printf("Server:%s \n", estado.servername);
		printf("Port: %u \n", estado.port);
		if(estado.channel){
			printf("Canal: %s \n", estado.channelname);
		}
	}
	
	printf("Fin \n");

	if(estado.serv){
		close(serverConnected);
		exit(EXIT_SUCCESS);
	}else{
		exit(-1);
	}
}
