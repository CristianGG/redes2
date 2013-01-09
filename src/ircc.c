#include "libRedes.h"

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

int init(char* nickname, char* channelname, int error);
void comprobarComando();
void shell();
void obtenerHostPort(char* host, int opcion);

/* NOTA IMPORTANTE:
 * El comando shell(), as� como el algoritmo de parseo de argumentos en main()
 * ha sido tomado de la practica de Sistemas Distribuidos, pero modificandolo casi
 * en su totalidad para cumplir los requisitos especificos de esta practica,
 * ya sea de funcionalidad como los distintos comandos o de restriccion de robustez
 * y el uso de librerias especificas.
 * El algoritmo de select() ha sido tomado en parte del libro de referencia, Beej.
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

struct palabra {
    int cantidad;
    char** nombre;
} palabra;

int serverConnected;
fd_set descriptorLectura;
fd_set copia;
int fdmax;
int quit;

/**
 * Funciones
 * */

void usage(char *program_name) {
        printf("Usage: %s [-s host:port] [-n nick] [-c channel] [-d] \n", program_name);
}

/* Obtiene el host y el port que se le va a pasar por parametro */
void obtenerHostPort(char* host, int opcion) {
        char* aux;
		char* aux2;

        if(strpos(host, ":") != 0){
        	aux = strtok(host, ":");
			aux2 = strtok(NULL, ":");

			if(aux2 == NULL){
					estado.serv = 0;
					if(opcion == 1)
						printf("*** Option -s also needs a port\n");
					else
						printf("*** The /connect command also needs a port\n");
			}else{
		        estado.servername = calloc(strlen(aux)+1, sizeof(char));
		        estado.port  = calloc(strlen(aux2)+1, sizeof(char));

		        strcpy(estado.servername, aux);
		        strcpy(estado.port, aux2);
			}
        }else{
			estado.serv = 0;
			if(opcion == 1)
				printf("*** Option -s also needs a hostname\n");
			else
				printf("*** The /connect command also needs a hostname\n");
        }
}

