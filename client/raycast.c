#include <math.h>
#include "game.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
rayCasting(float x, float y, float dirX, float dirY, float planeX, float planeY){
  int xs, mapX, mapY, hit, side, stepX, stepY, lineHeight, drawStart, drawEnd;
  unsigned rgb[3];
  float posX = x/MAP_SCALE, posY = y/MAP_SCALE, rayX, rayY, deltaDistX, deltaDistY, perpWallDist, sideDistX, sideDistY, cameraX;
  for(xs = 0; xs<MAP_WIDTH; xs++){
    hit = 0;
    cameraX = ((float) 2*xs/MAP_WIDTH) -1;
    rayX = posX + dirX + planeX*cameraX;
    rayY = posY + dirY + planeY*cameraX;
    mapX = (int) posX;
    mapY = (int) posY;

    deltaDistX = fabs(1/rayX);
    deltaDistY = fabs(1/rayY);

    if(rayX<0){
      stepX = -1;
      sideDistX = (posX - mapX) * deltaDistX;
    }
    else
    {
      stepX = 1;
      sideDistX = (mapX + 1.0 - posX) * deltaDistX;
    }
    if (rayY < 0)
    {
      stepY = -1;
      sideDistY = (posY - mapY) * deltaDistY;
    }
    else
    {
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
      if(GameMap[mapX + mapY * MAP_WIDTH] != '.') hit++;
    }

    if(side == 0) perpWallDist = (mapX - posX + (1 - stepX)/2)/rayX;
    else perpWallDist = (mapY - posY + (1 - stepY)/2)/rayY;

    lineHeight = (int) (MAP_HEIGHT/perpWallDist);
    drawStart = -lineHeight/2 + MAP_HEIGHT/2;
    if(drawStart<0) drawStart = 0;
    drawEnd = lineHeight/2 + MAP_HEIGHT;
    if(drawEnd >= MAP_HEIGHT) drawEnd = MAP_HEIGHT - 1;

    switch (GameMap[mapX + MAP_WIDTH*mapY]) {
      case '#':
        rgb[0] = 255;
        rgb[1] = 255;
        rgb[2] = 255;
        break;
      default:
        rgb[0] = 255;
        rgb[1] = 255;
        rgb[2] = 0;
    }
    if(side == 1){
      rgb[0] /= 2;
      rgb[1] /= 2;
      rgb[2] /= 2;
    }
    al_draw_line(xs, drawStart, xs, drawEnd, al_map_rgb(rgb[0], rgb[1], rgb[2]), 2.0);
  }
}
