#include "network.h"


struct addrinfo *query_addr()
{
    int status;
    struct addrinfo *res, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;      // IPv4 only
    hints.ai_socktype   = SOCK_STREAM;  // TCP sockets
    hints.ai_flags      = AI_PASSIVE;   // grab the IP for me pls
    

    if ((status = getaddrinfo(0, PORT_NUM, &hints, &res))) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    return res;
}

int establish_socket()
{

    int socketFd;
    struct addrinfo *res = query_addr(), *ptr;
    int yes=1;

    for(ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        if ((socketFd = socket(AF_INET, SOCK_STREAM, ptr->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        // enable reuse of connection.
        if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if ((bind(socketFd, ptr->ai_addr, ptr->ai_addrlen)) == -1)
        {
            close (socketFd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    if (ptr == NULL)
    {
        fprintf(stderr, "server: failed to bind.\n\n");
        exit(500);
    }

    return socketFd;
}

#define MAX_CONNECTIONS 8
void listen_socket(int fd)
{
    int cl_fd;
    struct sockaddr_storage cl_addr;
    socklen_t sin_len;

    if (listen(fd, 8) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("server: listening...\n");

    while(1) {
        sin_len = sizeof(cl_addr);
        cl_fd = accept(fd, (struct sockaddr *)&cl_addr, &sin_len);
        if (cl_fd == -1)
        {
            perror("accept");
            continue;
        }

        char s[INET_ADDRSTRLEN];
        inet_ntop(cl_addr.ss_family, &cl_addr, s, sizeof(s)); 
        printf("server: got connection from %s\n", s);

        if (send(cl_fd, "Hello, world!", 13, 0) == -1)
        {
            perror("send");
            exit(0);
        }

        close(cl_fd);
    }
}

int main(int argc, char *argv[])
{
    int socket;
    socket = establish_socket();
    listen_socket(socket);
    close(socket);

    return 0;
}
