#include "keyboard.h"
#include "game.h"
#include "client.h"
#include "raycast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200
#define RAIZ_3 1.7320508075688772
char game[120];

ALLEGRO_DISPLAY * window = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos=NULL;
enum GameRenderState game_render_state =GAME_MAP;

enum conn_ret_t tryConnect() {
  char server_ip[30];
  printf("Please enter the server IP: ");
  scanf(" %s", server_ip);
  getchar();
  return connectToServer(server_ip);
}

void assertConnection() {
  enum conn_ret_t ans = tryConnect();
  while (ans != SERVER_UP) {
    if (ans == SERVER_DOWN) {
      puts("Server is down!");
    } else if (ans == SERVER_FULL) {
      puts("Server is full!");
    } else if (ans == SERVER_CLOSED) {
      puts("Server is closed for new connections!");
    } else {
      puts("Server didn't respond to connection!");
    }
    printf("Want to try again? [Y/n] ");
    int res;
    while (res = tolower(getchar()), res != 'n' && res != 'y' && res != '\n'){
      puts("anh???");
    }
    if (res == 'n') {
      exit(EXIT_SUCCESS);
    }
    ans = tryConnect();
  }
  char login[LOGIN_MAX_SIZE + 4];
  ///printf("Please enter your login (limit = %d): ", LOGIN_MAX_SIZE);
  ///scanf(" %[^\n]", login);
  ///getchar();
  for(int i=0;i<LOGIN_MAX_SIZE;i++){
    login[i]=rand()%256;
  }
  int len = (int)strlen(login);
  sendMsgToServer(login, len + 1);
}

int inicializar() {
    if(!al_init()){
        fprintf(stderr, "Falha ao abrir biblioteca allegro\n");
        return 0;
    }
    if(!al_init_primitives_addon()){
        fprintf(stderr, "Falha ao abrir biblioteca de primitivas\n");
        return 0;
    }


    if (!al_install_keyboard()){
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return 0;
    }

    window = al_create_display(WIDTH, HEIGHT);
    if(!window){
        printf("Falha ao criar janela\n");
        return 0;
    }

    fila_eventos=al_create_event_queue();
    if(!fila_eventos){
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(window);
        return 0;
    }


    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(window));
    return 1;
}

void draw_map(unsigned int geladeiras) {
    for(int i=0;i<MAP_HEIGHT;i++){
        for(int j=0;j<MAP_WIDTH;j++){
            if(GameMap[j+i*MAP_WIDTH]=='#'){
                al_draw_filled_rectangle(j*MAP_SCALE,i*MAP_SCALE,
                        (j+1)*MAP_SCALE,(i+1)*MAP_SCALE,al_map_rgb(255,255,255));
            } else if(GameMap[j+i*MAP_WIDTH]>='0'&&GameMap[j+i*MAP_WIDTH]<='9'){
                int c=0;

                if(geladeiras&(1<<(GameMap[j+i*MAP_WIDTH]-'0'))) c = 255;

                al_draw_filled_rectangle(j*MAP_SCALE,i*MAP_SCALE,
                        (j+1)*MAP_SCALE,(i+1)*MAP_SCALE,al_map_rgb(c,255,0));

            }
        }
    }
}

int main(){

    srand(time(NULL));
    assertConnection();

    if(!inicializar()){
        return -1;
    }

    int sair=0;

    while(!sair){

        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.020);

        if(al_wait_for_event_until(fila_eventos, &evento, &timeout)){
            if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                sair=1;
            }

            unsigned char byte=0;
            int ktype=0, key=0;

            if(evento.type == ALLEGRO_EVENT_KEY_UP){
                ktype=KEYUP_TYPE;
                key=evento.keyboard.keycode;
            }

            if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                ktype=KEYDOWN_TYPE;
                key=evento.keyboard.keycode;

                if(key==ALLEGRO_KEY_ESCAPE){
                    if(game_render_state==GAME_MAP) {
                        game_render_state=GAME_RAYCAST;
                    } else {
                        game_render_state=GAME_MAP;
                    }
                }
            }

            byte = encodeKey(ktype, key);

            // Check confirmation byte
            if(byte&CONFIRMATION_BIT) {
                //TODO: check server's response
                sendMsgToServer((void*)&byte, 1);
            }
        }

        GameState state;
        //ClientState players[MAX_CHAT_CLIENTS];
        int ret = recvMsgFromServer(&state, DONT_WAIT);

        if(ret == NO_MESSAGE) {

        } else {

        }

        al_clear_to_color(al_map_rgb(0,0,0));

        if(game_render_state==GAME_MAP) {
            draw_map(state.geladeiras);

            for(int i=0;i<MAX_CHAT_CLIENTS;i++){
              if(state.players[i].active){

                float px = state.players[i].playerState.x;
                float py = state.players[i].playerState.y;
                float angle =  state.players[i].playerState.angle;

                al_draw_circle(px, py,
                        PLAYER_RADIUS, al_map_rgb(0, 0, 255),10.0f);

                al_draw_line(px, py,
                        px + cosf(angle)*PLAYER_VIEW_DIST,
                        py + sinf(angle)*PLAYER_VIEW_DIST,
                        al_map_rgb(255,0,0), 5);

                al_draw_rectangle(5,5,state.conta,10,
                        al_map_rgb(100,200,100),5);

              }
            }
        } else if(game_render_state==GAME_RAYCAST) {
          float px = state.players[0].playerState.x;
          float py = state.players[0].playerState.y;
          float angle =  state.players[0].playerState.angle;
          float dirX = 10*cosf(angle), planeY = RAIZ_3 * dirX;
          float dirY = 10*sinf(angle), planeX = -RAIZ_3 * dirY;
          al_clear_to_color(al_map_rgb(0,0,0));
          rayCasting(px, py, dirX, dirY, planeX, planeY);
        }

        al_flip_display();
    }

    return 0;
}
