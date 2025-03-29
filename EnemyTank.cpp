#include"EnemyTank.h"
#include<cstdlib>
#include<algorithm>
#include<SDL.h>

EnemyTank::EnemyTank(int startX,int startY)
{
    moveDelay=EnemymoveDelay;
    shootDelay=EnemyshootDelay;
    x=startX;
    y=startY;
    rect= {x,y,TILE_SIZE,TILE_SIZE};
    dirX=0;
    dirY=1;
    active=true;
    directionTimer=0;
}

void EnemyTank::changeDirection()
{
    int r=rand()%4;
    if(r==0)
    {
        this->dirX=0;
        this->dirY=-1;
    }
    else if(r==1)
    {
        this->dirX=0;
        this->dirY=1;
    }
    else if(r==2)
    {
        this->dirY=0;
        this->dirX=-1;
    }
    else if(r==3)
    {
        this->dirY=0;
        this->dirX=1;
    }
    directionTimer=0;
}

void EnemyTank::move(const std::vector<Wall>&walls)
{
    if(--moveDelay>0)return;
    moveDelay=EnemymoveDelay;
    directionTimer++;

    if(directionTimer>=180/EnemymoveDelay)
    {
        changeDirection();
    }

    int newX=x+this->dirX*enemySpeed;
    int newY=y+this->dirY*enemySpeed;
    SDL_Rect newRect= {newX,newY,TILE_SIZE,TILE_SIZE};

    bool wallCollision=false;
    for (const auto&wall:walls)
    {
        if(wall.active&&SDL_HasIntersection(&newRect,&wall.rect))
        {
            wallCollision=true;
            break;
        }
    }

    bool boundaryCollision=newX<TILE_SIZE||newX>SCREEN_WIDTH-TILE_SIZE*2||
                           newY<TILE_SIZE||newY>SCREEN_HEIGHT-TILE_SIZE*2;

    if(wallCollision||boundaryCollision)
    {
        changeDirection();
        return;
    }

    x=newX;
    y=newY;
    rect.x=x;
    rect.y=y;
}

void EnemyTank::shoot()
{
    if (--shootDelay>0)return;
    shootDelay=EnemyshootDelay;
    bullets.push_back(Bullet(x+TILE_SIZE/2-BULLET_SIZE/2,y+TILE_SIZE/2-BULLET_SIZE/2,this->dirX,this->dirY));
}

void EnemyTank::updateBullets()
{
    for(auto&bullet:bullets)
    {
        bullet.move();
    }
    bullets.erase(remove_if(bullets.begin(),bullets.end(),
                            [](Bullet&b)
    {
        return !b.active;
    }), bullets.end());
}

void EnemyTank::render(SDL_Renderer *renderer,SDL_Texture *bulletTexture)
{
    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    SDL_RenderFillRect(renderer,&rect);
    for (auto&bullet:bullets)
    {
        bullet.render(renderer,bulletTexture);
    }
}
