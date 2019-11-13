#include "server.h"
#include "game.h"
#include "keyboard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13

GameState state;
//ClientState clients[MAX_CHAT_CLIENTS];

// Decoda um byte vindo do cliente
unsigned char process_byte(unsigned char prev, unsigned char new){

    unsigned char nib;
    if(new&KEY_BYTE_L) nib = KEY_BYTE_L;
    if(new&KEY_BYTE_R) nib = KEY_BYTE_R;
    if(new&KEY_BYTE_U) nib = KEY_BYTE_U;
    if(new&KEY_BYTE_D) nib = KEY_BYTE_D;
    if(new&KEY_BYTE_ACTION) nib = KEY_BYTE_ACTION;

    if(new&KEYDOWN_TYPE){
       prev = prev|nib; 
    }

    if(new&KEYUP_TYPE){
        prev=prev & (~nib);
    }

    return prev;
}

int check_map_collision(float x, float y) {
    int mx = x/(float)MAP_SCALE;
    int my = y/(float)MAP_SCALE;

    return GameMap[mx+my*MAP_WIDTH]!='.';
}

int check_collision(float x, float y) {
    if(x<0||y<0||x>WIDTH||y>HEIGHT||check_map_collision(x,y)) return 1;
    return 0;
}

void update_players() {
    for(int i=0;isValidId(i);i++){
        if(state.players[i].active){

          float spd=0.2,prevx=state.players[i].playerState.x,prevy=state.players[i].playerState.y;

          if(state.players[i].keyboard&KEY_BYTE_L) {
              state.players[i].playerState.x -= spd;
          }
          if(state.players[i].keyboard&KEY_BYTE_R) {
              state.players[i].playerState.x += spd;
          }
          if(state.players[i].keyboard&KEY_BYTE_U) {
              state.players[i].playerState.y -= spd;
          }
          if(state.players[i].keyboard&KEY_BYTE_D) {
              state.players[i].playerState.y += spd;
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
    }
}

int main() {

  char client_names[MAX_CHAT_CLIENTS][LOGIN_MAX_SIZE];
  char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];


  serverInit(MAX_CHAT_CLIENTS);
  puts("Server is running!!");

  double prev=al_get_time();

  state.geladeiras=0x4;

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
      state.players[msg_ret.client_id].keyboard=process_byte(state.players[msg_ret.client_id].keyboard,incoming_byte);

    } else if (msg_ret.status == DISCONNECT_MSG) {
      state.players[msg_ret.client_id].active=0;
      sprintf(str_buffer, "%s disconnected", client_names[msg_ret.client_id]);
      printf("%s disconnected, id = %d is free\n",
             client_names[msg_ret.client_id], msg_ret.client_id);
    }

    update_players();

    if(al_get_time()-prev > 0.05) {
      prev=al_get_time();
      broadcast(state.players, sizeof(GameState));
    }
  }
}
