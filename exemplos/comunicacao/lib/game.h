#ifndef LIB_GAME_H
#define LIB_GAME_H

#define MAX_PLAYERS 20

typedef struct{
    int x;
    int y;

}Game_Vec2;

typedef struct {

    int players;
    Game_Vec2 board[MAX_PLAYERS];

}Game_State;



#endif