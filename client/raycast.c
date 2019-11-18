#include <math.h>
#include "game.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

void rayCasting(float x, float y, float dirX, float dirY, float planeX, float planeY, char geladeiras){
  int xs, mapX, mapY, hit, side, stepX, stepY, lineHeight, drawStart, drawEnd;
  unsigned rgb[3];
  float posX = x/MAP_WIDTH, posY = y/MAP_SCALE, rayX, rayY, deltaDistX, deltaDistY, perpWallDist, sideDistX, sideDistY, cameraX;
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
    printf("(%G,%G)\n",posX,posY);

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
          if(geladeiras&(1<<alvo)){
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 255;

            printf("GELAOIDEOAS\n");
          }
        }

    }
    if(side == 1){
      rgb[0] /= 2;
      rgb[1] /= 2;
      rgb[2] /= 2;
    }
    al_draw_line(xs, drawStart, xs, drawEnd, al_map_rgb(rgb[0], rgb[1], rgb[2]), 1.0);
  }
}
