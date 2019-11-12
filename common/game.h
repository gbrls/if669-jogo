#ifndef LIB_GAMELIB_H
#define LIB_GAMELIB_H

#define MAX_CHAT_CLIENTS 5

#define WIDTH 800
#define HEIGHT 800

typedef struct {
    float x;
    float y;
} GamePlayerState;

typedef struct {
    int active;
    unsigned char keyboard;
    GamePlayerState playerState;
} ClientState;

#endif
