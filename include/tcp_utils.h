#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include <stddef.h>

// Abre a socket
int connect_to_server(const char *ip_str, int port);

// Mandar o conteudo do buffer pela socket
int send_all(int sockfd, const char *buffer, size_t length);

// Recebe uma unica linha (\n) da socket. Conexões FTP são "line-based"
int recv_line(int sockfd, char *buffer, size_t maxlen);

#endif
