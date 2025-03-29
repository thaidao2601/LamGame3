#include"Explosion.h"
#include<SDL.h>

Explosion::Explosion(int posX,int posY):x(posX),y(posY),active(true)
{
    rect= {x,y,TILE_SIZE,TILE_SIZE};
    startTime=SDL_GetTicks();
}

bool Explosion::isExpired()const
{
    return SDL_GetTicks()-startTime>ExplosionTime;
}
