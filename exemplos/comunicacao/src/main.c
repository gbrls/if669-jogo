#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
 

#define JANELA_W 400
#define JANELA_H 400

#define LOGIN_MAX_SIZE 20

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define HIST_MAX_SIZE 200

enum conn_ret_t tryConnect() {
  char server_ip[30];
  printf("Please enter the server IP: ");
  scanf(" %s", server_ip);
  getchar();
  return connectToServer(server_ip);
  
}

void rodar_jogo() {

  char str_buffer[BUFFER_SIZE], type_buffer[MSG_MAX_SIZE] = {0};
  int ret = recvMsgFromServer(str_buffer, DONT_WAIT);
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
 
int main(void)
{
  ALLEGRO_DISPLAY *janela = NULL;
  ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
 
  if (!al_init())
  {
    fprintf(stderr, "Falha ao inicializar a Allegro.\n");
    return -1;
  }
 
  if (!al_init_image_addon())
  {
    fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
    return -1;
  }

  if (!al_init_primitives_addon())
  {
    fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
    return false;
  }
 
  janela = al_create_display(JANELA_W, JANELA_H);
  if (!janela)
  {
    fprintf(stderr, "Falha ao criar janela.\n");
    return -1;
  }
 
 
  fila_eventos = al_create_event_queue();
  if (!fila_eventos)
  {
    fprintf(stderr, "Falha ao criar fila de eventos.\n");
    al_destroy_display(janela);
    return -1;
  }
 
  al_register_event_source(fila_eventos, al_get_display_event_source(janela));
 
 
  al_flip_display();

  assertConnection();
 
  while (1)
  {
    ALLEGRO_EVENT evento;
    ALLEGRO_TIMEOUT timeout;
    al_init_timeout(&timeout, 0.05);
 
    int tem_eventos = al_wait_for_event_until(fila_eventos, &evento, &timeout);
 
    if (tem_eventos && evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
      break;
    }
 
    rodar_jogo();
    al_draw_filled_circle(350.0, 50.0, 43.0, al_map_rgb(0, 0, 255));
    al_flip_display();
  }
 
  al_destroy_display(janela);
  al_destroy_event_queue(fila_eventos);
 
  return 0;
}