void shell() {
        char line[1024];
		char *pch;
		char auxLine[1024];
		char* aux;
		char** auxArray;
		int i = 1;
		char* msg;

		memset(line, 0, 1024);
		pch = fgets(line, 1024, stdin);
		strcpy(auxLine,line);

		if(pch != NULL){
			if ( (strlen(line)>1) && ((line[strlen(line)-1]=='\n') || (line[strlen(line)-1]=='\r')) )
				line[strlen(line)-1]='\0';

			aux = strtok(auxLine, " ");
			if(strcmp(aux, "\n") != 0){
				palabra.cantidad = 1;
			}else{
				palabra.cantidad = 0;
			}

			while(strtok( NULL, " ") != NULL){
				palabra.cantidad += 1;
			}

			auxArray = calloc(palabra.cantidad+1, sizeof(char*));
			aux = strtok(line, " ");
			auxArray[0] = aux;
			while((aux = strtok( NULL, " ")) != NULL){
					auxArray[i] = aux;
					i++;
			}

			palabra.nombre = auxArray;

			if ( (palabra.nombre != NULL) && (palabra.cantidad > 0) ) {


				/* COMANDO HELP */
				if (strcmp(palabra.nombre[0],"/help")==0) {
					if (palabra.cantidad == 1)
						c_help();
					else
						printf("*** Syntax error. Use: /help\n");

				/* COMANDO CONNECT */
				} else if (strcmp(palabra.nombre[0],"/connect")==0) {
					if (palabra.cantidad == 2){
						if(estado.serv == 0){
							estado.serv = 1;
							obtenerHostPort(palabra.nombre[1],0);
							if(estado.serv == 1){
								estado.serv = c_connect(&serverConnected, &descriptorLectura, &fdmax, estado.servername, estado.port);
								if(estado.serv == 0){
						        	fdmax = 0;
						        	estado.channel = 0;
						        	estado.nick = 0;
						        	free(estado.servername);
						        	free(estado.port);
								}
							}
						}else{
							printf("*** You are already connected to %s:%s\n", estado.servername, estado.port);
						}
					}else
							printf("*** The /connect command needs a host:port\n");

				/* COMANDO AUTH */
				} else if (strcmp(palabra.nombre[0],"/auth")==0) {
					if (palabra.cantidad == 2)
						if(estado.serv == 1){
							if(estado.nick == 0)
								c_auth(palabra.nombre[1]);
							else
								printf("*** You are auth \n");
						}else
							printf("*** You are not connected to any server\n");
					else
							printf("*** You must specify the nickname to use.\n");

				/* COMANDO LIST */
				} else if (strcmp(palabra.nombre[0],"/list")==0) {
					if (palabra.cantidad == 1){
						if(estado.serv == 1){
							if(estado.nick == 1){
								c_list();
							}else
								printf("*** You are not authenticated in the server\n");
						}else
							printf("*** You are not connected to any server\n");
					}else
							printf("*** Syntax error. Use: /list\n");

				/* COMANDO JOIN */
				} else if (strcmp(palabra.nombre[0],"/join")==0) {
					if (palabra.cantidad == 2){
						if(estado.serv == 1){
							if(estado.nick == 1){
								if(estado.channel == 0){
									c_join(palabra.nombre[1]);

									estado.channelname = calloc(strlen(palabra.nombre[1])+1, sizeof(char));
									strcpy(estado.channelname, palabra.nombre[1]);
								}else{
									printf("*** You are already in channel %s. Leave the channel and try again\n",estado.channelname);
								}
							}else
								printf("*** You are not authenticated in the server\n");
						}else
							printf("*** You are not connected to any server\n");
					}else if(palabra.cantidad == 1){
							if(estado.channel == 1)
								printf("*** You are in channel %s\n",estado.channelname);
							else
								printf("*** You are not in any channel\n");
					}else{
							printf("*** Syntax error. Use: /join [<channel>]\n");
					}

				/* COMANDO LEAVE */
				} else if (strcmp(palabra.nombre[0],"/leave")==0) {
					if (palabra.cantidad == 1){
						if(estado.serv == 1){
							if(estado.nick == 1){
								if(estado.channel == 1){
									c_leave(estado.channelname);
								}else
									printf("*** You are not in any channel\n");
							}else
								printf("*** You are not authenticated in the server\n");
						}else
							printf("*** You are not connected to any server\n");
					}else{
							printf("*** Syntax error. Use: /leave\n");
					}

				/* COMANDO WHO */
				} else if (strcmp(palabra.nombre[0],"/who")==0) {
					if (palabra.cantidad == 1){
						if(estado.serv == 1){
							if(estado.nick == 1){
								if(estado.channel == 1){
									c_who(estado.channelname);
								}else
									printf("*** You are not in any channel\n");
							}else
								printf("*** You are not authenticated in the server\n");
						}else
							printf("*** You are not connected to any server\n");
					}else
							printf("Syntax error. Use: /who\n");

				/* COMANDO INFO */
				} else if (strcmp(palabra.nombre[0],"/info")==0) {
					if(estado.serv == 1){
						if(estado.nick == 1){
							if(estado.channel == 1){
								if (palabra.cantidad == 2){
									c_info(palabra.nombre[1]);
								}else
									printf("*** You must specify the nickname of the user.\n");
							}else
								printf("*** You are not in any channel\n");
						}else
							printf("*** You are not authenticated in the server\n");
					}else
						printf("*** You are not connected to any server\n");

				/* COMANDO MSG */
				} else if (strcmp(palabra.nombre[0],"/msg")==0) {
					if (palabra.cantidad >= 2){
							if(estado.serv == 1){
								if(estado.nick == 1){
									if(estado.channel == 1){
										aux = calloc(1, sizeof(char));
										for(i = 1;  i <= palabra.cantidad; i++){
											msg = strcon(aux, palabra.nombre[i], " \0", NULL, NULL);
					                        aux = calloc(strlen(msg)+1, sizeof(char));
					                        strcpy(aux, msg);
					                        free(msg);
										}
										c_msg(estado.channelname, aux);
										free(aux);
									}else
										printf("*** You are not in any channel\n");
								}else
									printf("*** You are not authenticated in the server\n");
							}else
								printf("*** You are not connected to any server\n");
					}else
							printf("*** Syntax error. Use: /msg <text>\n");

				/* COMANDO DISCONNECT */
				} else if (strcmp(palabra.nombre[0],"/disconnect")==0) {
					if (palabra.cantidad == 1){
						if(estado.serv == 1)
							quit = c_disconnect(&descriptorLectura, estado.servername, estado.port);
						else
							printf("*** You are not connected to any server\n");
					}else{
							printf("*** Syntax error. Use: /disconnect\n");
					}

				/* COMANDO QUIT */
				} else if (strcmp(palabra.nombre[0],"/quit")==0) {
					if (palabra.cantidad == 1){
						if(estado.serv == 1)
							quit = c_quit(&descriptorLectura);
						else
							quit = -1;
						printf("*** Bye!\n");
					} else
						printf("Syntax error. Use: /quit\n");

				/* COMANDO NOP */
				} else if (strcmp(palabra.nombre[0],"/nop")==0) {
					if (palabra.cantidad == 2)
							c_nop(estado.debug, palabra.nombre[1]);
					else
							printf("Syntax error. Use: /nop [<text>]\n");


				} else if (strcmp(palabra.nombre[0],"/sleep")==0) {
					if (palabra.cantidad == 2){
							c_sleep(estado.debug, palabra.nombre[1]);
					}else if (palabra.cantidad == 1){
							c_sleep(estado.debug, "5");
					}else
							printf("Syntax error. Use: /sleep [<secs>]\n");

				/* COMANDO MSG */
				} else if(palabra.cantidad > 0){
					if(palabra.nombre[0][0] != '/'){
						if(estado.serv == 1){
							if(estado.nick == 1){
								if(estado.channel == 1){
									aux = calloc(1, sizeof(char));
									for(i = 0;  i < palabra.cantidad; i++){
										msg = strcon(aux, palabra.nombre[i], " \0", NULL, NULL);
										free(aux);
				                        aux = calloc(strlen(msg)+1, sizeof(char));
				                        strcpy(aux, msg);
				                        free(msg);
									}
									c_msg(estado.channelname, aux);
									free(aux);
								}else
									printf("*** You are not in any channel\n");
							}else
								printf("*** You are not authenticated in the server\n");
						}else
							printf("*** You are not connected to any server\n");
					}else{
						printf("*** %s is not a valid command. Type /help to see a list of available commands.\n", palabra.nombre[0]);
					}
				}
			}
			free(auxArray);
		}

}

