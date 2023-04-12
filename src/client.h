#ifndef CLIENT_H
#define CLIENT_H

#include "defs.h"

extern int linesCompleted;
extern int linesToAdd;
extern SOCKET clientSocket;
extern SDL_mutex *mut;

int clientSideThread();

#endif