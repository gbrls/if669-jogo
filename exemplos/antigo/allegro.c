#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.H>
#include <allegro5/allegro_primitives.h>
#include <string.h>

#define mapWidth 900
#define mapHeight 900;

char game[120];

strcpy(game, "")
ALLEGRO_DISPLAY * window = NULL;

bool inicializar(){
  if(!al_init){
    fprintf(stderr, "Falha ao abrir biblioteca allegro\n");
    return false;
  }
  if(!al_init_primitives){
    fprintf(stderr, "Falha ao abrir biblioteca de primitivas\n");
    return false;
  }
  window = al_create_display(width, mapHeight);
  if(!window){
    printf("Falha ao criar janela\n");
    return false;
  }
  return true;
}
void debug(){
  int i, j;

  for(i=0; i<mapWidth; i++){
    for(j=0; j<mapHeight; j++){
      al_draw_filled_rectangle(i*5, j*5, 5, 5, al_map_rgb(10, 100, 10));

    }
  }
  al_draw_filled_rectangle()

}

int main(){
  if(!inicializar){
    return -1;
  }

  return 0;
}
