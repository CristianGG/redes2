#include "libRedes.c"

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
 * */

void shell();
char* obtenerIpServer(char* server);

/**
 * Atributos Globales:
 * */

struct inicio {
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

struct palabra {
    int cantidad;
    char** nombre;
} palabra;

int serverConnected;

fd_set descriptorLectura;
int fdmax;

/**
 * Funciones
 * */

void usage(char *program_name) {
	printf("Usage: %s [-s host:port] [-n nick] [-c channel] [-d] \n", program_name);
}

/* Obtiene el host y el port que se le va a pasar por parametro */
void obtenerHostPort(char* host) {
	char* aux;

	estado.serv = 1;
	estado.servername = strtok(host, ":");
	aux = strtok(NULL, ":");
	if(aux != NULL){
		estado.port = strtol(aux, NULL, 10);
	}else{
		estado.serv = 0;
		estado.servername = NULL;
		printf("Formato equivocado de <server:port> \n");
	}
}

void shell() {
	char line[1024];
		char *pch;
		int exit = 0;
		char* aux;
		char** auxArray;
		int i = 1;

		do {
			fprintf(stdout, "c> ");
			memset(line, 0, 1024);
			pch = fgets(line, 1024, stdin);

			if ( (strlen(line)>1) && ((line[strlen(line)-1]=='\n') || (line[strlen(line)-1]=='\r')) )
				line[strlen(line)-1]='\0';


			strtok(line, " ");
			palabra.cantidad = 1;
			while(strtok( NULL, " ") != NULL){
				palabra.cantidad += 1;
			}

			auxArray = calloc(palabra.cantidad, sizeof(char*));
			aux = strtok(line, " ");
			auxArray[0] = aux;


			while( (aux = strtok( NULL, " ")) != NULL){
					auxArray[i] = aux;
					i++;
			}

			palabra.nombre = auxArray;

			if ( (palabra.nombre != NULL) && (palabra.cantidad > 0) ) {
				if (strcmp(palabra.nombre[0],"/help")==0) {
					if (palabra.cantidad == 1)
						c_help();
					else
						printf("Syntax error. Use: /help\n");
				} else if (strcmp(palabra.nombre[0],"/connect")==0) {
					if (palabra.cantidad == 2){
						obtenerHostPort(palabra.nombre[1]);
						if(estado.serv){
							c_connect(&serverConnected, estado.servername, estado.port);
						}
					}else
						printf("Syntax error. Use: /connect <host:port>\n");
				} else if (strcmp(palabra.nombre[0],"/auth")==0) {
					if (palabra.cantidad == 2)
						c_auth(palabra.nombre[1]);
					else
						printf("Syntax error. Use: /auth <nick>\n");
				} else if (strcmp(palabra.nombre[0],"/list")==0) {
					if (palabra.cantidad == 1)
						c_list();
					else
						printf("Syntax error. Use: /list\n");
				} else if (strcmp(palabra.nombre[0],"/join")==0) {
					if (palabra.cantidad == 2)
						c_join(palabra.nombre[1]);
					else
						printf("Syntax error. Use: /join [<channel>]\n");
				} else if (strcmp(palabra.nombre[0],"/leave")==0) {
					if (palabra.cantidad == 1)
						c_leave();
					else
						printf("Syntax error. Use: /leave\n");
				} else if (strcmp(palabra.nombre[0],"/who")==0) {
					if (palabra.cantidad == 1)
						c_who();
					else
						printf("Syntax error. Use: /who\n");
				} else if (strcmp(palabra.nombre[0],"/info")==0) {
					if (palabra.cantidad == 2)
						c_info(palabra.nombre[1]);
					else
						printf("Syntax error. Use: /info <user>\n");
				} else if (strcmp(palabra.nombre[0],"/msg")==0) {
					if (palabra.cantidad == 2)
						c_msg(palabra.nombre[1]);
					else
						printf("Syntax error. Use: /msg <text>\n");
				} else if (strcmp(palabra.nombre[0],"/disconnect")==0) {
					if (palabra.cantidad == 1)
						c_disconnect();
					else
						printf("Syntax error. Use: /disconnect\n");
				} else if (strcmp(palabra.nombre[0],"/quit")==0) {
					if (palabra.cantidad == 1){
						c_quit();
						exit = 1;
					} else
						printf("Syntax error. Use: /quit\n");
				} else if (strcmp(palabra.nombre[0],"/nop")==0) {
					if (palabra.cantidad == 2)
						c_nop(palabra.nombre[1]);
					else
						printf("Syntax error. Use: /nop [<text>]\n");
				} else if (strcmp(palabra.nombre[0],"/sleep")==0) {
					if (palabra.cantidad == 2)
						c_sleep(palabra.nombre[1]);
					else
						printf("Syntax error. Use: /sleep [<secs>]\n");
				} else {
					fprintf(stderr, "Error: command '%s' not valid.\n", palabra.nombre[0]);
				}
			}
				free(auxArray);
		} while ((pch != NULL) && (!exit));
}

int main(int argc, char *argv[]){
	char *program_name = argv[0];
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
		c_connect(&serverConnected, estado.servername, estado.port);
		printf("Server:%s \n", estado.servername);
		printf("Port: %u \n", estado.port);
		if(estado.nick){
			c_auth(estado.nickname);
			printf("Nick: %s \n", estado.nickname);
		}
		if(estado.channel){
			c_connectChannel();
			printf("Canal: %s \n", estado.channelname);
		}
	}
	
	FD_ZERO(&descriptorLectura);

	FD_SET(serverConnected, &descriptorLectura);

	fdmax = serverConnected;

	while(1) {

		if (select(fdmax+1, &descriptorLectura, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}


		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &descriptorLectura)) {
				if (i == serverConnected) {
					shell();

					if (newfd == -1) {
						perror("accept");
					} else {
						FD_SET(newfd, &master);
						if (newfd > fdmax) {
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
					}
				} else {

					if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {

						if (nbytes == 0) {

							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i);
						FD_CLR(i, &master);
					} else {
						for(j = 0; j <= fdmax; j++) {
							if (FD_ISSET(j, &master)) {
								if (j != listener && j != i) {
									if (send(j, buf, nbytes, 0) == -1) {
										perror("send");
									}
								}
							}
						}
					}
				}
			}
		}
	}

	shell();
	
	printf("Fin \n");

	if(estado.serv){
		printf("%i \n", serverConnected);
		close(serverConnected);
		exit(EXIT_SUCCESS);
	}else{
		exit(-1);
	}
}
