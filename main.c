#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <time.h> // to set speed of the game and to append levels
#include <math.h>
#include <stdlib.h> //random + more

#include <stdio.h> //to debug and to get err messages
#include <stdbool.h> //convenience


//definitions
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define ROWS 20
#define COLS 10
#define ROW_SIZE 30
#define COL_SIZE 30
#define ROW_NEXT 15
#define COL_NEXT 15


//these are official tetromino(piece) terms
//the last row is the color component
int shape_i[5][4] = {
    {0,0,0,0},
    {0,0,0,0},
    {1,1,1,1},
    {0,0,0,0},
    {0,255,255,255}
};

int shape_j[5][4] = {
    {0,0,0,0},
    {1,0,0,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,255,255}
};

int shape_l[5][4] = {
    {0,0,1,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0},
    {255,127,0,255}
};

int shape_o[5][4] = {
    {0,0,0,0},
    {0,1,1,0},
    {0,1,1,0},
    {0,0,0,0},
    {255,255,0,255}
};

int shape_s[5][4] = {
    {0,1,1,0},
    {1,1,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,255,0,255}
};

int shape_t[5][4] = {
    {0,1,0,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0},
    {128,0,128,255}
};

int shape_z[5][4] = {
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0},
    {255,0,0,255}
};


//an array to randomly pick a shape
//piece_shape is an array of pointers, so it stores addresses
int (*piece_shape[7])[5][4] = {
    &shape_i,&shape_j,&shape_l,&shape_o,&shape_s,&shape_t,&shape_z
    };

//define a piece everytime with a random shape and color, so a struct
//id will be the shape address from piece_shape
//using said id, shape will be copied from piece_shape
typedef struct Piece{
    int id;
    int shape[5][4];
    int rotation;
    int x;
    int y;
}Piece;

//might be better to #define keyinputs instead
//struct's only useful if you want to create profiles
struct KeyInputs{
    int left;       //move piece left , (Left arrow key)
    int right;      //move piece right, (Right arrow key)
    int rotate_l;   //rotate anti-clockwise , (Up arrow key)
    int rotate_r;   //rotate clockwise , (Down arrow key)
    int soft_drop;  //(Shift or alt)
    int hard_drop;  //(spacebar)
    int hold;       //hold piece or swap piece, (C)
    int quit;       //exit to main menu(only for debugging) (Esc)
};


//initialize game window using library command
void initialize_window(); 

//i.e, shape with colour, x, y etc
void get_piece_props(SDL_Renderer *renderer, Piece *P1) {
    P1->id = rand() %7; //rand shape id
    for (int i=0; i<5; i++){
        for (int j=0; j<4; j++){
            //P1's shape array will now have a copied version of the shape
            //so that it doesn't mess with the original after rotation
            (P1->shape)[i][j] = (*piece_shape[P1->id])[i][j];
        }
    }

    //x and y of initial piece will start from the 4th grid and 1st col
    P1->x = (SCREEN_WIDTH / 2 - COLS*20) + (3*COL_SIZE);
    P1->y = COLS*5;
} 

//add collision checks here for when the piece rotates
void rotate_piece(Piece *P1){

    int temp[4][4];

    //we have to limit the pivot of the piece
    //so it'll be 3 for all pieces other than I and O
    int n = 3;
    if(P1->id == 0 || P1->id == 3){
        n =4;
    }

    for (int i =0; i<4; i++){
        for (int j =0; j<4; j++){
            temp[i][j] = P1->shape[i][j];
        }
    }

    //then we rotate it 90 degress within the 3x3 constraint
    for (int i =0; i<n; i++){
        for (int j =0; j<n; j++){
            P1->shape[i][j] = temp[n-j-1][i];
            //P1->shape[n-i-1][n-j-1] = temp[j][i]; //anti clockwise
        }
    }
}

