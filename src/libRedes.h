
/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */

#ifndef LIBREDES_H_
#define LIBREDES_H_

void c_help();

void c_connect(int* serverConnected, char* servername, int port);

void c_auth(char* cadena);

void c_list();

void c_join(char* cadena);

void c_leave();

void c_who();

void c_info(char* cadena);

void c_msg(char* cadena);

void c_disconnect();

void c_quit();

void c_nop(char* cadena);

void c_sleep(char* cadena);

void connectChannel();

char* concatenar(char* mensaje, char* mensaje2, char* mensaje3, char* mensaje4);

char* obtenerIpServer(char* server);

void enviar();

#endif /* LIBREDES_H_ */
