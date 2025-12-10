// getip.h
#ifndef GETIP_H
#define GETIP_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int resolve_hostname(const char *hostname, char *ip_str, size_t maxlen);

#endif
