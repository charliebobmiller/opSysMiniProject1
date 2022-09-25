#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Below function created for communication between client and server (2 way comminication).
void cfunction(int c_fd)
{
        char b[MAX];
        int N;
        // creating an infinite loop for communication.
        for (;;) {
                bzero(b, MAX);

                // reading message from client and copy it in to buffer(b)
                read(c_fd, b, sizeof(b));
                // printing buffer data.
                printf("Message from client: %s\t enter message for client : ", b);
                bzero(b, MAX);
                N = 0;
                // coping server response in to buffer(b).
                while ((b[N++] = getchar()) != '\n')
                        ;

                // sending response to client via buffer.
                write(c_fd, b, sizeof(b));

                // if response of server is "exit" connection is closed.
                if (strncmp("exit", b, 4) == 0) {
                        printf("Server disconnected\n");
                        break;
                }
        }
}

// main function
int main()
{
        int s_fd, c_fd, length;
        struct sockaddr_in serveraddress, client;

        // Here socket is created and verification is done.
        s_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (s_fd == -1) {
                printf("failed creation of socket\n");
                exit(0);
        }
        else
                printf("done creating socket\n");
        bzero(&serveraddress, sizeof(serveraddress));

        // Here we are assigning IP, PORT
        serveraddress.sin_family = AF_INET;
        serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddress.sin_port = htons(PORT);

        // Here binding of new socket to given IP address and verification is done.
        if ((bind(s_fd, (SA*)&serveraddress, sizeof(serveraddress))) != 0) {
                printf("binding failed try again\n");
                exit(0);
        }
        else
                printf("binding successfull\n");
        // Here server is ready to get some response from client and verification is done. 
        if ((listen(s_fd, 5)) != 0) {
                printf("Listening failed try again\n");
                exit(0);
        }
        else
                printf("Server waiting for response from clients:\n");
        length = sizeof(client);

        // Here the server is going to accept the data packet from client and verification is done.
        c_fd = accept(s_fd, (SA*)&client, &length);
        if (c_fd < 0) {
                printf("server is not able to connect\n");
                exit(0);
        }
        else
                printf("server is now connected to a client:\n");

        // This is the function for communication between client and server.
        cfunction(c_fd);

        // If the communication is completed close the socket. 
        close(s_fd);
}



