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

    int sockfd = connect_to_server(ip, 21);
    if (sockfd < 0) return 1;

    // Receive FTP welcome line
    char line[1024];
    recv_ftp_response(sockfd, line, sizeof(line));
    printf("FTP: %s", line);

    // Authenticate
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "USER %s\r\n", url.user);
    send_all(sockfd, cmd, strlen(cmd));
    recv_ftp_response(sockfd, line, sizeof(line));
    printf("FTP: %s", line);

    snprintf(cmd, sizeof(cmd), "PASS %s\r\n", url.password);
    send_all(sockfd, cmd, strlen(cmd));
    recv_ftp_response(sockfd, line, sizeof(line));
    printf("FTP: %s", line);

    // Entrar no modo Passive
    send_all(sockfd, "PASV\r\n", 6);
    recv_ftp_response(sockfd, line, sizeof(line));
    printf("FTP: %s", line);

    close(sockfd);
    return 0;
}
