#include "client.h"

int clientSideThread(){

    IPaddress ip;

    SDLNet_ResolveHost(&ip, "127.0.0.1", 1234);

    char recvLines[1];

    client = SDLNet_TCP_Open(&ip);

    if (!client) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }


    while(true){

        SDL_LockMutex(mut);

        SDLNet_TCP_Recv(client, recvLines, 1);
        printf("%s\n",recvLines);
        int temp = atoi(recvLines);
        linesToAdd +=temp;
        //printf("Recieved %d lines from server\n", linesToAdd);
        
        SDL_UnlockMutex(mut);

    }

    return 0;
}