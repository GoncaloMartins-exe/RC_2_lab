#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include <stddef.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_PORT 21

// Abre a socket
int connect_to_server(const char *ip_str, int port);

// Mandar o conteudo do buffer pela socket
int send_all(int sockfd, const char *buffer, size_t length);

// Recebe uma unica linha (\n) da socket. Conexões FTP são "line-based"
int recv_line(int sockfd, char *buffer, size_t maxlen);

// Da print em multiplas linhas da socket. (Util para respostas com mais de uma linha)
int recv_ftp_response(int sockfd, char *buf, size_t buflen);

#endif
