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

        if (n <= 0) return -1;

        buffer[pos++] = c;

        if (c == '\r') continue;
        if (c == '\n')break;
    }

    buffer[pos] = '\0';
    return pos;
}

// -----------------------------------------------------

int recv_ftp_response(int sockfd, char *buf, size_t buflen) {
    char line[1024];
    int code = 0;

    while (1) {
        if (recv_line(sockfd, line, sizeof(line)) <= 0) return -1;

        if (strlen(line) == 0) continue;

        printf("FTP: %s", line);

        if (code == 0) {
            code = atoi(line); 
        }

        // Multi-line responses have '-' after code
        if (line[3] == ' ')
            break;  // final line
    }

    if (buf && buflen > 0) {
        strncpy(buf, line, buflen - 1);
        buf[buflen - 1] = '\0';
    }

    return code;
}


int enter_passive_mode(int sockfd, char *ip_out, int *port_out) {
    char line[1024];

    // Mandar commando PASV para entrar no modo Passive
    if (send_all(sockfd, "PASV\r\n", 6) < 0) {
        fprintf(stderr, "Failed to send PASV command\n");
        return -1;
    }
    if (recv_ftp_response(sockfd, line, sizeof(line)) < 0) {
        fprintf(stderr, "Failed to read PASV response\n");
        return -1;
    }

    // Parse resposta 227
    int h1,h2,h3,h4,p1,p2;
    if (sscanf(line, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
               &h1,&h2,&h3,&h4,&p1,&p2) != 6) {
        fprintf(stderr, "Failed to parse PASV response: %s\n", line);
        return -1;
    }

    // Construir o IP e a Porta de saída dos dados
    snprintf(ip_out, 64, "%d.%d.%d.%d", h1,h2,h3,h4);
    *port_out = p1*256 + p2;

    return 0;
}

int download_file(int sockfd_data, const char *local_path) {
    FILE *fp = fopen(local_path, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    char buffer[4096];
    ssize_t n;

    while ((n = recv(sockfd_data, buffer, sizeof(buffer), 0)) > 0) {
        if (fwrite(buffer, 1, n, fp) != (size_t)n) {
            perror("fwrite");
            fclose(fp);
            return -1;
        }
    }

    if (n < 0) {
        perror("recv");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}
