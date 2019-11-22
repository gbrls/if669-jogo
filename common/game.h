#ifndef LIB_GAMELIB_H
#define LIB_GAMELIB_H

#define MAX_CHAT_CLIENTS 5

#define WIDTH 1366
#define HEIGHT 768

#define MAP_WIDTH 32
#define MAP_HEIGHT 32

#define MAP_SCALE 50

#define PLAYER_RADIUS 10
#define PLAYER_VIEW_DIST 100

#define NUM_GELADEIRAS 5

typedef struct {
    float angle;
    float x;
    float y;
    
    unsigned char froze; /* Mecanica de congelamento */
} GamePlayerState;

typedef struct {
    int active;
    unsigned char keyboard;
    GamePlayerState playerState;
} ClientState;

typedef struct {
    ClientState players[MAX_CHAT_CLIENTS]; 
    unsigned char geladeiras; /* Guarda o estado das geladeiras */
    
    double conta;
    double elapsed; /* Quanto tempo de jogo se passou */
    
    /* ID modificado para jogador que se manda,
     para cada jogador saber o seu ip */
    unsigned char id; 
    unsigned char jaquin; /* ID jo jauqin */
    unsigned char ended; /* Já acabou o jogo? 0: não, 1: chefs: 2 jaquin */
    unsigned char started;
    unsigned char n_players;

    float time; // Tempo em que a mensagem foi enviada
} GameState;

enum GameRenderState {
    GAME_MAP,
    GAME_RAYCAST,
};
enum estadoDoJogo {
    menu,
    jogar,
    jogar_IP,
    HowPlay,
    contexto,
    sair,
    nada,
    tela_vitoria,
    waiting_for_players,
    abertura,
};
enum Hover {
    sairHover,
    jogarHover,
    contextoHover,
    howPlayHover,
};

extern unsigned char GameMap[MAP_WIDTH][MAP_HEIGHT];

#endif
