#include "getip.h"

int resolve_hostname(const char *hostname, char *ip_str, size_t maxlen) {
    struct hostent *h;
    char *host_address;

    if ((h = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        return -1;
    }

    host_address = h->h_addr_list[0];

    // Convert binary address to string
    struct in_addr *addr = (struct in_addr *) host_address;
    const char *result = inet_ntoa(*addr);

    if (!result) return -1;

    strncpy(ip_str, result, maxlen - 1);
    ip_str[maxlen - 1] = '\0';

    return 0;
}
