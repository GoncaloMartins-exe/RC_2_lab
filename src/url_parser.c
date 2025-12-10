#include "url_parser.h"

int parse_ftp_url(const char *url, ftp_url *out) {
    if (strncmp(url, "ftp://", 6) != 0) {
        fprintf(stderr, "Invalid URL: must begin with ftp://\n");
        return -1;
    }

    strcpy(out->user, DEFAULT_USER);
    strcpy(out->password, DEFAULT_PASS);
    out->host[0] = '\0';
    out->path[0] = '\0';
    out->file[0] = '\0';

    const char *p = url + 6; //Da skip no "ftp://"

    const char *at = strchr(p, '@');

    if (at != NULL) {
        char userpass[MAX_LENGTH*2+1];
        size_t len = at - p;
        if (len >= sizeof(userpass)) return -1;

        strncpy(userpass, p, len);
        userpass[len] = '\0';

        // user:password
        char *colon = strchr(userpass, ':');
        if (colon == NULL) {
            fprintf(stderr, "Invalid user/password format\n");
            return -1;
        }

        *colon = '\0';
        strcpy(out->user, userpass);
        strcpy(out->password, colon + 1);

        p = at + 1; // Passar para dps do @
    }

    const char *slash = strchr(p, '/');
    if (slash == NULL) {
        strcpy(out->host, p);
        strcpy(out->path, "");
        strcpy(out->file, p);
        return 0;
    }

    // Pegar o host
    size_t host_len = slash - p;
    if (host_len >= sizeof(out->host)) return -1;

    strncpy(out->host, p, host_len);
    out->host[host_len] = '\0';

    // Pegar o path (tudo dps do '/')
    strcpy(out->path, slash + 1);
    
    // Pegar o nome do ficheiro (tudo dps do ultimo '/')
    const char *last_slash = strrchr(out->path, '/');
    if (last_slash) {
        strcpy(out->file, last_slash + 1);
    } else {
        strcpy(out->file, out->path);
    }
    return 0;
}