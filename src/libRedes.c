#include "libRedes.h"




/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */


int serverConnected;
char* channelname;
int quit = 0;
int channel = 0;
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
                fprintf(stderr, "*** Unable to connect to server %s:%s", servername, port);
                return 0;
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



        mensaje = "NICK ";

        mensaje = strcon(mensaje, cadena, "\r\n", NULL, NULL);
        length = strlen(mensaje);
        enviar(mensaje, length);

        free(mensaje);

        mensaje = "USER ";
        mensaje = strcon(mensaje, cadena, " 0 * :", cadena, "\r\n");
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

        if(cadena != NULL){
			mensaje = "JOIN ";

			channel = 1;
			channelname = calloc(strlen(cadena), sizeof(char));
			strcpy(channelname, cadena);

			mensaje = strcon(mensaje, cadena, "\r\n", NULL, NULL);
			length = strlen(mensaje);
			enviar(mensaje, length);

			free(mensaje);
        }
        return 1;
}

int c_leave(char* cadena){
		char* mensaje;
		int length;

		mensaje = "PART ";

		mensaje = strcon(mensaje, cadena, "\r\n", NULL, NULL);
		length = strlen(mensaje);
		enviar(mensaje, length);

		free(mensaje);

		if(channel == 1){
			free(channelname);
			channel = 0;
		}

		return 1;
}

int c_who(char* cadena){
		char* mensaje;
		int length;

		mensaje = "WHO ";

		mensaje = strcon(mensaje, cadena, "\r\n", NULL, NULL);
		length = strlen(mensaje);
		enviar(mensaje, length);

		free(mensaje);

		return 1;
}

int c_info(char* cadena){
		char* mensaje;
		int length;

		mensaje = "WHOIS ";

		mensaje = strcon(mensaje, cadena, "\r\n", NULL, NULL);
		length = strlen(mensaje);
		enviar(mensaje, length);

		free(mensaje);

		return 1;
}

int c_msg(char* channelname, char* cadena){
		char* mensaje;
		int length;

		mensaje = "PRIVMSG ";

		mensaje = strcon(mensaje, channelname, " :", cadena, "\r\n");
		length = strlen(mensaje);
		enviar(mensaje, length);

		free(mensaje);

		printf("> %s \n" , cadena);
		return 1;
}

int c_disconnect(fd_set* descriptorLectura, char* host, char* port){
		char* mensaje;
		int length;

		mensaje = "QUIT :Disconnected from server\r\n";

		length = strlen(mensaje);
		enviar(mensaje, length);

		quit = 0;

		FD_CLR(serverConnected, descriptorLectura);
		close(serverConnected);

		printf("*** Disconnected from server %s:%s\n", host, port);

		if(channel == 1){
			free(channelname);
			channel = 0;
		}

		return 505;
}

int c_quit(fd_set* descriptorLectura){
		char* mensaje;
		int length;

		mensaje = "QUIT :Bye!\r\n";

		length = strlen(mensaje);
		enviar(mensaje, length);

		quit = -1;

		FD_CLR(serverConnected, descriptorLectura);
		close(serverConnected);

		if(channel == 1){
			free(channelname);
			channel = 0;
		}

		return quit;
}

int c_nop(int depurar, char* cadena){
		if(depurar == 1){
			printf("*** %s \n" , cadena);
		}

        return 1;
}

int c_sleep(int depurar, char* cadena){
        int tiempo;
        if(depurar == 1){
        	printf("*** Sleeping %s \n" , cadena);
        }

        tiempo = strtol(cadena,NULL, 10);
        sleep(tiempo);

        if(depurar == 1){
        	printf("*** Sleeping End %s \n" , cadena);
        }
        return 1;
}

int c_ping(char* cadena){
        printf("%s \n" , cadena);
        return 1;
}

int c_pingServer(char* cadena){
        if(strstr(cadena, "PING")!= NULL)
                return 1;
        else
                return 0;
}

int c_pong(char* cadena){
        char* mensaje;
        char* aux = strtok(cadena, "PING ");
        int length;

        mensaje = "PONG ";

        mensaje = strcon(mensaje, aux, "\r\n", NULL, NULL);
        length = strlen(mensaje);
        enviar(mensaje, length);

        free(mensaje);

        return 1;
}

int c_error(char* cadena){
        if(strstr(cadena, "ERROR") != NULL)
                return 1;
        else
                return 0;
}

int c_privmsg(char* cadena){
        if(strstr(cadena, "PRIVMSG") != NULL)
                return 1;
        else
                return 0;
}

int c_joinServer(char* cadena){
        if(strstr(cadena, "JOIN") != NULL)
                return 1;
        else
                return 0;
}

int c_leaveServer(char* cadena){
        if(strstr(cadena, "PART") != NULL)
                return 1;
        else
                return 0;
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
        int leidos;
        int length;
        char* aux;
        char* aux2;
        int leer;
        aux = calloc(3, sizeof(char));
        (*mensaje) = calloc(2, sizeof(char));
        leer = 2;

        do{
                leidos = recv(serverConnected,aux,leer,0);

                if(leidos > 0){
                        length = strlen(*mensaje) + strlen(aux)+1;
                        aux2 = calloc(length, sizeof(char));
                        strcat(aux2,(*mensaje));
                        strcat(aux2,aux);
                        free(*mensaje);
                        (*mensaje) = calloc(length, sizeof(char));
                        strcpy(*mensaje, aux2);
                        free(aux2);

                        if(strpos(aux, "\r") > -1){
                                leer = 1;
                                free(aux);
                                aux = calloc(2, sizeof(char));
                        }else{
                                leer = 2;
                                free(aux);
                                aux = calloc(3, sizeof(char));
                        }
           ;
                }
        }while(strstr(*mensaje, "\n") == NULL && leidos > 0);

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
        }else if(c_leaveServer(mensaje) == 1){
            tipo = c_leaveMsg(mensaje);
        }else if(c_error(mensaje) == 1){
        	if(quit == 0){
        		tipo = parserError(mensaje);
        	}else
        		tipo = -1;
        }else if(c_privmsg(mensaje) == 1){
        	tipo = imprimir(mensaje);
        }else
        	tipo = parser(mensaje);
        free(mensaje);

       	return tipo;
}

