//fork var

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SDL.h"
#include "SDL_net.h"

#define MAX_PACKET_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (SDL_Init(0) == -1) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (SDLNet_Init() == -1) {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, port) == -1) {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    TCPsocket server_socket = SDLNet_TCP_Open(&ip);
    if (!server_socket) {
        printf("SDLNet_TCP_Open failed: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", port);

    int num_clients = 0;

    while (true) {
        TCPsocket client_socket = SDLNet_TCP_Accept(server_socket);
        if (client_socket) {
            printf("Client %d connected\n", num_clients + 1);

            if (num_clients % 2 == 0) {
                //create a new instance of server
                pid_t pid = fork();
                if (pid == 0) {
                    char port_str[10];
                    snprintf(port_str, 10, "%d", port + num_clients/2 + 1);

                    char *args[] = { argv[0], port_str, NULL };
                    execv(argv[0], args);
                }
            }

            char buffer[MAX_PACKET_SIZE];
            int bytes_received;
            while ((bytes_received = SDLNet_TCP_Recv(client_socket, buffer, MAX_PACKET_SIZE)) > 0) {
                //send the message to the other client
                int other_client_num = (num_clients % 2 == 0) ? num_clients + 1 : num_clients - 1;
                TCPsocket other_client_socket = SDLNet_TCP_Open(&ip);
                if (!other_client_socket) {
                    printf("SDLNet_TCP_Open failed: %s\n", SDLNet_GetError());
                    exit(EXIT_FAILURE);
                }
                if (SDLNet_TCP_Connect(other_client_socket, &ip, port + other_client_num/2 + 1) == -1) {
                    printf("SDLNet_TCP_Connect failed: %s\n", SDLNet_GetError());
                    exit(EXIT_FAILURE);
                }
                SDLNet_TCP_Send(other_client_socket, buffer, bytes_received);
                SDLNet_TCP_Close(other_client_socket);
            }

            printf("Client %d disconnected\n", num_clients + 1);
            SDLNet_TCP_Close(client_socket);
            num_clients++;
        }
    }

    SDLNet_TCP_Close(server_socket);
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}
