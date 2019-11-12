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

ClientState clients[MAX_CHAT_CLIENTS];

// Decoda um byte vindo do cliente
unsigned char process_byte(unsigned char prev, unsigned char new){

    unsigned char nib;
    if(new&KEY_BYTE_L) nib = KEY_BYTE_L;
    if(new&KEY_BYTE_R) nib = KEY_BYTE_R;
    if(new&KEY_BYTE_U) nib = KEY_BYTE_U;
    if(new&KEY_BYTE_D) nib = KEY_BYTE_D;

    if(new&KEYDOWN_TYPE){
       prev = prev|nib; 
    }

    if(new&KEYUP_TYPE){
        prev=prev & (~nib);
    }

    return prev;
}


int check_position(float x, float y) {

}

void update_players() {
    for(int i=0;isValidId(i);i++){
        if(clients[i].active){

          float spd=0.2;
          if(clients[i].keyboard&KEY_BYTE_L) {
              if(clients[i].playerState.x-spd>0) {
                  clients[i].playerState.x -= spd;
              }
          }
          if(clients[i].keyboard&KEY_BYTE_R) {
              if(clients[i].playerState.x+spd<WIDTH){
                  clients[i].playerState.x += spd;
              }
          }
          if(clients[i].keyboard&KEY_BYTE_U) {
              if(clients[i].playerState.y-spd>0){
                  clients[i].playerState.y -= spd;
              }
          }
          if(clients[i].keyboard&KEY_BYTE_D) {
              if(clients[i].playerState.y+spd<HEIGHT){
                  clients[i].playerState.y += spd;
              }
          }

        }
    }
}

void init_client(int id) {
    if(isValidId(id)){
        clients[id].active=1;
        clients[id].keyboard=0;

        clients[id].playerState.x=WIDTH/2;
        clients[id].playerState.y=HEIGHT/2;
    }
}

int main() {

  char client_names[MAX_CHAT_CLIENTS][LOGIN_MAX_SIZE];
  char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];


  serverInit(MAX_CHAT_CLIENTS);
  puts("Server is running!!");

  double prev=al_get_time();

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
      clients[msg_ret.client_id].keyboard=process_byte(clients[msg_ret.client_id].keyboard,incoming_byte);

    } else if (msg_ret.status == DISCONNECT_MSG) {
      clients[msg_ret.client_id].active=0;
      sprintf(str_buffer, "%s disconnected", client_names[msg_ret.client_id]);
      printf("%s disconnected, id = %d is free\n",
             client_names[msg_ret.client_id], msg_ret.client_id);
    }

    update_players();

    if(al_get_time()-prev > 0.05) {
      prev=al_get_time();
      broadcast(clients, sizeof(ClientState)*MAX_CHAT_CLIENTS);
    }
  }
}
