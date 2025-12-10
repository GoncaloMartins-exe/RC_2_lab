#ifndef URL_PARSER_H
#define URL_PARSER_H

#include <string.h>
#include <stdio.h>

#define MAX_LENGTH 512

#define DEFAULT_USER "anonymous"
#define DEFAULT_PASS "password"

typedef struct {
    char user[MAX_LENGTH];
    char password[MAX_LENGTH];
    char host[MAX_LENGTH];
    char path[MAX_LENGTH];
    char file[MAX_LENGTH];
} ftp_url;

// Da parse no url de input para depois processar e utilizar no envio da request
int parse_ftp_url(const char *url, ftp_url *out);

#endif
