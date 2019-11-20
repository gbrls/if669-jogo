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
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define MSG_MAX_SIZE 350
#define BUFFER_SIZE (MSG_MAX_SIZE + 100)
#define LOGIN_MAX_SIZE 13
#define HIST_MAX_SIZE 200
#define RAIZ_3 1.7320508075688772
char game[120];

ALLEGRO_TIMEOUT timeout;

ALLEGRO_EVENT evento;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_MONITOR_INFO info;

ALLEGRO_BITMAP *background = NULL;

ALLEGRO_FONT *font = NULL;
ALLEGRO_FONT *font_ip = NULL;
ALLEGRO_FONT *font_op = NULL;

ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

ALLEGRO_BITMAP *botao_sair = NULL;
ALLEGRO_BITMAP *botao_jogar = NULL;
ALLEGRO_BITMAP *botao_contexto = NULL;
ALLEGRO_BITMAP *botao_howPlay = NULL;

enum GameRenderState game_render_state = GAME_MAP;
enum estadoDoJogo state = menu;
enum Hover hovermenu = nada;

int res_x_comp, res_y_comp;
char str[12] = "0";

/***************************************************************************************************************/
/***************************************************************************************************************/
/***************************************************************************************************************/

enum conn_ret_t tryConnect()
{
  return connectToServer(str);
}

void assertConnection()
{
  enum conn_ret_t ans = tryConnect();
  while (ans != SERVER_UP)
  {
    if (ans == SERVER_DOWN)
    {
      puts("Server is down!");
    }
    else if (ans == SERVER_FULL)
    {
      puts("Server is full!");
    }
    else if (ans == SERVER_CLOSED)
    {
      puts("Server is closed for new connections!");
    }
    else
    {
      puts("Server didn't respond to connection!");
    }
    printf("Want to try again? [Y/n] ");
    int res;
    while (res = tolower(getchar()), res != 'n' && res != 'y' && res != '\n')
    {
      puts("anh???");
    }
    if (res == 'n')
    {
      exit(EXIT_SUCCESS);
    }
    ans = tryConnect();
  }
  char login[LOGIN_MAX_SIZE + 4];
  ///printf("Please enter your login (limit = %d): ", LOGIN_MAX_SIZE);
  ///scanf(" %[^\n]", login);
  ///getchar();
  for (int i = 0; i < LOGIN_MAX_SIZE; i++)
  {
    login[i] = rand() % 256;
  }
  int len = (int)strlen(login);
  sendMsgToServer(login, len + 1);
}

