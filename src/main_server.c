#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SDL.h"
#include "SDL_net.h"

#define MAX_PACKET_SIZE 1024
#define MAX_INSTANCES 10

typedef struct {
    TCPsocket client_socket1;
    TCPsocket client_socket2;
    bool is_client1_connected;
    bool is_client2_connected;
} GameInstance;

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
    char hostname[20] = "127.0.0.1";
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

    GameInstance instances[MAX_INSTANCES];
    int num_instances = 0;

    while (true) {
        //look for any incoming connections
        TCPsocket client_socket = SDLNet_TCP_Accept(server_socket);
        if (client_socket) {
            printf("New client connected\n");

            //try to find an available game instance to join
            bool joined_instance = false;
            for (int i = 0; i < num_instances; i++) {
                GameInstance* instance = &instances[i];
                if (!instance->is_client1_connected) {
                    instance->client_socket1 = client_socket;
                    instance->is_client1_connected = true;
                    printf("Client 1 joined game instance %d\n", i);
                    joined_instance = true;
                    break;
                } else if (!instance->is_client2_connected) {
                    instance->client_socket2 = client_socket;
                    instance->is_client2_connected = true;
                    printf("Client 2 joined game instance %d\n", i);
                    joined_instance = true;
                    break;
                }
            }

            //if no available instance was found, create a new one
            if (!joined_instance && num_instances < MAX_INSTANCES) {
                GameInstance* instance = &instances[num_instances++];
                instance->client_socket1 = client_socket;
                instance->is_client1_connected = true;
                printf("New game instance %d created\n", num_instances - 1);
            }
        }

        //handle client messages for each game instance
        for (int i = 0; i < num_instances; i++) {
            GameInstance* instance = &instances[i];
            if (instance->is_client1_connected && instance->is_client2_connected) {
                char buffer[MAX_PACKET_SIZE];

                //receive message from client 1 and send it to client 2
                int bytes_received = SDLNet_TCP_Recv(instance->client_socket1, buffer, MAX_PACKET_SIZE);
                if (bytes_received <= 0) {
                    printf("Client 1 disconnected from game instance %d\n", i);
                    instance->is_client1_connected = false;
                    SDLNet_TCP_Close(instance->client_socket1);
                    if (instance->is_client2_connected) {
                        SDLNet_TCP_Send(instance->client_socket2, "Opponent disconnected\n", 22);
                    }
                    continue;
                }
                //printf("%s\n",buffer+bytes_received-1);
                if(buffer[bytes_received-1]=='9'){
                    printf("client 2 wins\n");
                    instance->is_client1_connected = false;
                    SDLNet_TCP_Close(instance->client_socket1);
                    instance->is_client2_connected = false;
                    SDLNet_TCP_Close(instance->client_socket2);
                }
                SDLNet_TCP_Send(instance->client_socket2, buffer, bytes_received);
                //receive message from client 2 and send to client 1
                bytes_received = SDLNet_TCP_Recv(instance->client_socket2, buffer, MAX_PACKET_SIZE);
                if (bytes_received <= 0) {
                    printf("Client 2 disconnected from game instance %d\n", i);
                    instance->is_client2_connected = false;
                    SDLNet_TCP_Close(instance->client_socket2);
                    if (instance->is_client1_connected) {
                        SDLNet_TCP_Send(instance->client_socket1, "Opponent disconnected\n", 22);
                    }
                    continue;
                }
                //printf("%s\n",buffer+bytes_received-1);
                if(buffer[bytes_received-1]=='9'){
                    printf("client 1 wins\n");
                    instance->is_client1_connected = false;
                    SDLNet_TCP_Close(instance->client_socket1);
                    instance->is_client2_connected = false;
                    SDLNet_TCP_Close(instance->client_socket2);
                }
                SDLNet_TCP_Send(instance->client_socket1, buffer, bytes_received);
            }
        }

        SDL_Delay(10);
    }

    //close all client sockets and cleanup
    for (int i = 0; i < num_instances; i++) {
        if (instances[i].is_client1_connected) {
            SDLNet_TCP_Close(instances[i].client_socket1);
        }
        if (instances[i].is_client2_connected) {
            SDLNet_TCP_Close(instances[i].client_socket2);
        }
    }

    SDLNet_TCP_Close(server_socket);
    SDLNet_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}