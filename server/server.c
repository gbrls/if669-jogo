#include "server.h"
#include "game.h"
#include "keyboard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <allegro5/allegro.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13

GameState state;

int check_map_collision(float x, float y) {
    int mx = x/(float)MAP_SCALE;
    int my = y/(float)MAP_SCALE;

    if(GameMap[mx][my]!='.'){

        if(GameMap[mx][my] >='0'&&GameMap[mx][my] <= '9'){
            return  (GameMap[mx][my]-'0')+2;
        }

        return 1;

    } else {
        return 0;
    }
}

void toggle_geladeiras(float x, float y, float angle) {
    //printf("%02.f, %02.f, %0.2f\n",x,y,angle);
    float nx=x,ny=y,step=0.3;

    while(((nx-x)*(nx-x) + (ny-y)*(ny-y)) < PLAYER_VIEW_DIST*PLAYER_VIEW_DIST) {

        nx += cosf(angle)*step;
        ny += sinf(angle)*step;

        int ret = check_map_collision(nx,ny);
        if(ret>=1) {
            if(ret>1) {
                ret-=2;
                state.geladeiras^=(1<<ret);

            }
            return;
        }
    }
}

// Decoda um byte vindo do cliente
unsigned char process_byte(int id, unsigned char prev, unsigned char new){


    unsigned char nib;
    if(new&KEY_BYTE_L) nib = KEY_BYTE_L;
    if(new&KEY_BYTE_R) nib = KEY_BYTE_R;
    if(new&KEY_BYTE_U) nib = KEY_BYTE_U;
    if(new&KEY_BYTE_D) nib = KEY_BYTE_D;
    if(new&KEY_BYTE_ACTION) nib = KEY_BYTE_ACTION;

    if(new&KEYDOWN_TYPE){
       prev = prev|nib;

       if(new&KEY_BYTE_ACTION) {
           toggle_geladeiras(state.players[id].playerState.x,
                   state.players[id].playerState.y,
                   state.players[id].playerState.angle);
       }
       //if(new&KEY_BYTE_ACTION) state.geladeiras^=0xff;
    }

    if(new&KEYUP_TYPE){
        prev=prev & (~nib);
    }

    return prev;
}



int check_collision(float x, float y) {
    if(x<0||y<0||x>WIDTH||y>HEIGHT||check_map_collision(x,y)) return 1;
    return 0;
}


void update_players() {
    for(int i=0;i<MAX_CHAT_CLIENTS;i++){
        if(state.players[i].active){

          float spd=0.2,rotspd=0.002;
          float prevx=state.players[i].playerState.x,prevy=state.players[i].playerState.y;

          if(state.players[i].keyboard&KEY_BYTE_L) {
              state.players[i].playerState.angle -= rotspd;
          }
          if(state.players[i].keyboard&KEY_BYTE_R) {
              state.players[i].playerState.angle += rotspd;
          }
          if(state.players[i].keyboard&KEY_BYTE_U) {
              float ang= state.players[i].playerState.angle;

              state.players[i].playerState.x += cosf(ang)*spd;
              state.players[i].playerState.y += sinf(ang)*spd;

              //state.players[i].playerState.y -= spd;
          }
          if(state.players[i].keyboard&KEY_BYTE_D) { 
              //TODO:
              float ang= state.players[i].playerState.angle;

              state.players[i].playerState.x -= cosf(ang)*spd;
              state.players[i].playerState.y -= sinf(ang)*spd;
          }

          if(check_collision(state.players[i].playerState.x,state.players[i].playerState.y)) {
              state.players[i].playerState.x=prevx;
              state.players[i].playerState.y=prevy;
          }

        }
    }
}

void init_client(int id) {
    if(isValidId(id)){
        state.players[id].active=1;
        state.players[id].keyboard=0;

        state.players[id].playerState.x=WIDTH/2;
        state.players[id].playerState.y=HEIGHT/2;
        state.players[id].playerState.angle=0.0f;

    }
}

void update_game_state(double delta_time) {
    //printf("%g\n",state.conta);

    double K=5.0;
    state.elapsed += delta_time;

    for(int i=0;i<NUM_GELADEIRAS;i++){
        if(state.geladeiras & (1<<i)){
            state.conta += delta_time*K;
        }
    }
}

int main() {

  char client_names[MAX_CHAT_CLIENTS][LOGIN_MAX_SIZE];
  char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];


  serverInit(MAX_CHAT_CLIENTS);
  puts("Server is running!!");

  double prev_broadcast_time=al_get_time(), prev_update_time=al_get_time();

  //state.geladeiras=0x4; /* geladeira nmuber 3 starts on */
  state.elapsed=0;
  while (1) {

    int id = acceptConnection();
    if (id != NO_CONNECTION) {
      recvMsgFromClient(client_names[id], id, WAIT_FOR_IT);
      printf("%s logged in!\n", client_names[id]);
      init_client(id);
    }

    unsigned char incoming_byte;
    struct msg_ret_t msg_ret = recvMsg(&incoming_byte);

    if (msg_ret.status == MESSAGE_OK) {
      printf("Recieved 0x%x from %d\n", incoming_byte, msg_ret.client_id);
      state.players[msg_ret.client_id].keyboard=process_byte(msg_ret.client_id,state.players[msg_ret.client_id].keyboard,incoming_byte);

    } else if (msg_ret.status == DISCONNECT_MSG) {
      state.players[msg_ret.client_id].active=0;
      sprintf(str_buffer, "%s disconnected", client_names[msg_ret.client_id]);
      printf("%s disconnected, id = %d is free\n",
             client_names[msg_ret.client_id], msg_ret.client_id);
    }

    update_players();
    update_game_state(al_get_time()-prev_update_time);

    if(al_get_time()-prev_broadcast_time > 0.05) {
      prev_broadcast_time=al_get_time();

        // a little gambiarra :)
        for (int i = 0; i < MAX_CHAT_CLIENTS; ++i) {
            if (isValidId(i)) {
                state.id=i;
                sendMsgToClient(&state, sizeof(GameState), i);
            }
        }

      //broadcast(state.players, sizeof(GameState));
    }

    prev_update_time = al_get_time();

  }
}
