/**      (C)2000-2021 FEUP
 *       tidy up some includes and parameters
 * */

#include "url_parser.h"
#include "getip.h"
#include "tcp_utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s ftp://<user>:<pass>@<host>/<path> or ftp://<host>/<path> \n", argv[0]);
        return 1;
    }

    ftp_url url;
    if (parse_ftp_url(argv[1], &url) != 0) {
        printf("Error parsing URL.\n");
        return 1;
    }

    printf("User: %s\n", url.user);
    printf("Password: %s\n", url.password);
    printf("Host: %s\n", url.host);
    printf("Path: %s\n", url.path);

    char ip[64];
    if (resolve_hostname(url.host, ip, sizeof(ip)) != 0) {
        printf("Could not resolve host.\n");
        return 1;
    }

    printf("Resolved %s → %s\n", url.host, ip);

    int sockfd = connect_to_server(ip, FTP_PORT);
    if (sockfd < 0) return 1;

    // Receive FTP welcome line
    char line[1024];
    recv_ftp_response(sockfd, line, sizeof(line));

    // Fazer login
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "USER %s\r\n", url.user);
    send_all(sockfd, cmd, strlen(cmd));
    recv_ftp_response(sockfd, line, sizeof(line));

    snprintf(cmd, sizeof(cmd), "PASS %s\r\n", url.password);
    send_all(sockfd, cmd, strlen(cmd));
    recv_ftp_response(sockfd, line, sizeof(line));

    // Entrar no modo Passive
    char data_ip[64];
    int data_port;

    if (enter_passive_mode(sockfd, data_ip, &data_port) != 0) {
        fprintf(stderr, "Failed to enter passive mode\n");
        close(sockfd);
        return 1;
    }

    printf("Data connection IP: %s\n", data_ip);
    printf("Data connection port: %d\n", data_port);

    // Conectar a socket de output de dados
    int data_sock = connect_to_server(data_ip, data_port);
    if (data_sock < 0) {
        fprintf(stderr, "Failed to connect to data socket\n");
        close(sockfd);
        return 1;
    }

    // Mandar comando RETR para receber os dados
    snprintf(cmd, sizeof(cmd), "RETR %s\r\n", url.path);
    send_all(sockfd, cmd, strlen(cmd));
    recv_ftp_response(sockfd, line, sizeof(line));
    
    // Calcular tamanho total para a barra de progresso
    size_t total_size = 0;
    char *start = strchr(line, '(');
    char *end = strchr(line, ')');
    if (start && end && end > start) {
        *end = '\0';          // terminate string at ')'
        total_size = strtoull(start + 1, NULL, 10);
        printf("File size: %zu bytes\n", total_size);
    }

    char path[MAX_LENGTH+10];
    snprintf(path, sizeof(path), "Downloads/%s", url.file);

    // Fazer download do ficheiro
    if (download_file(data_sock, path, total_size) != 0) {
        fprintf(stderr, "Failed to download file\n");
        close(data_sock);
        close(sockfd);
        return 1;
    }

    printf("File '%s' downloaded successfully!\n", url.file);

    close(data_sock);

    // Ler mensagem final do servidor
    recv_ftp_response(sockfd, line, sizeof(line));

    close(sockfd);
    return 0;
}
