#include "libRedes.h"

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


/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */


int serverConnected;

void c_help(){
	printf(
	"*** A typical user session goes like this:\n"
	"***\n"
	"***     /connect localhost 6757\n"
	"***     /auth alcortes\n"
	"***     /list\n"
	"***     /join #ro2\n"
	"***     hello\n"
	"***     /leave\n"
	"***     /disconnect\n"
	"***     /quit\n"
	"***\n"
	"*** The full list of available commands are\n"
	"***\n");
	printf(
	"***     /help                    Show this text.\n"
	"***     /connect <host> <port>   Connect to a server.\n"
	"***     /auth <nick>             Authenticate with the server,\n"
	"***                              sets nick, username & real name.\n"
	"***     /list                    Lists all current irc channels,\n"
	"***                              number of users, and topic.\n"
	"***     /join [<channel>]        Join a channel or tell which is\n"
	"***                              the current channel.\n");
	printf(
	"***     /leave                   Leaves your current channel.\n"
	"***     /who                     List users in current channel.\n"
	"***     /msg <txt>               Send <txt> to the channel.\n"
	"***	    /dccinfo                 Info about IP address from other users\n"
	"***     /disconnect              Disconnect from server.\n"
	"***     /quit                    Close program.\n"
	"***     /nop [<text>]            (debugging) no operation,\n"
	"***                              just echoes <text> back.\n");
	printf(
	"***     /sleep [<secs>]          (debugging) pause for <secs>\n"
	"***                              seconds.\n"
	"***\n"
	"*** Lines from the user that don't begin with '/' are equivalent to\n"
	"***\n"
	"***     /msg line\n"
	"***\n"
	"*** Lines beginning with ***  are messages from the program.\n"
	"*** Lines beginning with <user> are messages from <user>.\n"
	"*** Lines beginning with DDD  are debug messages from the program.\n"
	"*** Lines beginning with >  are your messages to other users.\n"
	"*** The rest of the lines are echoes of the user commands.\n");
}

void c_connect(int* serverConnected2, char* servername, int port){
	int status;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(servername, "7102", &hints, &servinfo)) != 0) {
	    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
	    exit(1);
	}

	/* Se crea el socket */
	*serverConnected2 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if(*serverConnected2 < 0){
		perror("Error creando socket");
	}

	/* Comprueba si se puede conectar */
	if(connect(*serverConnected2, servinfo->ai_addr, servinfo->ai_addrlen) <0){
		fprintf(stderr, "Error en la conexion con el servidor %s:%i", servername, port);
		exit(-1);
	}

	serverConnected = *serverConnected2;

	freeaddrinfo(servinfo);
}

void c_connect2(int serverConnected, char* servername, int port){
	struct sockaddr_in serverIn;

	/* Se crea el socket */
	serverConnected = socket(AF_INET, SOCK_STREAM, 0);

	if(serverConnected < 0){
		perror("Error creando socket");
	}

	/* Obtiene la direccion del servidor */
	serverIn.sin_family = AF_INET;
	serverIn.sin_port = htons(port);
	serverIn.sin_addr.s_addr = inet_addr(servername);

	/* Comprueba si se puede conectar */
	if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
		char* ip = obtenerIpServer(servername);
		serverIn.sin_addr.s_addr = inet_addr(ip);
		if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
			fprintf(stderr, "Error en la conexion con el servidor %s:%i", servername, port);
			exit(-1);
		}
	}
}

void c_auth(char* cadena){
	char* mensaje;
	int length;
	char* aux;
	mensaje = "NICK ";

	mensaje = concatenar(mensaje, cadena, "\r\n", NULL);
	printf("%s" , mensaje);

	length = strlen(mensaje);
	enviar(mensaje, length);

	mensaje = "USER ";
	aux = concatenar(cadena, "\r\n", NULL, NULL);
	mensaje = concatenar(mensaje, cadena, aux, NULL);
	printf("%s" , mensaje);

	length = strlen(mensaje);
	enviar(mensaje, length);

}

void c_list(){

}

void c_join(char* cadena){
	printf("%s \n" , cadena);
}

void c_leave(){

}

void c_who(){

}

void c_info(char* cadena){
	printf("%s \n" , cadena);
}

void c_msg(char* cadena){
	printf("%s \n" , cadena);
}

void c_disconnect(){

}

void c_quit(){

}

void c_nop(char* cadena){
	printf("%s \n" , cadena);
}

void c_sleep(char* cadena){
	printf("%s \n" , cadena);
}

void c_connectChannel(){

}


/* Concatena string */
char* concatenar(char* mensaje, char* mensaje2, char* mensaje3, char* mensaje4) {
	char* aux;
	int length = strlen(mensaje);
	int length2 = strlen(mensaje2);
	int length3= 0;
	int length4= 0;
	if(mensaje3 != NULL){
		length3 = strlen(mensaje3);
	}
	if(mensaje4 != NULL){
		length4 = strlen(mensaje4);
	}
	aux = malloc(sizeof(char)*(length+length2+length3+length4));
	*aux = '\0';
	strcat(aux, mensaje);
	strcat(aux, mensaje2);
	if(mensaje3 != NULL){
		strcat(aux, mensaje3);
	}
	if(mensaje4 != NULL){
		strcat(aux, mensaje4);
	}

	return aux;
}

/* Obtiene la ip a traves de su hostname */
char* obtenerIpServer(char* server) {
	struct sockaddr_in host;

	host.sin_addr = * (struct in_addr*) gethostbyname(server)->h_addr;
	return inet_ntoa(host.sin_addr);
}

/* Envia al servidor los datos */
void enviar(char* mensaje, int length) {
	/* FALTA POR HACER EL BUCLE PARA LOS DATOS NO ENVIADOS*/
	send(serverConnected,mensaje,length,0);
}