//movement restriction so that it won't move outside the grid
int collision_check(int next_pieces[6]){
    //when we add the piece to the grid after collision or when we hold the piece
    //or when we hard drop the piece,
    //collision_check(next_pieces);

    for (int i = 0; i < (6-1); i++){
        next_pieces[i] = next_pieces[i+1];
    }
    int next_id = rand() %7; //rand shape id
    next_pieces[6] = next_id;

    //we move all the elements by 1 index and then 
        //fill the last element with a random id
} 

//score + misc updates
void draw_ui_elements(); 

//draws a grid
void draw_grid(); 

//clears rows when a row is full from bottom to top
void clear_rows(); 

//draws the next piece on the sidebar
void draw_next_piece(SDL_Renderer *renderer, int next_pieces[6]){
    //get_next_piece();
    for (int k = 0; k<6; k++){
        for (int i = 0; i<4; i++){
            for (int j = 0; j<4; j++){
                if ((*piece_shape[next_pieces[k]])[i][j]){
                    SDL_Rect rectNextPiece = {
                        //+8 is the x padding from the bg
                        //+4 is the x padding from the grid
                        .x = (SCREEN_WIDTH / 2 - COLS*20) + COL_SIZE*10 + 4 + (j*COL_NEXT) + 8,
                        .y = COLS*5 + ROW_SIZE*2 +i*ROW_NEXT + k*ROW_NEXT*4,    // l*nextrow*4 draws next piece after 4 rows
                        .w = COL_NEXT,
                        .h = ROW_NEXT
                    };

                    //getting the color from the shape array's 5th(last) row
                    int next_piece_R = (*piece_shape[next_pieces[k]])[4][0];
                    int next_piece_G = (*piece_shape[next_pieces[k]])[4][1];
                    int next_piece_B = (*piece_shape[next_pieces[k]])[4][2];
                    int next_piece_A = (*piece_shape[next_pieces[k]])[4][3];
                    SDL_SetRenderDrawColor(renderer, next_piece_R, next_piece_G, next_piece_B, next_piece_A);
                    SDL_RenderFillRect(renderer, &rectNextPiece);
                }
            }
        }
    }
    //below can be used to debug frame rate issues
    //SDL_RenderPresent(renderer);
}

//draws the porjection of where the piece may land
void draw_piece_projection(); 

//wasd, c, space, esc keys
int get_key_input(); 

//check time to increase the drop speed of the pieces
int difficulty(); 

bool main_menu(SDL_Renderer *renderer, SDL_Window *screen){

    //surfaces for the image to be displayed on
    SDL_Surface *imageSurface = NULL;  //image surface
    SDL_Surface *windowSurface = NULL; //canvas to display images on

    windowSurface = SDL_GetWindowSurface(screen);

    //load png
    imageSurface = IMG_Load("Images/menu_logo.png");
    if(imageSurface == NULL){
        printf("Image Error: '%s'\n", SDL_GetError());
    }

    bool running = true;
    SDL_Event event_running;
    while (running) {
        while(SDL_PollEvent(&event_running)) {
            if(event_running.type == SDL_QUIT){
                    //exits inits
                    SDL_FreeSurface(imageSurface);
                    SDL_FreeSurface(windowSurface);
                    imageSurface = NULL;
                    windowSurface = NULL;
                    return false;
            }
            else if(event_running.type == SDL_KEYDOWN){
                if (event_running.key.keysym.sym == SDLK_KP_ENTER
                    || event_running.key.keysym.sym ==SDLK_RETURN){
                    //continues to gameplay
                    SDL_FreeSurface(imageSurface);
                    SDL_FreeSurface(windowSurface);
                    imageSurface = NULL;
                    windowSurface = NULL;
                    return true;
                }
            }
            else{
                
                /* //bg doesn't work currently
                SDL_SetRenderDrawColor(renderer, 0,120,120,255);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);*/

                //blit image does work
                SDL_BlitSurface( imageSurface, NULL, windowSurface, NULL );
                //refreshes screen
                SDL_UpdateWindowSurface(screen);

            }
        }
    }
}

