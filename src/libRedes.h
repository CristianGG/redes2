
/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */

#ifndef LIBREDES_H_
#define LIBREDES_H_

#include <netdb.h>

void c_help();

void c_connect(int* serverConnected, fd_set* descriptorLectura, int* fdmax, char* servername, char* port);

void c_auth(char* cadena);

void c_list();

void c_join(char* cadena);

int c_joinServer(char* cadena);

void c_leave();

void c_who();

void c_info(char* cadena);

void c_msg(char* cadena);

void c_disconnect();

void c_quit();

void c_nop(char* cadena);

void c_sleep(char* cadena);

void c_ping();

int c_pingServer();

void c_pong();

void recibirMensaje();

char* concatenar(char* mensaje, char* mensaje2, char* mensaje3, char* mensaje4);

char* obtenerIpServer(char* server);

void enviar(char* mensaje, int length);

void recibir(char** mensaje);

void error(int codigo);

#endif /* LIBREDES_H_ */