int inicializar()
{
  printf("Inicializando allegro\n");
  if (!al_init())
  {
    printf("Falha ao abrir biblioteca allegro\n");
    return 0;
  }
  printf("Inicializando allegro primitivas\n");
  if (!al_init_primitives_addon())
  {
    printf("Falha ao abrir biblioteca de primitivas\n");
    return 0;
  }
  printf("Inicializando allegro imagem\n");
  if (!al_init_image_addon())
  {
    printf("Falha ao abrir biblioteca de imagem");
    return 0;
  }

  printf("Inicializando allegro font\n");
  if (!al_init_font_addon())
  {
    printf("Falha ao abrir biblioteca da fonte");
    return 0;
  }

  printf("Inicializando allegro ttf\n");
  if (!al_init_ttf_addon())
  {
    printf("Falha ao abrir biblioteca de ttf");
    return 0;
  }

  printf("Inicializando teclado\n");
  if (!al_install_keyboard())
  {
    printf("Falha ao inicializar o teclado.\n");
    return 0;
  }
  printf("Inicializando mouse\n");
  if (!al_install_mouse())
  {
    printf("Falha ao inicializar o mouse.\n");
    return 0;
  }

  //Deixar em tela inteira
  al_get_monitor_info(0, &info);
  res_x_comp = info.x2 - info.x1;
  res_y_comp = info.y2 - info.y1;
  al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
  printf("Criando janela\n");
  janela = al_create_display(res_x_comp, res_y_comp);
  if (!janela)
  {
    printf("Erro ao inicializar Janela");
    return 0;
  }
  float red_x = res_x_comp / (float)WIDTH;
  float red_y = res_y_comp / (float)HEIGHT;
  ALLEGRO_TRANSFORM transformar;
  al_identity_transform(&transformar);
  al_scale_transform(&transformar, red_x, red_y);
  al_use_transform(&transformar);

  // Atribui o cursor padrão do sistema para ser usado
  printf("Atribuindo cursor\n");
  if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
  {
    fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
    al_destroy_display(janela);
    al_destroy_font(font_op);
    al_destroy_font(font);
    al_destroy_font(font_ip);
    al_destroy_bitmap(background);
    return -1;
  }

  fila_eventos = al_create_event_queue();
  if (!fila_eventos)
  {
    printf("Falha ao criar fila de eventos.\n");
    al_destroy_display(janela);
    return 0;
  }

  al_register_event_source(fila_eventos, al_get_mouse_event_source());
  al_register_event_source(fila_eventos, al_get_keyboard_event_source());
  al_register_event_source(fila_eventos, al_get_display_event_source(janela));

  printf("carregando imagens\n");
  background = al_load_bitmap("assets/img/menu.png");

  if (!background)
  {
    al_destroy_display(janela);
    al_destroy_font(font);
    al_destroy_font(font_ip);
    al_destroy_font(font_op);
    printf("Erro ao carregar a imagem de background");
    return -1;
  }

  printf("Carregando fontes\n");
  font = al_load_font("assets/fonts/PixelBreack.ttf", 100, 0);
  font_op = al_load_font("assets/fonts/PixelBreack.ttf", 50, 0);
  font_ip = al_load_font("assets/fonts/Symtext.ttf", 50, 0);

  // Alocamos o botão Jogar
  botao_jogar = al_create_bitmap(140, 60);
  if (!botao_jogar)
  {
    printf("Falha ao criar botão de jogar.\n");
    al_destroy_display(janela);
    al_destroy_font(font_op);
    al_destroy_font(font);
    al_destroy_font(font_ip);
    al_destroy_bitmap(background);
    return -1;
  }

  // Alocamos o botão Como Jogar
  botao_howPlay = al_create_bitmap(280, 55);
  if (!botao_howPlay)
  {
    printf("Falha ao criar botão de Como jogar.\n");
    al_destroy_display(janela);
    al_destroy_font(font_op);
    al_destroy_font(font);
    al_destroy_font(font_ip);
    al_destroy_bitmap(background);
    al_destroy_bitmap(botao_jogar);
    return -1;
  }

  // Alocamos o botão Contexto
  botao_contexto = al_create_bitmap(210, 55);
  if (!botao_contexto)
  {
    printf("Falha ao criar botão de contexto.\n");
    al_destroy_display(janela);
    al_destroy_font(font_op);
    al_destroy_font(font);
    al_destroy_font(font_ip);
    al_destroy_bitmap(background);
    al_destroy_bitmap(botao_jogar);
    al_destroy_bitmap(botao_howPlay);
    return -1;
  }

  // Alocamos o botão para fechar a aplicação
  botao_sair = al_create_bitmap(100, 50);
  if (!botao_jogar)
  {
    printf("Falha ao criar botão de saída.\n");
    al_destroy_display(janela);
    al_destroy_font(font_op);
    al_destroy_font(font);
    al_destroy_font(font_ip);
    al_destroy_bitmap(background);
    al_destroy_bitmap(botao_jogar);
    al_destroy_bitmap(botao_howPlay);
    al_destroy_bitmap(botao_contexto);
    return -1;
  }

  al_set_window_title(janela, "Jacquin's Hell");

  return 1;
}

void draw_map(unsigned int geladeiras)
{
  for (int i = 0; i < MAP_HEIGHT; i++)
  {
    for (int j = 0; j < MAP_WIDTH; j++)
    {
      if (GameMap[j + i * MAP_WIDTH] == '#')
      {
        al_draw_filled_rectangle(j * MAP_SCALE, i * MAP_SCALE,
                                 (j + 1) * MAP_SCALE, (i + 1) * MAP_SCALE, al_map_rgb(255, 255, 255));
      }
      else if (GameMap[j + i * MAP_WIDTH] >= '0' && GameMap[j + i * MAP_WIDTH] <= '9')
      {
        int c = 0;

        if (geladeiras & (1 << (GameMap[j + i * MAP_WIDTH] - '0')))
          c = 255;

        al_draw_filled_rectangle(j * MAP_SCALE, i * MAP_SCALE,
                                 (j + 1) * MAP_SCALE, (i + 1) * MAP_SCALE, al_map_rgb(c, 255, 0));
      }
    }
  }
}

