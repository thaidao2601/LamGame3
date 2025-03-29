#include"Bullet.h"
#include<SDL.h>

Bullet::Bullet(int startX,int startY,int dirX,int dirY)
{
    x=startX;
    y=startY;
    dx=dirX;
    dy=dirY;
    active=true;
    rect={x,y,BULLET_SIZE,BULLET_SIZE};
}

void Bullet::move()
{
    x+=dx*bulletSpeed;
    y+=dy*bulletSpeed;
    rect.x=x;
    rect.y=y;
    if(x<TILE_SIZE||x>SCREEN_WIDTH-TILE_SIZE||y<TILE_SIZE||y>SCREEN_HEIGHT-TILE_SIZE)
    {
        active=false;
    }
}

void Bullet::render(SDL_Renderer *renderer,SDL_Texture *bulletTexture)
{
    if(bulletTexture)
    {
        double angle=0;
        if(dx==0&&dy==-1)angle=0;
        else if(dx==0&&dy==1)angle = 180;
        else if(dx==-1&&dy==0)angle=270;
        else if(dx==1&&dy==0)angle=90;
        SDL_RenderCopyEx(renderer,bulletTexture,NULL,&rect,angle,NULL,SDL_FLIP_NONE);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderFillRect(renderer,&rect);
    }
}
