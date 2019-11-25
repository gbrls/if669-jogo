#ifndef RAYCAST_H
#define RAYCAST_H
#include <math.h>
#include <allegro5/allegro.h>
#include "game.h"
void rayCasting(float x, float y, float dirX, float dirY, float planeX, float planeY, GameState* state, ALLEGRO_BITMAP* chef,ALLEGRO_BITMAP* jaq);
#endif