void draw(SDL_Renderer *renderer, Piece *P1, int next_pieces[6])
{
    //background color
    //changes the color of the pallete
    SDL_SetRenderDrawColor(renderer, 0,120,120,255);
    //flushes the screen with whatever colour's selected
    SDL_RenderClear(renderer);

    //creates the grid
    SDL_SetRenderDrawColor(renderer, 10,10,10,255);
    SDL_Rect gridRect = {
        .x = SCREEN_WIDTH / 2 - COLS*20,
        .y = COLS*5,
        .w = COLS*COL_SIZE,
        .h = ROWS*ROW_SIZE
    };
    //then draws the grid
    SDL_RenderFillRect(renderer, &gridRect);

    //
    //we will display the pieces here so that grid lines will overlap the pieces
    //

    //should go inside gameplay
    //draw the piece
    //you need to draw this in a while loop and present it at the end of the loop
        //you keep adjusting the x and y based on key press and y-- offset
    for (int i = 0; i<4; i++){
        for (int j = 0; j<4; j++){
            if ((P1->shape)[i][j]){ //replace this with the piece shape from get_piece_props
                SDL_Rect rectPiece = {
                    .x = P1->x + (j*COL_SIZE),
                    .y = P1->y + i*ROW_SIZE,
                    .w = COL_SIZE,
                    .h = ROW_SIZE
                };

                //fetching the color of the pieces
                int cur_piece_R = P1->shape[4][0];
                int cur_piece_G = P1->shape[4][1];
                int cur_piece_B = P1->shape[4][2];
                int cur_piece_A = P1->shape[4][3]; //always a const, not needed
                SDL_SetRenderDrawColor(renderer, cur_piece_R, cur_piece_G, cur_piece_B, cur_piece_A);
                SDL_RenderFillRect(renderer, &rectPiece);
            }
        }
    }


    //vertical grid lines
    for (int i=0; i<((COLS+1)*COL_SIZE); i+=COL_SIZE) {
        //creating the grid lines
        SDL_SetRenderDrawColor(renderer, 45,45,45,255);
        SDL_Rect gridLines = {
            .x = (SCREEN_WIDTH / 2 - COLS*20) + i,
            .y = COLS*5,
            .w = 2,
            .h = ROWS*ROW_SIZE
        };
        //drawing them
        SDL_RenderFillRect(renderer, &gridLines);
    }

    //horizontal grid lines
    for (int i=0; i<((ROWS+1)*ROW_SIZE); i+=ROW_SIZE) {
        //creating the grid lines
        SDL_SetRenderDrawColor(renderer, 45,45,45,255);
        SDL_Rect gridLines = {
            .x = SCREEN_WIDTH / 2 - COLS*20,
            .y = (COLS*5) + i,
            .w = COLS*COL_SIZE,
            .h = 2
        };
        //drawing them
        SDL_RenderFillRect(renderer, &gridLines);
    }

    //creates the bg for the next pieces
    SDL_SetRenderDrawColor(renderer, 10,10,10,255);
    SDL_Rect gridNextRect = {
        .x = (SCREEN_WIDTH / 2 - COLS*20) + COL_SIZE*10 + 4,
        .y = COLS*5 + ROW_SIZE*1,
        .w = 5*COL_NEXT,
        .h = 25*ROW_NEXT
    };
    SDL_RenderFillRect(renderer, &gridNextRect);

    //to draw the next 6 pieces
    draw_next_piece(renderer, next_pieces);

    //outlines for the next pieces
    //vertical outlines
    for (int i=0; i<5*COL_NEXT; i+=COL_NEXT) {
        //creating the grid lines
        SDL_SetRenderDrawColor(renderer, 10,10,10,255);
        SDL_Rect gridLines = {
            //+8 is the x padding from the bg
            //+4 is the x padding from the grid
            .x = (SCREEN_WIDTH / 2 - COLS*20) + COL_SIZE*10 + 4 + i + 8, 
            .y = COLS*5 + ROW_SIZE*1,
            .w = 1,
            .h = 25*ROW_NEXT
        };
        //drawing them
        SDL_RenderFillRect(renderer, &gridLines);
    }

    //horizontal outlines
    for (int i=0; i<26*ROW_NEXT; i+=ROW_NEXT) {
        //creating the grid lines
        SDL_SetRenderDrawColor(renderer, 10,10,10,255);
        SDL_Rect gridLines = {
            .x = SCREEN_WIDTH / 2 - COLS*20 + COL_SIZE*10 + 4,
            .y = (COLS*5) + i + ROW_SIZE*1,
            .w = 5*COL_NEXT,
            .h = 1
        };
        //drawing them
        SDL_RenderFillRect(renderer, &gridLines);
    }

    //updates the screen from the backbuffer
    SDL_RenderPresent(renderer);

}
void gameplay(SDL_Renderer *renderer, int (*board)[COLS]) {

    Piece  P1;
    //we get a random shape for our piece along with x,y on the board
    get_piece_props(renderer, &P1);

    int next_pieces[6], next_id;

    for (int i = 0; i<6; i++){
        next_id = rand() %7; //rand shape id
        next_pieces[i] = next_id;
    }

    //exit check
    bool running = true;
    SDL_Event event_running;
    while (running) {
        while(SDL_PollEvent(&event_running)) {
            if(event_running.type == SDL_QUIT){
                    running = false;
                    break;
            }
            else if(event_running.type == SDL_KEYDOWN){
                if (event_running.key.keysym.sym == SDLK_LEFT){
                    //collision_check();
                    //move left
                    if (P1.id == 3){
                        if (P1.x >= ((SCREEN_WIDTH / 2 - COLS*20))){
                            P1.x -= COL_SIZE;
                        }
                    }
                    else if (P1.x >= ((SCREEN_WIDTH / 2 - COLS*20) + (1*COL_SIZE))){
                        P1.x -= COL_SIZE;
                    }
                }
                //the else here makes a difference
                else if (event_running.key.keysym.sym == SDLK_RIGHT){
                    //collision_check();
                    //move right
                    int distFromBoundary = 1;
                    if (P1.id == 0){
                        distFromBoundary = 2;
                    }
                    if (P1.x <= ((SCREEN_WIDTH / 2 - COLS*20) + (7*COL_SIZE - distFromBoundary*COL_SIZE))){
                        P1.x += COL_SIZE;
                    }
                }
                else if (event_running.key.keysym.sym == SDLK_DOWN){
                    //collision_check();
                    //to soft drop the piece
                    P1.y += ROW_SIZE;
                }
                else if (event_running.key.keysym.sym == SDLK_UP){
                    //to rotate the piece
                    //collision_check();
                    rotate_piece(&P1);
                }
            }
            else{
                
                
                //frame rate
                Uint32 startTime=SDL_GetTicks();

                draw(renderer, &P1, next_pieces);

                //frame rate
                Uint32 frameTime=SDL_GetTicks()-startTime;
                if(frameTime<1000/60){
                    SDL_Delay(1000/60-frameTime);
                }
            }
        }
    }

}

