#include "tcp_utils.h"

int connect_to_server(const char *ip_str, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    // Cria a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // Prepara o endereço
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_str, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    // Conecta
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// -----------------------------------------------------

int send_all(int sockfd, const char *buffer, size_t length) {
    size_t total_sent = 0;

    while (total_sent < length) {
        ssize_t sent = send(sockfd, buffer + total_sent, length - total_sent, 0);

        if (sent < 0) {
            perror("send");
            return -1;
        }

        total_sent += sent;
    }

    return total_sent;
}

// -----------------------------------------------------

int recv_line(int sockfd, char *buffer, size_t maxlen) {
    size_t pos = 0;

    while (pos < maxlen - 1) {
        char c;
        ssize_t n = recv(sockfd, &c, 1, 0);

        if (n <= 0)
            return -1;

        buffer[pos++] = c;

        if (c == '\n')
            break;
    }

    buffer[pos] = '\0';
    return pos;
}

// -----------------------------------------------------

int recv_ftp_response(int sockfd, char *buf, size_t buflen) {
    char line[1024];
    int code = 0;

    while (1) {
        if (recv_line(sockfd, line, sizeof(line)) <= 0)
            return -1;

        printf("FTP: %s", line);

        if (code == 0) {
            code = atoi(line);  // first 3 digits
        }

        // Multi-line responses have '-' after code
        if (line[3] == ' ')
            break;  // final line
    }

    return code;
}

