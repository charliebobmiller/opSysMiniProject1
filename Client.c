#include <arpa/inet.h> // internet address [inet_addr()]
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // to use bzero() function
#include <sys/socket.h>
#include <unistd.h> //  To use read(), write(), close() function
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
void cfunction(int sock_fd)
{
        char big[MAX];
        int m;
        for (;;) {
                bzero(big, sizeof(big));
                printf("Enter the Message : ");
                m = 0;
                while ((big[m++] = getchar()) != '\n')
                        ;
                write(sock_fd, big, sizeof(big));
                bzero(big, sizeof(big));
                read(sock_fd, big, sizeof(big));
                printf("Message from Server : %s", big);
                if ((strncmp(big, "exit", 4)) == 0) {
                printf("Processing to exit.....\n");
                printf("Client Exit...\n");
                        break;
                }
        }
}

int main()
{
        int socket_fd, connection_fd;
        struct sockaddr_in serv_addr, client;

        // For socket creation and verification in server client connection
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
                printf("Failed to Create the Socket...\n");
                exit(0);
        }
        else
                printf("Socket Creation Successful..\n");
        bzero(&serv_addr, sizeof(serv_addr));

        // To assign IP,and PORT for the client server connection
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(PORT);

        // To perform connection of client socket to server socket
        if (connect(socket_fd, (SA*)&serv_addr, sizeof(serv_addr))
                != 0) {
                printf("Server Connection failed...\n");
                printf("Exiting....\n");
                exit(0);
        }
        else
                printf("Server is Connected..\n");

        // function for message send and recieve
        cfunction(socket_fd);

        // Exiting the socket connection
        close(socket_fd);
}