void gameover();


int main(int argv, char** args) {

    //new seed for rand, should only be called once
    // will consider system time as the seed
    srand(time(NULL));

    //make the pieces offset --y so that it keeps falling,
        // get speed from difficulty()
    

    //initialize everything and then a check if it initialized properly
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error: SDL failed to initialize: '%s'\n", SDL_GetError());
        return 1;
    }

    //initialize sdl mixer
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)==-1) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
    }

    //load audio files
    Mix_Music *bgsound = Mix_LoadMUS("Music/Tetris99.mp3");
    if (bgsound == NULL) {
        printf("Mix_LoadMUS: %s\n", Mix_GetError());
    }
    //Mix_Chunk *clear = Mix_LoadWAV("clear.wav"); //sound effects

    //create the window
    SDL_Window *screen = SDL_CreateWindow(
        "TETRIS", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_WIDTH, SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN
    );

    //checks if the window was created
    if (screen == NULL) {
        printf("Failed to create the window: %s\n", SDL_GetError());
        return 1;
    }

    //initialize the image
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Image init error: %s\n", IMG_GetError());
    }

    //create the renderer
        //index -1 is the default gpu index
        //we use hardware acceleration and vsync
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    //adjust volume
    Mix_VolumeMusic(MIX_MAX_VOLUME*0.3);

    //play music
    if(Mix_PlayMusic(bgsound, -1)==-1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
    }

    //main menu screen
    bool running = main_menu(renderer, screen);
    
    //create the board with 0 values
    int board[ROWS][COLS] = {0};

    //gameplay begins
    //we send the board's pointer to the function
    if (running){
        gameplay(renderer, board);
    }
    
    //exit
    Mix_FreeMusic(bgsound);
    //Mix_FreeChunk(clear);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}

