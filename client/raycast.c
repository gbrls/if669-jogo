#include <math.h>
#include "game.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

void rayCasting(float x, float y, float dirX, float dirY, float planeX, float planeY, GameState* state){
  int xs, mapX, mapY, hit, side, stepX, stepY, lineHeight, drawStart, drawEnd;
  unsigned rgb[3];
  float zbuffer[WIDTH]={0};
  float posX = x/MAP_SCALE, posY = y/MAP_SCALE, rayX, rayY, deltaDistX, deltaDistY, perpWallDist, sideDistX, sideDistY, cameraX;
  for(xs = 0; xs<WIDTH; xs++){
    hit = 0;
    cameraX = ((float) 2*xs/WIDTH) -1;
    rayX = dirX + planeX*cameraX;
    rayY = dirY + planeY*cameraX;
    mapX = (int) posX;
    mapY = (int) posY;

    deltaDistX = fabs(1/rayX);
    deltaDistY = fabs(1/rayY);

    if(rayX<0){
      //mapX = (int)(posX/MAP_SCALE)*MAP_SCALE;
      stepX = -1;
      sideDistX = (posX - mapX) * deltaDistX;
    }
    else
    {
      //mapX = (int)(posX/MAP_SCALE + 1)*MAP_SCALE;
      stepX = 1;
      sideDistX = (mapX + 1.0 - posX) * deltaDistX;
    }
    if (rayY < 0)
    {
      //mapY = (int)(posY /MAP_SCALE)*MAP_SCALE;
      stepY = -1;
      sideDistY = (posY - mapY) * deltaDistY;
    }
    else
    {
      //mapY = (int)(posY /MAP_SCALE + 1)*MAP_SCALE;
      stepY = 1;
      sideDistY = (mapY + 1.0 - posY) * deltaDistY;
    }
    while(hit == 0){
      if(sideDistX < sideDistY)
      {
        sideDistX+= deltaDistX;
        mapX += stepX;
        side = 0;
      }
      else{
        sideDistY += deltaDistY;
        mapY+= stepY;
        side = 1;
      }
      if(GameMap[mapX][mapY] != '.') hit++;
    }

    if(side == 0) perpWallDist = (mapX - posX + (1 - stepX)/2)/rayX;
    else perpWallDist = (mapY - posY + (1 - stepY)/2)/rayY;

    lineHeight = (int) (HEIGHT/perpWallDist)*0.5;

    drawStart = -lineHeight/2 + HEIGHT/2;
    if(drawStart<0) drawStart = 0;
    drawEnd = lineHeight/2 + HEIGHT/2;
    if(drawEnd >= HEIGHT) drawEnd = HEIGHT - 1;

    char alvo = GameMap[(int)(mapX)][(int)(mapY)];
    //printf("(%G,%G)\n",posX,posY);

    switch (alvo) {
      case '#':
        rgb[0] = 255;
        rgb[1] = 255;
        rgb[2] = 255;
        break;
      default:
        rgb[0] = 255;
        rgb[1] = 255;
        rgb[2] = 0;

        if(alvo >= '0' && alvo <= '9'){
          alvo = alvo - '0';
          if(state->geladeiras&(1<<alvo)){
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 255;

            //printf("GELAOIDEOAS\n");
          }
        }

    }
    if(side == 1){
      rgb[0] /= 2;
      rgb[1] /= 2;
      rgb[2] /= 2;
    }
    al_draw_line(xs, drawStart, xs, drawEnd, al_map_rgb(rgb[0], rgb[1], rgb[2]), 1.0);
    zbuffer[xs]=perpWallDist;
  }

  // Desenhando os outros jogadores
    for(int i=0;i<MAX_CHAT_CLIENTS;i++) {
      if(i == state->id || !state->players[i].active) continue;
      
      float spriteX = state->players[i].playerState.x/MAP_SCALE - posX;
      float spriteY = state->players[i].playerState.y/MAP_SCALE - posY;

      //transform sprite with the inverse camera matrix
      // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
      // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
      // [ planeY   dirY ]                                          [ -planeY  planeX ]

      float invDet = 1.0 / (planeX * dirY - dirX * planeY); //required for correct matrix multiplication

      float transformX = invDet * (dirY * spriteX - dirX * spriteY);
      float transformY = invDet * (-planeY * spriteX + planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D

      int spriteScreenX = (int)((WIDTH / 2) * (1 + transformX / transformY));

      //calculate height of the sprite on screen
      int spriteHeight = abs((int)(HEIGHT / (transformY))); //using "transformY" instead of the real distance prevents fisheye
      //calculate lowest and highest pixel to fill in current stripe
      int drawStartY = -spriteHeight / 2 + HEIGHT / 2;
      if(drawStartY < 0) drawStartY = 0;
      int drawEndY = spriteHeight / 2 + HEIGHT / 2;
      if(drawEndY >= HEIGHT) drawEndY = HEIGHT - 1;

      //calculate width of the sprite
      int spriteWidth = abs( (int)(HEIGHT / (transformY)));
      int drawStartX = -spriteWidth / 2 + spriteScreenX;
      if(drawStartX < 0) drawStartX = 0;
      int drawEndX = spriteWidth / 2 + spriteScreenX;
      if(drawEndX >= WIDTH) drawEndX = WIDTH - 1;

      //float ang = atan2f(spriteX-posX, spriteY-posY);
      //printf("Angle %0.2f\n", ang);
      int is_front = ((cosf(state->players[i].playerState.angle) * cosf(state->players[state->id].playerState.angle)) < 0?1:0);

      //printf("%s\n", ang < 0.0f? "FRONT":"BACK");

      //if(transformY > 0) {
      //  int c = 50;
      //  if(is_front) {
      //    c = 255;
      //  }
      //  al_draw_filled_rectangle(drawStartX, drawStartY,
      //   drawStartX+spriteWidth, drawStartY+spriteHeight, al_map_rgb(c,0,0));
      //}

      for(int stripe = drawStartX; stripe < drawEndX; stripe++){
        if(transformY > 0 && stripe > 0 && stripe < WIDTH && transformY < zbuffer[stripe]){
          al_draw_line(stripe, drawStartY, stripe, drawEndY, al_map_rgb(255,0,0), 2);
        }
      }
    }

}
