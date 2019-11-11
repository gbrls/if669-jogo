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


void update_players() {
    for(int i=0;isValidId(i);i++){
        if(clients[i].active){
          float spd=0.2;
          if(clients[i].keyboard&KEY_BYTE_L) clients[i].playerState.x -= spd;
          if(clients[i].keyboard&KEY_BYTE_R) clients[i].playerState.x += spd;
          if(clients[i].keyboard&KEY_BYTE_U) clients[i].playerState.y -= spd;
          if(clients[i].keyboard&KEY_BYTE_D) clients[i].playerState.y += spd;
        }
                        //printf("Player %d, at (%0.2f, %0.2f)\n",i,
                //clients[i].playerState.x,
                //clients[i].playerState.y);

          //sendMsgToClient(clients, sizeof(ClientState)*MAX_CHAT_CLIENTS, i);
        
    }
}

int main() {

  char client_names[MAX_CHAT_CLIENTS][LOGIN_MAX_SIZE];
  char str_buffer[BUFFER_SIZE], aux_buffer[BUFFER_SIZE];


  serverInit(MAX_CHAT_CLIENTS);
  puts("Server is running!!");

  double prev=al_get_time();

  while (1) {

    //char* ptr = (char*) clients;
    //puts("Sending to client");
    //for(int i=0;i<sizeof(ClientState)*MAX_CHAT_CLIENTS;i++){
      //printf("%x%c",ptr[i],i+1==sizeof(ClientState)*MAX_CHAT_CLIENTS?'\n':' ');
    //}

    int id = acceptConnection();
    if (id != NO_CONNECTION) {
      recvMsgFromClient(client_names[id], id, WAIT_FOR_IT);
      printf("%s logged in!\n", client_names[id]);
      clients[id].active=1;
      //broadcast(str_buffer, (int)strlen(str_buffer) + 1);
    }

    unsigned char incoming_byte;
    struct msg_ret_t msg_ret = recvMsg(&incoming_byte);

    if (msg_ret.status == MESSAGE_OK) {
      printf("Recieved 0x%x from %d\n", incoming_byte, msg_ret.client_id);
      clients[msg_ret.client_id].keyboard=process_byte(clients[msg_ret.client_id].keyboard,incoming_byte);

      //sendMsgToClient(clients, sizeof(ClientState)*MAX_CHAT_CLIENTS, msg_ret.client_id);

    } else if (msg_ret.status == DISCONNECT_MSG) {
      clients[msg_ret.client_id].active=0;
      sprintf(str_buffer, "%s disconnected", client_names[msg_ret.client_id]);
      printf("%s disconnected, id = %d is free\n",
             client_names[msg_ret.client_id], msg_ret.client_id);
      //broadcast(str_buffer, (int)strlen(str_buffer) + 1);
    }


    update_players();

    if(al_get_time()-prev > 0.05) {
      prev=al_get_time();
      broadcast(clients, sizeof(ClientState)*MAX_CHAT_CLIENTS);
    }

  }
}
