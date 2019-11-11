#ifndef LIB_GAMELIB_H
#define LIB_GAMELIB_H

#define MAX_CHAT_CLIENTS 5

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
