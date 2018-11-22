#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <errno.h>

#define HLEN 4
#define MAXMENS 1024

int leer_n_bytes(int sd, void *buffer, int n)
{
    int dev;
    int leido;

    dev   = 1;
    leido = 0;

    while ( ( leido < n ) && ( dev != 0 ) ) {

        dev = recv(sd, buffer+leido, (n-leido) , 0);

        if (dev < 0) {

             perror("Error en recv");

             exit(1);

        }

        leido += dev;

    }

    return (leido);

}

int leer_mensaje(int sd, char *mens)
{

    int n;
    uint32_t lon;

    n = leer_n_bytes (sd, &lon, HLEN );

    lon = ntohl(lon);

    if (n != 0) {

        n = leer_n_bytes (sd, mens, lon);

    }

    return (n);

}

int enviar_mensaje(int sd, char *mens)
{
    int n;
    uint32_t  lon;
    uint32_t  lon_net;
    char buffer[MAXMENS];

    lon = strlen(mens)+1;
    lon_net = htonl( lon );

    memcpy ( buffer , &lon_net , HLEN );

    memcpy ( buffer + HLEN , mens , lon );

    n = send (sd, buffer, (lon + HLEN) , 0);

    return (n);

}


