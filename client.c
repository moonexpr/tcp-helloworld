#include "network.h"
#include <netinet/in.h>


struct addrinfo *query_addr(char *hostname)
{
    int status;
    struct addrinfo *res, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;      // IPv4 only
    hints.ai_socktype   = SOCK_STREAM;  // TCP sockets
    hints.ai_flags      = AI_PASSIVE;   // grab the IP for me pls
    

    if ((status = getaddrinfo(hostname, PORT_NUM, &hints, &res))) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    return res;
}

int establish_socket(char *hostname)
{

    int socketFd;
    struct addrinfo *res = query_addr(hostname), *ptr;
    ;

    for(ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        if ((socketFd = socket(AF_INET, SOCK_STREAM, ptr->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if ((connect(socketFd, ptr->ai_addr, ptr->ai_addrlen)) == -1)
        {
            close (socketFd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (ptr == NULL)
    {
        fprintf(stderr, "client: failed to connect to %s.\n\n", hostname);
        exit(404);
    }

    char s[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, ptr->ai_addr, s, sizeof(s));
    printf("client: connecting to %s\n", s);

    freeaddrinfo(res);
    return socketFd;
}

void read_socket(int fd)
{
    int bytes;
    char buf[MAX_BUFSIZE];

    if ((bytes = recv(fd, buf, MAX_BUFSIZE - 1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }

    // null terminate string.
    buf[bytes] = 0;
    printf("> %s\n\n", buf);
}

int main(int argc, char *argv[])
{
    // Weed out invalid arguments real fast.
    if (argc != 2) {
        fprintf(stderr,"usage: %s <hostname>\n", argv[0]);
        exit(1);
    }

    int socket;
    socket = establish_socket(argv[1]);
    read_socket(socket);
    close(socket);

    return 0;
}