int main()
{

  srand(time(NULL));
  //assertConnection();

  if (!inicializar())
  {
    printf("Falha ao inicializar\n");
    return -1;
  }

  printf("inicializado!");
  while (1)
  {
    switch (state)
    {
    case menu:

      //printf("Menu\n");

      /***************************************************************************************************************/

      // colore o fundo
      al_clear_to_color(al_map_rgb(255, 255, 255));

      /***************************************************************************************************************/

      // background
      al_draw_bitmap(background, 0, 0, 0);

      /***************************************************************************************************************/

      // Titulo
      al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH - (WIDTH / 4.0), HEIGHT / 7.0, ALLEGRO_ALIGN_CENTER, "Jacquin's Hell");
      al_draw_text(font, al_map_rgb(200, 0, 0), (WIDTH - (WIDTH / 4.0)) - 3, (HEIGHT / 7.0) - 3, ALLEGRO_ALIGN_CENTER, "Jacquin's Hell");

      /***************************************************************************************************************/

      // Verificamos se há eventos na fila
      while (!al_is_event_queue_empty(fila_eventos))
      {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
          state = sair;
        }

        // Hover
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
          // Hover no botao Jogar
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_jogar) - 233 &&
              evento.mouse.x <= WIDTH - 233 &&
              evento.mouse.y <= HEIGHT - 333 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_jogar) - 333)
          {
            hovermenu = jogarHover;
          }
          // Hover no botao como Jogar
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_howPlay) - 163 &&
              evento.mouse.x <= WIDTH - 163 &&
              evento.mouse.y <= HEIGHT - 253 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_howPlay) - 253)
          {
            hovermenu = howPlayHover;
          }
          // Hover no botao como Contexto
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_contexto) - 203 &&
              evento.mouse.x <= WIDTH - 203 &&
              evento.mouse.y <= HEIGHT - 173 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_contexto) - 173)
          {
            hovermenu = contextoHover;
          }
          // Hover no botao Sair
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_sair) - 253 &&
              evento.mouse.x <= WIDTH - 253 &&
              evento.mouse.y <= HEIGHT - 103 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_sair) - 103)
          {
            hovermenu = sairHover;
          }
        }
        /***************************************************************************************************************/
        // Clicado
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
          // clicou no botão jogar
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_jogar) - 233 &&
              evento.mouse.x <= WIDTH - 233 &&
              evento.mouse.y <= HEIGHT - 333 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_jogar) - 333)
          {
            printf("Jogar\n");
            state = jogar_IP;
          }
          //clicou no botão Como Jogar
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_howPlay) - 163 &&
              evento.mouse.x <= WIDTH - 163 &&
              evento.mouse.y <= HEIGHT - 253 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_howPlay) - 253)
          {
            printf("Como Jogar\n");
            state = HowPlay;
          }
          // clicou no botão contexto
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_contexto) - 203 &&
              evento.mouse.x <= WIDTH - 203 &&
              evento.mouse.y <= HEIGHT - 173 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_contexto) - 173)
          {
            printf("Contexto\n");
            state = contexto;
          }
          //clicou no botão sair
          if (evento.mouse.x >= WIDTH - al_get_bitmap_width(botao_sair) - 253 &&
              evento.mouse.x <= WIDTH - 253 &&
              evento.mouse.y <= HEIGHT - 103 &&
              evento.mouse.y >= HEIGHT - al_get_bitmap_height(botao_sair) - 103)
          {
            state = sair;
          }
        }
      }
      if (hovermenu != jogarHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_jogar) - 233, HEIGHT - al_get_bitmap_height(botao_jogar) - 333, ALLEGRO_ALIGN_LEFT, "Jogar");
        al_draw_text(font_op, al_map_rgb(235, 10, 0), WIDTH - al_get_bitmap_width(botao_jogar) - 230, HEIGHT - al_get_bitmap_height(botao_jogar) - 330, ALLEGRO_ALIGN_LEFT, "Jogar");
      }
      else if (hovermenu == jogarHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_jogar) - 233, HEIGHT - al_get_bitmap_height(botao_jogar) - 333, ALLEGRO_ALIGN_LEFT, "Jogar");
        al_draw_text(font_op, al_map_rgb(150, 0, 0), WIDTH - al_get_bitmap_width(botao_jogar) - 230, HEIGHT - al_get_bitmap_height(botao_jogar) - 330, ALLEGRO_ALIGN_LEFT, "Jogar");
      }

      if (hovermenu != howPlayHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_howPlay) - 163, HEIGHT - al_get_bitmap_height(botao_howPlay) - 253, ALLEGRO_ALIGN_LEFT, "Como Jogar");
        al_draw_text(font_op, al_map_rgb(235, 10, 0), WIDTH - al_get_bitmap_width(botao_howPlay) - 160, HEIGHT - al_get_bitmap_height(botao_howPlay) - 250, ALLEGRO_ALIGN_LEFT, "Como Jogar");
      }
      else if (hovermenu == howPlayHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_howPlay) - 163, HEIGHT - al_get_bitmap_height(botao_howPlay) - 253, ALLEGRO_ALIGN_LEFT, "Como Jogar");
        al_draw_text(font_op, al_map_rgb(150, 0, 0), WIDTH - al_get_bitmap_width(botao_howPlay) - 160, HEIGHT - al_get_bitmap_height(botao_howPlay) - 250, ALLEGRO_ALIGN_LEFT, "Como Jogar");
      }
      if (hovermenu != contextoHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_contexto) - 203, HEIGHT - al_get_bitmap_height(botao_contexto) - 173, ALLEGRO_ALIGN_LEFT, "Contexto");
        al_draw_text(font_op, al_map_rgb(235, 10, 0), WIDTH - al_get_bitmap_width(botao_contexto) - 200, HEIGHT - al_get_bitmap_height(botao_contexto) - 170, ALLEGRO_ALIGN_LEFT, "Contexto");
      }
      else if (hovermenu == contextoHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_contexto) - 203, HEIGHT - al_get_bitmap_height(botao_contexto) - 173, ALLEGRO_ALIGN_LEFT, "Contexto");
        al_draw_text(font_op, al_map_rgb(150, 0, 0), WIDTH - al_get_bitmap_width(botao_contexto) - 200, HEIGHT - al_get_bitmap_height(botao_contexto) - 170, ALLEGRO_ALIGN_LEFT, "Contexto");
      }
      if (hovermenu != sairHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_sair) - 253, HEIGHT - al_get_bitmap_height(botao_sair) - 103, ALLEGRO_ALIGN_LEFT, "Sair");
        al_draw_text(font_op, al_map_rgb(235, 10, 0), WIDTH - al_get_bitmap_width(botao_sair) - 250, HEIGHT - al_get_bitmap_height(botao_sair) - 100, ALLEGRO_ALIGN_LEFT, "Sair");
      }
      else if (hovermenu == sairHover)
      {
        al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - al_get_bitmap_width(botao_sair) - 253, HEIGHT - al_get_bitmap_height(botao_sair) - 103, ALLEGRO_ALIGN_LEFT, "Sair");
        al_draw_text(font_op, al_map_rgb(150, 0, 0), WIDTH - al_get_bitmap_width(botao_sair) - 250, HEIGHT - al_get_bitmap_height(botao_sair) - 100, ALLEGRO_ALIGN_LEFT, "Sair");
      }
      break;
    case jogar_IP:

      /***************************************************************************************************************/

      // colore o fundo
      al_clear_to_color(al_map_rgb(255, 255, 255));

      /***************************************************************************************************************/

      // background
      al_draw_bitmap(background, 0, 0, 0);

      /***************************************************************************************************************/

      // Titulo
      al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH - (WIDTH / 4.0), HEIGHT / 7.0, ALLEGRO_ALIGN_CENTER, "Jacquin's Hell");
      al_draw_text(font, al_map_rgb(200, 0, 0), (WIDTH - (WIDTH / 4.0)) - 3, (HEIGHT / 7.0) - 3, ALLEGRO_ALIGN_CENTER, "Jacquin's Hell");

      /***************************************************************************************************************/

      // Verificamos se há eventos na fila
      while (!al_is_event_queue_empty(fila_eventos))
      {

        al_wait_for_event(fila_eventos, &evento);

        // Tecla pressionada
        if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
        {
          if ((evento.keyboard.unichar >= '0' && evento.keyboard.unichar <= '9' || evento.keyboard.unichar == '.'))
          {
            char key[] = {evento.keyboard.unichar};
            strcat(str, key);
          }
          if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
          {
            str[strlen(str) - 1] = '\0';
          }
          if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
          {
            state = jogar;
          }
        }
      }
      /***************************************************************************************************************/

      al_draw_text(font_op, al_map_rgb(255, 255, 255), WIDTH - 503, HEIGHT - 333, ALLEGRO_ALIGN_LEFT, "Coloque seu IP:");
      al_draw_text(font_op, al_map_rgb(235, 10, 0), WIDTH - 500, HEIGHT - 330, ALLEGRO_ALIGN_LEFT, "Coloque seu IP:");

      al_draw_text(font_ip, al_map_rgb(255, 255, 255), WIDTH - (strlen(str) * 15) - 333, HEIGHT - 233, ALLEGRO_ALIGN_LEFT, str);
      al_draw_text(font_ip, al_map_rgb(235, 10, 0), WIDTH - (strlen(str) * 15) - 330, HEIGHT - 230, ALLEGRO_ALIGN_LEFT, str);
      break;
    case jogar:

      al_init_timeout(&timeout, 0.020);

      if (al_wait_for_event_until(fila_eventos, &evento, &timeout))
      {

        unsigned char byte = 0;
        int ktype = 0, key = 0;

        if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
          state = sair;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_UP)
        {
          ktype = KEYUP_TYPE;
          key = evento.keyboard.keycode;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
          ktype = KEYDOWN_TYPE;
          key = evento.keyboard.keycode;

          if (key == ALLEGRO_KEY_ESCAPE)
          {
            if (game_render_state == GAME_MAP)
            {
              game_render_state = GAME_RAYCAST;
            }
            else
            {
              game_render_state = GAME_MAP;
            }
          }
        }

        byte = encodeKey(ktype, key);

        // Check confirmation byte
        if (byte & CONFIRMATION_BIT)
        {
          //TODO: check server's response
          sendMsgToServer((void *)&byte, 1);
        }
      }

      GameState state;
      //ClientState players[MAX_CHAT_CLIENTS];
      int ret = recvMsgFromServer(&state, DONT_WAIT);

      al_clear_to_color(al_map_rgb(0, 0, 0));

      if (game_render_state == GAME_MAP)
      {
        draw_map(state.geladeiras);

        for (int i = 0; i < MAX_CHAT_CLIENTS; i++)
        {
          if (state.players[i].active)
          {

            float px = state.players[i].playerState.x;
            float py = state.players[i].playerState.y;
            float angle = state.players[i].playerState.angle;

            al_draw_circle(px, py,
                           PLAYER_RADIUS, al_map_rgb(0, 0, 255), 10.0f);

            al_draw_line(px, py,
                         px + cosf(angle) * PLAYER_VIEW_DIST,
                         py + sinf(angle) * PLAYER_VIEW_DIST,
                         al_map_rgb(255, 0, 0), 5);

            al_draw_rectangle(5, 5, state.conta, 10,
                              al_map_rgb(100, 200, 100), 5);
          }
        }
      }
      else if (game_render_state == GAME_RAYCAST)
      {
        float px = state.players[0].playerState.x;
        float py = state.players[0].playerState.y;
        float angle = state.players[0].playerState.angle;
        float k = 0.5;
        float dirX = 1.0 * cosf(angle), planeY = (RAIZ_3 * dirX) * k;
        float dirY = 1.0 * sinf(angle), planeX = -(RAIZ_3 * dirY) * k;
        al_clear_to_color(al_map_rgb(0, 0, 0));
        rayCasting(px, py, dirX, dirY, planeX, planeY);
      }

      break;
    case HowPlay:

      break;
    case contexto:

      break;
    case sair:
      // Desaloca os recursos utilizados na aplicação
      al_destroy_display(janela);
      al_destroy_event_queue(fila_eventos);
      al_destroy_font(font_op);
      al_destroy_font(font);
      al_destroy_font(font_ip);
      al_destroy_bitmap(background);
      al_destroy_bitmap(botao_jogar);
      al_destroy_bitmap(botao_howPlay);
      al_destroy_bitmap(botao_contexto);
      al_destroy_bitmap(botao_sair);
      return 0;
      break;
    }
    // Atualiza a tela
    al_flip_display();
  }
  return 0;
}