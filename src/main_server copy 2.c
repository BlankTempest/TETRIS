//without instancing

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

    TCPsocket client_socket1 = NULL;
    TCPsocket client_socket2 = NULL;
    bool is_client1_connected = false;
    bool is_client2_connected = false;

    printf("Server started on port %d\n", port);

    while (true) {
        if (!is_client1_connected) {
            client_socket1 = SDLNet_TCP_Accept(server_socket);
            if (client_socket1) {
                printf("Client 1 connected\n");
                is_client1_connected = true;
            }
        }

        if (!is_client2_connected) {
            client_socket2 = SDLNet_TCP_Accept(server_socket);
            if (client_socket2) {
                printf("Client 2 connected\n");
                is_client2_connected = true;
            }
        }

        if (is_client1_connected && is_client2_connected) {
            char buffer[MAX_PACKET_SIZE];
            int bytes_received = SDLNet_TCP_Recv(client_socket1, buffer, MAX_PACKET_SIZE);
            if (bytes_received <= 0) {
                printf("Client 1 disconnected\n");
                is_client1_connected = false;
            }
            else {
                SDLNet_TCP_Send(client_socket2, buffer, bytes_received);
            }

            bytes_received = SDLNet_TCP_Recv(client_socket2, buffer, MAX_PACKET_SIZE);
            if (bytes_received <= 0) {
                printf("Client 2 disconnected\n");
                is_client2_connected = false;
            }
            else {
                SDLNet_TCP_Send(client_socket1, buffer, bytes_received);
            }
        }

        SDL_Delay(10);
    }

    SDLNet_TCP_Close(server_socket);
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}