int init(char* nickname, char* channelname, int error){
	int i = 0;

	/*Si se han dado ya los nombres del servidor*/
    if(estado.serv == 1){
			estado.serv = c_connect(&serverConnected, &descriptorLectura, &fdmax, estado.servername, estado.port);
            /* Si se ha conectado al servidor*/
			if(estado.serv == 1){
				/*Si ha habido un error al introducir el nick*/
				if(error == 0 || error == 2){
					/*Si se le ha dado un nick de entrada*/
					if(estado.nick == 1){
						estado.nick = 0;
						estado.nickname = calloc(strlen(nickname)+1, sizeof(char));
						strcpy(estado.nickname, nickname);

						c_auth(estado.nickname);
						quit = recibirMensaje(&descriptorLectura);
						/*Comprueba que se ha dado el mensaje recibido correcto*/
						if(quit == 1){
							estado.nick = 1;

							for(i = 0; i < 14; i++){
								recibirMensaje(&descriptorLectura);
							}

							if(error != 2){
								if(estado.channel == 1){
									estado.channel = 0;
									estado.channelname = calloc(strlen(channelname)+1, sizeof(char));
									strcpy(estado.channelname, channelname);

									c_join(estado.channelname);
									quit = recibirMensaje(&descriptorLectura);

									comprobarComando();
								}
							}else{
								printf("*** You must specify a channel\n");
							}
						}else{
							if(estado.channel == 1)
								printf("*** You are not authenticated in the server\n");

							estado.nick = 0;
							estado.channel = 0;

							free(estado.nickname);


						}
					}
				}else if(error == 1){
					printf("*** You must specify the nickname to use\n");
					estado.channel = 0;
				}
				if(estado.channel == 1){
					printf("*** You are not authenticated in the server\n");
					estado.channel = 0;
				}
            }else{
            	/* Si no se puede conectar al servidor*/
				fdmax = 0;
				estado.channel = 0;
				estado.nick = 0;
				free(estado.servername);
				free(estado.port);
            }
    }else{
    	fdmax = 0;
    	if(estado.channel == 1 || estado.nick == 1)
    		printf("*** You are not connected to any server.\n");
    	estado.channel = 0;
       	estado.nick = 0;
    }

    if(estado.debug == 1){
		printf("%i \n",estado.serv);
		printf("%i \n",estado.nick);
		printf("%i \n",estado.channel);
    }
	return 1;
}

