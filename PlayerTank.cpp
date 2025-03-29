#include"PlayerTank.h"
#include<algorithm>
#include<SDL.h>

PlayerTank::PlayerTank(int startX,int startY)
{
    x=startX;
    y=startY;
    rect={x,y,TILE_SIZE,TILE_SIZE};
    dirX=0;
    dirY=-1;//Default direction up
    active=true;
    shootDelay=playerShootDelay;
}

void PlayerTank::move(int dx,int dy,const std::vector<Wall>&walls)
{
    int newX=x+dx*playerSpeed;
    int newY=y+dy*playerSpeed;
    this->dirX=dx;
    this->dirY=dy;

    SDL_Rect newRect={newX,newY,TILE_SIZE,TILE_SIZE};
    for (int i=0;i<int(walls.size());i++)
    {
        if(walls[i].active&&SDL_HasIntersection(&newRect,&walls[i].rect))
        {
            return;//Prevent movement if colliding with a wall
        }
    }

    if(newX>=TILE_SIZE&&newX<=SCREEN_WIDTH-TILE_SIZE*2&&newY>=TILE_SIZE&&newY<=SCREEN_HEIGHT-TILE_SIZE*2)
    {
        x=newX;
        y=newY;
        rect.x=x;
        rect.y=y;
    }
}

void PlayerTank::shoot()
{
    if (shootDelay<=0)
    {
        bullets.push_back(Bullet(x+TILE_SIZE/2-BULLET_SIZE/2,y+TILE_SIZE/2-BULLET_SIZE/2,this->dirX,this->dirY));
        shootDelay=playerShootDelay;
    }
}

void PlayerTank::updateBullets()
{
    if(shootDelay>0)
    {
        shootDelay--;
    }

    for(auto&bullet:bullets)
    {
        bullet.move();
    }

    bullets.erase(remove_if(bullets.begin(),bullets.end(),
                            [](Bullet& b)
    {
        return !b.active;
    }),bullets.end());
}

void PlayerTank::render(SDL_Renderer *renderer,int id,SDL_Texture *bulletTexture)
{
    if(id==1)SDL_SetRenderDrawColor(renderer,0,250,250,255);
    else SDL_SetRenderDrawColor(renderer,0,17,250,255);
    SDL_RenderFillRect(renderer,&rect);
    for (auto&bullet:bullets)
    {
        bullet.render(renderer,bulletTexture);
    }
}
