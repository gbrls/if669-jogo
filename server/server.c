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

void hit_players(int id, float x, int y, float angle) {
    float nx=x,ny=y,step=0.3;

    while(((nx-x)*(nx-x) + (ny-y)*(ny-y)) < PLAYER_VIEW_DIST*PLAYER_VIEW_DIST) {
        
        nx += cosf(angle)*step;
        ny += sinf(angle)*step;
        
        for(int target=0;target<MAX_CHAT_CLIENTS;target++){    
            if(isValidId(target) &&  target != id && target != state.jaquin) {
                float px = state.players[target].playerState.x;
                float py = state.players[target].playerState.y;
                if((nx-px)*(nx-px) + (ny-py)*(ny-py) < 100.0) {
                    if(id==state.jaquin) state.players[target].playerState.froze=1;
                    else state.players[target].playerState.froze=0;

                }
            }
        }

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

            if(!state.players[id].playerState.froze) {
                puts("Freezing");
                hit_players(id, state.players[id].playerState.x,
                state.players[id].playerState.y,
                state.players[id].playerState.angle);
            }
       }
       //if(new&KEY_BYTE_ACTION) state.geladeiras^=0xff;
    }

    if(new&KEYUP_TYPE){
        prev=prev & (~nib);
    }

    return prev;
}



int check_collision(float x, float y) {
    if(x<0||y<0||check_map_collision(x,y)) return 1;
    return 0;
}


void update_players() {
    for(int i=0;i<MAX_CHAT_CLIENTS;i++){
        if(state.players[i].active){

          float spd=0.2,rotspd=0.002;
          float prevx=state.players[i].playerState.x,prevy=state.players[i].playerState.y;


            if(i==state.jaquin) spd/=5.0f;

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

          //if(state.players[i].playerState.froze) {
          //  printf("%d is frozen\n",i);
          //  return;
          //}

          if(check_collision(state.players[i].playerState.x,state.players[i].playerState.y)
          ||state.players[i].playerState.froze) {
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

        int mx = 0;
        int my = 0;


        do {
            mx = rand()%MAP_WIDTH;
            my = rand()%MAP_HEIGHT;
        } while(GameMap[mx][my]!='.');


        state.players[id].playerState.x=mx*MAP_SCALE;
        state.players[id].playerState.y=mx*MAP_SCALE;

        

        state.players[id].playerState.angle=0.0f;
        state.players[id].playerState.froze=0;
        
    }
}

void update_game_state(double delta_time) {
    //printf("%g\n",state.conta);

    double K=5.0;
    if(state.started) {
        state.elapsed += delta_time;
        for(int i=0;i<NUM_GELADEIRAS;i++){
            if(state.geladeiras & (1<<i)){
                state.conta += delta_time*K;
            }
        }
    }

    if(state.elapsed/60.0 > MAX_ELAPSED) {
        state.ended = 1;
    }

    if(state.conta > MAX_CONTA) {
        state.ended = 2;
    }

    if(!state.started && state.players[(int)state.jaquin].keyboard&KEY_BYTE_ACTION){
        state.started=1;
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
  state.started=0;
  state.n_players=0;
  

  while (1) {

    
    if(!state.started) {
    int id = acceptConnection();
        if (id != NO_CONNECTION) {
          recvMsgFromClient(client_names[id], id, WAIT_FOR_IT);
          printf("%s logged in!\n", client_names[id]);
          state.n_players++;
          init_client(id);
        }
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
             state.n_players--;
    }

    if(state.started) {
        update_players();
    }
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
