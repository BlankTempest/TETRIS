#include "defs.h"
#include "init.h"
#include "input.h"
#include "draw.h"
#include "piece.h"
#include "board.h"
#include "tetris.h"
#include "client.h"

int linesCompleted = 0;
int linesToAdd = 0;

SDL_mutex *mut;
SOCKET clientSocket;

int main(int argc, char* args[]) {

    //sets seed to be time variant
    srand(time(NULL));
    
    initializeSdlElements();

    bool quitCheck = false;
    bool running = true;

    SDL_Thread *clientThread = SDL_CreateThread(clientSideThread, "ClientThread", (void*)NULL); 

    //loop back to menu
    while (!quitCheck && running){
        //main menu screen
        running = main_menu();

        //create the board with 0 values
        int board[ROWS][COLS][5] = {0};

        //to prevent junk lines from appearing in a new game just in case
        SDL_LockMutex(mut);
        linesToAdd = 0;
        SDL_UnlockMutex(mut);

        //tetris effect begins
        if (running){
            quitCheck = gameplay(board);
        }
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();


    SDL_DetachThread(clientThread);

    //exit
    Mix_FreeMusic(bgsound);
    //Mix_FreeChunk(clear);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
