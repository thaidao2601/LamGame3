#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include<SDL.h>
#include<vector>

#include"Constants.h"
#include"Bullet.h"
#include"Wall.h"

class PlayerTank
{
public:
    int x,y;
    int dirX,dirY;
    SDL_Rect rect;
    std::vector<Bullet>bullets;
    bool active;
    int shootDelay;

    PlayerTank(int startX,int startY);
    void move(int dx,int dy,const std::vector<Wall>&walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer *renderer,int id,SDL_Texture *bulletTexture);
};

#endif
