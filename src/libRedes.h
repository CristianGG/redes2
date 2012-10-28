
/* Autores:
 *
 * Alonso Luna, Miguel
 * Galan Galiano, Cristian
 *
 * */

#ifndef LIBREDES_H_
#define LIBREDES_H_

struct palabra {
    int cantidad;
    char** nombre;
} palabra;

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

char* obtenerIpServer(char* server);

#endif /* LIBREDES_H_ */
