#ifndef EXPLOSION_H
#define EXPLOSION_H

#include<SDL.h>

#include"Constants.h"

class Explosion
{
public:
    int x,y;
    SDL_Rect rect;
    Uint32 startTime;
    bool active;

    Explosion(int posX,int posY);
    bool isExpired()const;
};

#endif
