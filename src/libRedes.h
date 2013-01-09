
/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */

#ifndef LIBREDES_H_
#define LIBREDES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int c_help();

int c_connect(int* serverConnected, fd_set* descriptorLectura, int* fdmax, char* servername, char* port);

int c_auth(char* cadena);

int c_list();

int c_join(char* cadena);

int c_joinServer(char* cadena);

int c_joinMsg(char* cadena);

int c_leave(char* cadena);

int c_leaveServer(char* cadena);

int c_leaveMsg(char* cadena);

int c_quitServer(char* cadena);

int c_quitMsg(char* cadena);

int c_who(char* cadena);

int c_info(char* cadena);

int c_msg(char* channelname, char* cadena);

int c_disconnect(fd_set* descriptorLectura, char* channelname, char* port);

int c_quit(fd_set* descriptorLectura);

int c_nop(int depurar, char* cadena);

int c_sleep(int depurar, char* cadena);

int c_ping();

int c_pingServer();

int c_pong();

int c_error();

int recibirMensaje(fd_set* descriptorLectura);

int imprimir(char* mensaje);

int parserError(char* mensaje);

int parser(char* mensaje, fd_set* descriptorLectura);

char* obtenerIpServer(char* server);

int enviar(char* mensaje, int length);

int recibir(char** mensaje);

int liberar(fd_set* descriptorLectura);

int checkCodigo(char* mensaje, int codigo, char* cadena);

char* strcon(char* mensaje, char* mensaje2, char* mensaje3, char* mensaje4, char* mensaje5);

int strpos(char* origen, char* substring);

int strrem(char* origen, char** dest, char* substring);

int strsub(char* origen, char* dest, int posIni, int length);

#endif /* LIBREDES_H_ */
