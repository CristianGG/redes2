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


/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */


int serverConnected;
char* host;

int c_help(){
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
        "***        /dccinfo                 Info about IP address from other users\n"
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

        return 1;
}

int c_connect(int* serverConnected2, fd_set* descriptorLectura ,int* fdmax, char* servername, char* port){
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
        *serverConnected2 = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        if(*serverConnected2 < 0){
                perror("Error creando socket");
        }

        /* Comprueba si se puede conectar */
        if(connect(*serverConnected2, servinfo->ai_addr, servinfo->ai_addrlen) <0){
                fprintf(stderr, "Error en la conexion con el servidor %s:%s", servername, port);
                exit(-1);
        }else{
                printf("*** Connected to server %s:%s \n", servername, port);
                FD_SET(*serverConnected2, descriptorLectura);
        }
        serverConnected = *serverConnected2;

        if (*fdmax < serverConnected){
                        *fdmax = serverConnected;
        }

        freeaddrinfo(servinfo);

        return 1;
}

int c_auth(char* cadena){
        char* mensaje;
        int length;
        char* aux;
        mensaje = "NICK ";

        mensaje = strcon(mensaje, cadena, "\r\n", NULL);      
	length = strlen(mensaje);
        enviar(mensaje, length);

        free(mensaje);

        mensaje = "USER ";
        aux = strcon(cadena, "\r\n", NULL, NULL);
        mensaje = strcon(mensaje, cadena, " 0 * :", aux);

        free(aux);

        length = strlen(mensaje);
        enviar(mensaje, length);

        free(mensaje);

        return 1;
}

int c_list(){
	char* mensaje;
	int length;

	mensaje = "LIST\r\n";

	length = strlen(mensaje);
	enviar(mensaje, length);

	return 1;
}

int c_join(char* cadena){
        char* mensaje;
        int length;

        mensaje = "JOIN ";

        mensaje = strcon(mensaje, cadena, "\r\n", NULL);
        length = strlen(mensaje);
        enviar(mensaje, length);

        free(mensaje);

        return 1;
}

int c_joinServer(char* cadena){
        if(strstr(cadena, "JOIN"))
                return 1;
        else
                return -0;
}

int c_leave(){
	return 1;
}

int c_who(char* cadena){
	char* mensaje;
	int length;

	mensaje = "WHO ";

	mensaje = strcon(mensaje, cadena, "\r\n", NULL);
	length = strlen(mensaje);
	enviar(mensaje, length);

	free(mensaje);

	return 1;
}

int c_info(char* cadena){
        printf("%s \n" , cadena);

        return 1;
}

int c_msg(char* cadena){
        printf("%s \n" , cadena);

        return 1;
}

int c_disconnect(){
	return 1;
}

int c_quit(){
	char* mensaje;
	int length;

	mensaje = "QUIT :Saludos!\r\n";

	length = strlen(mensaje);
	enviar(mensaje, length);

	return 1;
}

int c_nop(char* cadena){
        printf("%s \n" , cadena);
        return 1;
}

int c_sleep(char* cadena){
        printf("%s \n" , cadena);
        return 1;
}

int c_ping(char* cadena){
        printf("%s \n" , cadena);
        return 1;
}

int c_pingServer(char* cadena){
        if(strstr(cadena, "PING"))
                return 1;
        else
                return 0;
}

int c_pong(char* cadena){
        char* mensaje;
        char* aux = strtok(cadena, "PING ");
        int length;
        mensaje = "PONG ";

        mensaje = strcon(mensaje, aux, "\r\n", NULL);

        length = strlen(mensaje);
        enviar(mensaje, length);

        free(mensaje);

        return 1;
}

/* Envia al servidor los datos */
int enviar(char* mensaje, int length) {
        int enviados;
        int posicion = 0;
        while(length > 0){
                enviados = send(serverConnected,&mensaje[posicion],length,0);

                posicion = posicion +enviados;
                length = enviados - length;
                if (length == -1 && enviados == -1){
                        return 0;
                }
        }
        return 1;
}

/* Recibe del servidor los datos */
int recibir(char** mensaje) {
        int length;
        char* aux;
        char* aux2;
        int leer;
        aux = calloc(2, sizeof(char));
        /* AQUI VALGRIND */
        (*mensaje) = calloc(2, sizeof(char));
        leer = 2;

        do{
                length = recv(serverConnected,aux,leer,0);

                if(length > 0){
                        length = strlen(*mensaje)+strlen(aux)+1;
                        aux2 = calloc(length, sizeof(char));
                        strcat(aux2,(*mensaje));
                        strcat(aux2,aux);
                        free(*mensaje);
                        (*mensaje) = calloc(length, sizeof(char));
                        strcpy(*mensaje, aux2);
                        free(aux2);
                        free(aux);
                        if(strpos(aux, "\r") > -1){
                                leer = 1;
                                aux = calloc(1, sizeof(char));
                        }else{
                                leer = 2;
                                aux = calloc(2, sizeof(char));
                        }
                }
        }while(strstr(*mensaje, "\n") == NULL && length > 0);

        free(aux);

        return 1;
}


/* Recibe del servidor los datos */
int recibirMensaje() {
		int tipo;
        char* mensaje = NULL;
        recibir(&mensaje);

        if(c_pingServer((mensaje)) == 1){
                tipo = c_pong(mensaje);
        }else if(c_joinServer(mensaje) == 1){
                tipo = c_pong(mensaje);
        }else
                tipo = parser(mensaje);

        free(mensaje);

       	return tipo;
}

