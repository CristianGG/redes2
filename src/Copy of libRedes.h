
/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */

#ifndef LIBREDES_H_
#define LIBREDES_H_

#include <netdb.h>

int c_help();

int c_connect(int* serverConnected, fd_set* descriptorLectura, int* fdmax, char* servername, char* port);

int c_auth(char* cadena);

int c_list();

int c_join(char* cadena);

int c_leave(char* cadena);

int c_leaveServer(char* cadena);

int c_leaveMsg(char* cadena);

int c_who(char* cadena);

int c_info(char* cadena);

int c_msg(char* channelname, char* cadena);

int c_disconnect();

int c_quit();

int c_nop(int depurar, char* cadena);

int c_sleep(int depurar, char* cadena);

int c_ping();

int c_pingServer();

int c_pong();

int c_error();

int recibirMensaje();

int imprimir(char* mensaje);

int parser(char* mensaje);

char* obtenerIpServer(char* server);

int enviar(char* mensaje, int length);

int recibir(char** mensaje);

int checkCodigo(char* mensaje, int codigo, char* cadena);

char* strcon(char* mensaje, char* mensaje2, char* mensaje3, char* mensaje4, char* mensaje5);

int strpos(char* origen, char* substring);

int strrem(char* origen, char** dest, char* substring);

int strsub(char* origen, char* dest, int posIni, int length);

#endif /* LIBREDES_H_ */
