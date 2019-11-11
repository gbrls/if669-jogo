#include "keyboard.h"
#include "game.h"
#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>

#define mapWidth 900
#define mapHeight 900
#define WIDTH 800
#define HEIGHT 800
#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200

char game[120];

ALLEGRO_DISPLAY * window = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos=NULL;

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
  printf("Please enter your login (limit = %d): ", LOGIN_MAX_SIZE);
  scanf(" %[^\n]", login);
  getchar();
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

    window = al_create_display(WIDTH, mapHeight);
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

int main(){

    assertConnection();

    if(!inicializar()){
        return -1;
    }

    int sair=0;

    while(!sair){

        
        
        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.05);

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
            }

            byte = encodeKey(ktype, key);

            // Check confirmation byte
            if(byte&CONFIRMATION_BIT) {
                //TODO: check server's response
                sendMsgToServer((void*)&byte, 1);
            }
        }

        ClientState players[MAX_CHAT_CLIENTS];
        int ret = recvMsgFromServer(players, DONT_WAIT);
        if(ret == NO_MESSAGE) {
          //puts("NO MESSAGE");
          //printf("%0.2lf\n",al_get_time());
        } else {
          //printf("(%d), Recieved %d bytes, %d players\n",time(NULL),ret, ret/sizeof(ClientState));
          char* ptr = (char*) players;

          for(int i=0;i<ret;i++){
            //printf("%x%c",ptr[i],i==ret-1?'\n':' ');
          }
        }

        al_clear_to_color(al_map_rgb(0,0,0));

        for(int i=0;i<MAX_CHAT_CLIENTS;i++){
          if(players[i].active){
            al_draw_circle(players[i].playerState.x, players[i].playerState.y,
            10.0f, al_map_rgb(0xff, 0xff, 0xff),10.0f);
          }
        }

        al_flip_display();
    }

    //al_rest(1);

    return 0;
}