int imprimir(char* mensaje) {
		char* user;
		char* cadena;

		strtok(mensaje, ":");
		strtok(mensaje, ":");
		user = strtok(mensaje, "!");
		strtok(mensaje, ":");
		cadena = strtok(mensaje, ":");
		printf("<%s> %s\n", user, cadena);

		return 0;
}

int c_leaveMsg(char* mensaje){
	char* name;
	char* channel;

	printf("%s \n", mensaje);

	strtok(mensaje, " ");
	strtok(NULL, " ");
	channel = strtok(NULL, ":");
	name = strtok(NULL, "\r");

	printf("*** %s has left channel %s \n", name, channel);

	return 451;
}

int parserError(char* mensaje) {

	printf("%s \n", mensaje);
	if(strstr(mensaje, "down") != NULL){
		printf("*** Connection to IRC server lost. Try to connect again in a few minutes\n");
		return 504;
	}
	return 0;
}

int parser(char* mensaje) {
        char* grupo;
        int codigo;
        char* cadena;
        char* aux;
        int pos;

        printf("%s \n", mensaje);

        grupo = strtok(mensaje, " ");
        if(grupo != NULL){
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
			}else if(codigo == 311){
				strtok(NULL, " ");
				grupo = strtok(NULL, " ");
				strtok(NULL, " ");
				aux = strtok(NULL, " ");
			}else if(codigo == 317){
				strtok(NULL, " ");
				strtok(NULL, " ");
				grupo = strtok(NULL, " ");
				aux = strtok(NULL, " ");
			}else if(codigo == 322 || codigo == 401){
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

			if(codigo > 200 && codigo != 311 && codigo != 317 && codigo != 322 && codigo != 332 && codigo != 353 && codigo != 352 && codigo != 401){
				grupo = NULL;
			}

			if(pos > 0){
				checkCodigo(grupo, codigo, cadena);
				free(cadena);
			}else
				checkCodigo(grupo, codigo, aux);

			return codigo;
        }else{
        	printf("*** Problem with the message's server. Exit\n");
        	return -1;
        }
}

/* Comprueba los codigos */
int checkCodigo(char* grupo, int codigo, char* cadena) {
	char* aux;
	char* aux2;
	switch(codigo){
		case 0:
			printf("*** %s \n", cadena);
			break;
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
        case 311:
        		printf("*** %s's IP address: %s \n", grupo, obtenerIpServer(cadena));
        		printf("*** %s's hostname: %s \n", grupo, cadena);
        		break;
        case 312:printf("*** %s \n", cadena);break;
		/* Final de la lista de usuarios, WHO*/
        case 315:break;
        case 317:printf("*** %s seconds idle, %s signon time \n", grupo, cadena);break;
		/* Final de la lista de usuarios, WHOIS*/
        case 318:break;
        case 319:printf("*** Connected Channel: %s \n", cadena);break;
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
        case 372:
        		printf("*** Message of the day from :\n");
        		printf("*** %s \n", cadena);break;
        /* Final del connect, CONNECT*/
        case 376:break;
        case 401:printf("*** %s is not in channel %s\n", grupo, channelname);break;
        case 412:printf("*** %s \n", cadena);break;
        case 421:printf("*** %s \n", cadena);break;
        case 432:printf("*** %s \n", cadena);break;
        case 433:printf("*** %s \n", cadena);break;
        case 451:printf("*** %s \n", cadena);break;
        case 474:printf("*** The channel does not exist");break;
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
                                if(pos == -1)
                                        pos = i;
                                /* Antes estaba puesto un else if*/
                                if (j == lengthsub){
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
char* strcon(char* mensaje, char* mensaje2, char* mensaje3, char* mensaje4, char* mensaje5) {
        char* aux;
        int length = 0;
        int length2 = 0;
        int length3= 0;
        int length4= 0;
        int length5= 0;

        if(mensaje != NULL){
                length = strlen(mensaje);
        }
        if(mensaje2 != NULL){
                length2 = strlen(mensaje2);
        }
        if(mensaje3 != NULL){
                length3 = strlen(mensaje3);
        }
        if(mensaje4 != NULL){
                length4 = strlen(mensaje4);
        }

        if(mensaje5 != NULL){
                length5 = strlen(mensaje5);
        }

        aux = calloc((length+length2+length3+length4+length5)+1, sizeof(char));

        if(mensaje != NULL){
        		strcat(aux, mensaje);
		}
		if(mensaje2 != NULL){
				strcat(aux, mensaje2);
		}
        if(mensaje3 != NULL){
                strcat(aux, mensaje3);
        }
        if(mensaje4 != NULL){
                strcat(aux, mensaje4);
        }
        if(mensaje5 != NULL){
                strcat(aux, mensaje5);
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

/**Busca la cadena y devuelve la subcadena comprendida*/
/*length es la cadena que se quiere obtener*/
int strsub(char* origen, char* dest, int posIni, int length){
        dest = calloc(length+posIni+1, sizeof(char));
        strncpy(dest, &origen[posIni], length);
        dest[length] = '\0';

        return 1;
}