/*
Add the color components of the pieces to the shape arrays instead since they'll have fixed colors anyway
    this will be a pain since you'll have to add a 5th row which will mess up all for loops and such
    but is required.
Random pick the id of the shape from the pieces_shape array
Then use said id to copy the shape into Piece.shape instead of passing array address
Then perform transpose etc to that copy instead


----------------------------------I------------------------------

Could create a 20x10 grid for the board for collision checks. 
    This will take care of collisions, line fill checks, drops etc.

This board/grid will need to have it's values set to 1 where the piece is there.
the piece will need to toggle 1's for where it is initially, at 4th col, 1st row
then a check will happen as to which of the board's cells are 1
these cells will then need to be filled with the respective color
which i suppose will need a 3rd dimension k


As for moving the piece,
move them as is
do a check of if (piece[i][j]+board[][]) <=1, then the piece can move
the board's location is where the piece is present at so pass the piece's location
then we just do collision checks.
so we need to update the piece's x and y location that it occupies on the board
and offset this along with the piece's location
the check will need to be done before the piece moves to check the collision

we checks if the piece collides with any of the blocks and, if it collides on the sides,
then we make it so that it doesn't move left or right
if it collides with the bottom, that is the last occupuied row, then we add it to the board grid
and get rid of the rects.
if hold is pressed, then we can just take the piece out and put it in a temp place
the board doesn't need to be constantly updated with 1s
instead just needs collision checks


As for checking if lines are made

//eliminating lines
shift_board_down(int (*board)[COLS]){

    //the row below will take the values of the row above
    for (int i =ROWS-1; i>0; i--){
        for (int j = 0; j<COLS-1; j++){
            board[i][j] = board[i-1][j];
        }
    }

    //the first row will be emptied
    for (int j = 0; j<COLS-1; j++){
        board[0][j]=0;
    }

    //the color components will also need to be moved down
    //add that later, for now it'll be 0,0,0 if you try to draw it which is black
}

//checking if lines are full
line_full_check(board);
line_full_check(int (*board)[COLS], int* score){
    for (int j=ROWS-1; j>0; j--){

        int row_elements[]={0};
        int count = 0;

        for (int i= 0; i<COLS-1;i++){
            row_elements[i] = (*board)[j][i];  //k the color var will be ignored
        }

        if(row_elements[i]){
            count++;
        }

        if (count == COLS){
            shift_board_down(&board);
            //append score after this loop to check if other lines were also eliminated
            //append a line counter that will take values up to 4 to record the bonus(?)
            //i think 4 lines = 1200 points
            //*score +=400
        }
    }
}


dont use II
---------------------------------II------------------------------
or we could go the collision route.
drop:
we stop the pieces if the x value of the it is greater than the boundary location,
in which case we return out of the function, and bring the next piece in

collision:
we check each grid cell, that is the row_size and col_size, and check if any of the pieces is
on top of it. we'll have to record every single piece's x and y values into an array.
we remove these values when the piece is destroyed.
if it is on top, then it won't be able to move
if the bottom part of the piece collides with any piece, then we return out of the drop function,
record these values into the array
*/