void comprobarComando(){
	 /* COMMAND AUTH*/
		if(quit == 1){
			estado.nick = 1;
		/* COMMAND JOIN*/
		}else if(quit == 332){
			estado.channel = 1;
		/* ERROR: SERVER DOWN */
		}else if(quit == 432 || quit == 433){
			if(estado.nick == 1){
				estado.nick = 0;
				free(estado.nickname);
			}
		/* OTROS ERRORES*/
		}else if(quit == 451){
			if(estado.channel == 1){
				estado.channel = 0;
				free(estado.channelname);
			}
		/* CANAL INEXISTENTE*/
		}else if(quit == 474){
			estado.channel = 0;
			free(estado.channelname);
		/* ERROR: SERVER DOWN */
		}else if(quit == 504){
			if(estado.serv == 1){
				estado.serv = 0;
				free(estado.servername);
				free(estado.port);
				FD_CLR(serverConnected, &descriptorLectura);
				close(serverConnected);
			}
			if(estado.nick == 1){
				estado.nick = 0;
				free(estado.nickname);
			}
			if(estado.channel == 1){
				estado.channel = 0;
				free(estado.channelname);
			}
		/* COMMAND DISCONNECT // CLOSED */
		}else if(quit == 505){
			if(estado.serv == 1){
				estado.serv = 0;
				free(estado.servername);
				free(estado.port);
			}
			if(estado.nick == 1){
				estado.nick = 0;
				free(estado.nickname);
			}
			if(estado.channel == 1){
				estado.channel = 0;
				free(estado.channelname);
			}
		/* COMMAND LEAVE*/
		}else if(quit == 506){
			/* Sin la 2a comprobacion si se escribe muy rapido 2 veces el comando peta dando un error de doble free,
			 * en la teoria no deberia entrar pero lo hace y depurando no entra dos veces en el comando leave, pero recibe dos 451
			 * */
			if(estado.channel == 1){
				estado.channel = 0;
				free(estado.channelname);
			}
		}

		quit = 0;

	    if(estado.debug == 1){
			printf("%i \n",estado.serv);
			printf("%i \n",estado.nick);
			printf("%i \n",estado.channel);
	    }
}

int main(int argc, char *argv[]){
		char *program_name = argv[0];
        int opt;
        int i;
        char* aux;
        char* aux2;
        int error = 0;

        /* Parse command-line arguments */
        while ((opt = getopt(argc, argv, "n:s:dc:")) != -1) {
		   switch (opt) {
				   case 'n':
						   estado.nick = 1;
						   aux = optarg;
						   break;
				   case 'd':
						   estado.debug = 1;
						   break;
				   case 's':
						   estado.serv = 1;
						   if(argv[3] == NULL)
							   obtenerHostPort(optarg, 1);
						   else
							   if(strpos(argv[3], "-") == 0)
								   obtenerHostPort(optarg, 1);
							   else{
								   printf("*** Option -s also needs a host:port\n");
								   estado.serv = 0;
							   }
						   break;
				   case 'c':
						   estado.channel = 1;
						   aux2 = optarg;
						   break;
				   case '?':
						   if (optopt == 's'){
							   fprintf (stderr, "*** Option -%c needs a host:port.\n", optopt);
						   }else if (optopt == 'n'){
							 	error = 1;
						   }else if (optopt == 'c'){
						   	 	error = 2;
						   }else if (isprint (optopt))
							   fprintf (stderr, "*** Unknown option `-%c'.\n", optopt);
						   else
							   fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
						   break;
				   default:
						   usage(program_name);
						   break;
		   }
        }

        if(estado.debug){
                printf("MODO DEBUG \n");
        }

        FD_ZERO(&descriptorLectura);

        /* Llama al metodo para comprobar los datos de entrada*/
        init(aux, aux2, error);

        FD_SET(0, &descriptorLectura);

        quit = 0;
        while(quit >= 0) {
                copia = descriptorLectura;

                if (select(fdmax+1, &copia, NULL, NULL, NULL) == -1) {
                        perror("select");
                        exit(4);
                }

                for(i = 0; i <= fdmax; i++) {
                        if (FD_ISSET(i, &copia)) {
                        	if (i == 0) {
								shell();
							} else if (i == serverConnected) {
								quit = recibirMensaje(&descriptorLectura);
							}
                        }
                }

                if(quit > 0)
                	comprobarComando();
        }

        /* Liberar memoria */
      	if(estado.nick == 1)
        	free(estado.nickname);

       	if(estado.channel == 1)
			free(estado.channelname);

        if(estado.serv == 1){
        	free(estado.servername);
			free(estado.port);
        }
		exit(EXIT_SUCCESS);



}