int parser(char* mensaje) {
        char* grupo;
        int codigo;
        char* cadena;
        char* aux;
        int pos;

        /*printf("%s", mensaje);*/

        grupo = strtok(mensaje, " ");

        codigo = strtol(strtok(NULL, " "), NULL, 10);

        if(codigo == 4){
		strtok(NULL, " ");
		strtok(NULL, " ");
		strtok(NULL, " ");
		grupo = strtok(NULL, " ");
		aux = strtok(NULL, " ");
        }else if(codigo == 254 || codigo == 333){
        	strtok(NULL, " ");
        	aux = strtok(NULL, " ");
        }else if(codigo == 322){
        	strtok(NULL, " ");
		grupo = strtok(NULL, " ");
		aux = strtok(NULL, "\r\n");
        }else if(codigo == 332){
        	grupo = strtok(NULL, " ");
        	aux = strtok(NULL, "\r\n");
        }else if(codigo == 352){
        	strtok(NULL, " ");
		grupo =	strtok(NULL, " ");
		strtok(NULL, ":");
		strtok(NULL, " ");
		aux = strtok(NULL, " ");
        }else if(codigo == 353){
        	strtok(NULL, "=");
           	grupo = strtok(NULL, ":");
           	aux = strtok(NULL, "\r\n");
        }else{
		strtok(NULL, ":");
		aux = strtok(NULL, ":");
        }
        pos = strrem(aux, &cadena, "\r\n");

        if(codigo > 200 && codigo != 332 && codigo != 353 && codigo != 322 && codigo != 352){
        	grupo = NULL;
        }

        if(pos > 0){
                checkCodigo(grupo, codigo, cadena);
                free(cadena);
        }else
                checkCodigo(grupo, codigo, aux);

        return codigo;
}

/* Comprueba los codigos */
int checkCodigo(char* grupo, int codigo, char* cadena) {
	char* aux;
	char* aux2;
	switch(codigo){
	case 0:printf("*** %s \n", cadena);break;
        case 1:
                host = strtok(grupo,":");
                printf("*** %s (from %s)\n", cadena, host);
                break;
        case 2:printf("*** %s \n", cadena);break;
        case 3:printf("*** %s \n", cadena);break;
        case 4:printf("*** umodes available %s, channel modes available %s \n", grupo, cadena); break;
        case 251:printf("*** %s \n", cadena);break;
        case 254:printf("*** %s channels formed \n", cadena);break;
        case 255:printf("*** %s \n", cadena);break;
		/* Final de la lista de usuarios, WHO*/
        case 315:break;
        case 322:
		aux = strtok(cadena, ":");
		aux2 = strtok(NULL, ":");
		printf("*** %s \t %s \t %s \n", grupo, aux, aux2);
		break;
        /* Final de la lista de channels, LIST*/
        case 323:break;
        case 332:
		aux = strtok(cadena, ":");
		aux2 = strtok(NULL, ":");
		printf("*** %s are on channel %s \n", grupo, aux);
		printf("*** Topic for %s: %s \n", aux, aux2);
		break;
        case 352:printf("*** %s \t %s\n", grupo, cadena);break;
        case 353:printf("*** Users in channel %s: %s \n", grupo, cadena);break;
        /* Final de la lista de usuarios, AUTH*/
        case 366:;break;
        case 372:printf("*** %s \n", cadena);break;
        case 376:printf("*** %s \n", cadena);break;
        case 421:printf("*** %s \n", cadena);break;
        default:
        	if(grupo != NULL){
        		printf("*** Unknown message from server with command <%i> \n",codigo);
        	}else{
        		printf("*** Message received with a command unknown to RFC2812 (%i) \n",codigo);
        	}
        	break;
        }

	return 1;
}

/* Obtiene la ip a traves de su hostname */
char* obtenerIpServer(char* server) {
        struct sockaddr_in host;

        host.sin_addr = * (struct in_addr*) gethostbyname(server)->h_addr;
        return inet_ntoa(host.sin_addr);
}

/* Devuelve la posicion de la cadena*/
int strpos(char* origen, char* substring){
        int i = 0;
        int j = 0;
        int pos = -1;
        int encontrado = 0;
        int length = strlen(origen);
        int lengthsub = strlen(substring);

        for(i = 0; i < length; i++){
                if(encontrado == 0){
                        if(origen[i] == substring[j]){
                                j++;
                                if(pos == -1){
                                        pos = i;
                                }else if (j == lengthsub){
                                        encontrado = 1;
                                }
                        }else{
                                pos = -1;
                                j = 0;
                        }
                }
        }

        return pos;
}


/* Concatena string */
char* strcon(char* mensaje, char* mensaje2, char* mensaje3, char* mensaje4) {
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
        aux = calloc((length+length2+length3+length4)+1, sizeof(char));
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

/* Busca 1a cadena repetida y devuelve el resto sin ella*/
int strrem(char* origen, char** dest, char* substring){
        int pos = -1;
        int length = strlen(origen);
        int lengthsub = strlen(substring);
        char* aux;

        pos = strpos(origen, substring);

        if(pos != -1){
                aux = calloc(length-lengthsub+1, sizeof(char));
                strncpy(aux, &origen[0],pos);                
		strncpy(&aux[pos], &origen[pos+lengthsub],(length-lengthsub-pos));
        }else{
                return pos;
        }
        *dest = calloc(length-lengthsub+1, sizeof(char));
        strncpy(*dest, aux, length-lengthsub+1);
        free(aux);

        return pos;
}

/** Busca la cadena y devuelve la cadena comprendida*/
/*length es la cadena que se quiere obtener*/
int strsub(char* origen, char* dest, int posIni, int length){
        dest = calloc(length+posIni+1, sizeof(char));
        strncpy(dest, &origen[posIni], length);
        dest[length] = '\0';

        return 1;
}
