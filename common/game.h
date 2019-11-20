#ifndef LIB_GAMELIB_H
#define LIB_GAMELIB_H

#define MAX_CHAT_CLIENTS 5

#define WIDTH 800
#define HEIGHT 800

#define MAP_WIDTH 16
#define MAP_HEIGHT 16

#define MAP_SCALE 50

#define PLAYER_RADIUS 10
#define PLAYER_VIEW_DIST 100

#define NUM_GELADEIRAS 5

typedef struct {
    float angle;
    float x;
    float y;
} GamePlayerState;

typedef struct {
    int active;
    unsigned char keyboard;
    GamePlayerState playerState;
} ClientState;

typedef struct {
    ClientState players[MAX_CHAT_CLIENTS];
    unsigned char geladeiras;
    double conta;
    unsigned char id;
} GameState;

enum GameRenderState {
    GAME_MAP,
    GAME_RAYCAST,
};
enum estadoDoJogo
{
    menu,
    jogar,
    jogar_IP,
    HowPlay,
    contexto,
    sair,
    nada,
};
enum Hover
{
    sairHover,
    jogarHover,
    contextoHover,
    howPlayHover,
};

extern unsigned char GameMap[MAP_WIDTH][MAP_HEIGHT];

#endif
