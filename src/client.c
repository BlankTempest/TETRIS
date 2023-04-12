#include "client.h"

int clientSideThread(){

    WSADATA wsaData;
    int iResult;

    //winsock init
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        exit(EXIT_FAILURE);
    }

    //err hander for sock creation
    //ipv4,tcp,tcp
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    //server infomer
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);//conv to big-endian
    serverAddress.sin_addr.s_addr = inet_addr("192.168.124.208");//to binary

    //err handler while connecting to server
    iResult = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR) {
        printf("Unable to connect to server: %ld\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    char recvLines[1];

    while(true){

        SDL_LockMutex(mut);

        iResult = recv(clientSocket, recvLines, 1, 0);
        if (iResult > 0) {
            printf("Data received from server: %.*s\n", iResult, recvLines);
        }
        else if (iResult == 0) {
            printf("Connection closed by server.\n");
        }
        else {
            printf("Unable to receive data from server: %ld\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        
        printf("Recieved %s lines\n",recvLines);
        int temp = atoi(recvLines);
        linesToAdd +=temp;
        //printf("Recieved %d lines from server\n", linesToAdd);
        
        SDL_UnlockMutex(mut);

    }

    return 0;
}