#include<iostream>
#include<vector>
#include<algorithm>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<SDL.h>

using namespace std;

const int SCREEN_WIDTH=800;
const int SCREEN_HEIGHT=600;
const int TILE_SIZE=40;
const int MAP_WIDTH=SCREEN_WIDTH/TILE_SIZE;
const int MAP_HEIGHT=SCREEN_HEIGHT/TILE_SIZE;
const int BULLET_SIZE=10;
const int PlayersNum=1;
const int EnemiesNum=4;
const int EnemymoveDelay=15;
const int EnemyshootDelay=3;

class Wall
{
public:
    int x,y;
    SDL_Rect rect;
    bool active;

    Wall(int startX,int startY)
    {
        x=startX;
        y=startY;
        active=true;
        rect={x,y,TILE_SIZE,TILE_SIZE};
    }
    void render(SDL_Renderer *renderer)
    {
        if(active)
        {
            SDL_SetRenderDrawColor(renderer,150,75,0,225);//Brown color
            SDL_RenderFillRect(renderer,&rect);
        }
    }
};

class Bullet
{
public:
    int x,y;
    int dx,dy;
    SDL_Rect rect;
    bool active;

    Bullet(int startX,int startY,int dirX,int dirY)
    {
        x=startX;
        y=startY;
        dx=dirX;
        dy=dirY;
        active=true;
        rect={x,y,BULLET_SIZE,BULLET_SIZE};//Square shape bullet
    }

    void move()
    {
        x+=dx;
        y+=dy;
        rect.x=x;
        rect.y=y;
        if(x<TILE_SIZE||x>SCREEN_WIDTH-TILE_SIZE||y<TILE_SIZE||y>SCREEN_HEIGHT-TILE_SIZE)
        {
            active=false;
        }
    }

    void render(SDL_Renderer *renderer)
    {
        if (active)
        {
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
            SDL_RenderFillRect(renderer,&rect);
        }
    }

};

class PlayerTank
{
public:
    int x,y;
    int dirX,dirY;
    SDL_Rect rect;
    vector<Bullet>bullets;

    PlayerTank(int startX,int startY)
    {
        x=startX;
        y=startY;
        rect={x,y,TILE_SIZE,TILE_SIZE};
        dirX=0;
        dirY=-1;//Default direction up
    }

    void move(int dx,int dy,const vector<Wall>&walls)
    {
        int newX=x+dx;
        int newY=y+dy;
        this->dirX=dx;
        this->dirY=dy;

        SDL_Rect newRect={newX,newY,TILE_SIZE,TILE_SIZE};
        for(int i=0;i<int(walls.size());i++)
        {
            if (walls[i].active&&SDL_HasIntersection(&newRect,&walls[i].rect))
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

    void shoot()
    {
        bullets.push_back(Bullet(x+TILE_SIZE/2-BULLET_SIZE/2,y+TILE_SIZE/2-BULLET_SIZE/2,this->dirX,this->dirY));
    }

    void updateBullets()
    {
        for(auto&bullet:bullets)
        {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(),bullets.end(),
                                [](Bullet &b){return !b.active;}), bullets.end());
    }

    void render(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer,255,255,0,255);
        SDL_RenderFillRect(renderer,&rect);
            for (auto&bullet:bullets)
            {
                bullet.render(renderer);
            }
    }
};

class EnemyTank
{
public:
    int x,y;
    int dirX,dirY;
    int moveDelay,shootDelay;
    SDL_Rect rect;
    bool active;
    vector<Bullet>bullets;

    EnemyTank(int startX,int startY)
    {
        moveDelay=EnemymoveDelay;
        shootDelay=EnemyshootDelay;
        x=startX;
        y=startY;
        rect={x,y,TILE_SIZE,TILE_SIZE};
        dirX=0;
        dirY=1;
        active=true;
    }

    void move(const vector<Wall>&walls)
    {
        if(--moveDelay>0)return;
        moveDelay=EnemymoveDelay;
        int r=rand()%4;
        if(r==0)//Up
        {
            this->dirX=0;
            this->dirY=-5;
        }
        else if(r==1)//Down
        {
            this->dirX=0;
            this->dirY=5;
        }
        else if(r==2)// Left
        {
            this->dirY=0;
            this->dirX=-5;
        }
        else if(r==3)// Right
        {
            this->dirY=0;
            this->dirX=5;
        }

        int newX=x+this->dirX;
        int newY=y+this->dirY;
        SDL_Rect newRect={newX,newY,TILE_SIZE,TILE_SIZE};
        for(const auto&wall:walls)
        {
            if(wall.active&&SDL_HasIntersection(&newRect,&wall.rect))
            {
                return;
            }
        }
        if(newX>=TILE_SIZE&&newX<=SCREEN_WIDTH-TILE_SIZE*2&&
           newY>=TILE_SIZE&&newY<=SCREEN_HEIGHT-TILE_SIZE*2)
        {
            x=newX;
            y=newY;
            rect.x=x;
            rect.y=y;
        }
    }

    void shoot()
    {
        if(--shootDelay>0)return;
        shootDelay=EnemyshootDelay;
        bullets.push_back(Bullet(x+TILE_SIZE/2-BULLET_SIZE/2,y+TILE_SIZE/2-BULLET_SIZE/2,this->dirX,this->dirY));
    }

    void updateBullets()
    {
        for (auto&bullet:bullets)
        {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(),bullets.end(),
                                [](Bullet &b){return !b.active;}),bullets.end());
    }

    void render(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderFillRect(renderer,&rect);
        for(auto&bullet:bullets)
        {
            bullet.render(renderer);
        }
    }

};
class Game
{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    vector<Wall>walls;
    PlayerTank player=spawnPlayer();
    vector<EnemyTank>enemies;

    void generateWalls()
    {
        for (int i=3;i<MAP_HEIGHT-3;i+=2)
        {
            for(int j=3;j<MAP_WIDTH-3;j+=2)
            {
                Wall w=Wall(j*TILE_SIZE,i*TILE_SIZE);
                walls.push_back(w);
            }
        }
    }

    void spawnEnemies()
    {
        enemies.clear();
        for(int i=0;i<EnemiesNum;++i)
        {
            int ex,ey;
            bool validPosition=false;
            while(!validPosition)
            {
                ex=(rand()%(MAP_WIDTH-2)+1)*TILE_SIZE;
                ey=(rand()%(MAP_HEIGHT-2)+1)*TILE_SIZE;
                validPosition=true;
                for(const auto&wall:walls)
                {
                    if(wall.active&&wall.x==ex&&wall.y==ey)
                    {
                        validPosition=false;
                        break;
                    }
                }
            }
            enemies.push_back(EnemyTank(ex,ey));
        }
    }

    PlayerTank spawnPlayer()
    {
        return PlayerTank(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE);
    }

    Game()
    {
        bool running=true;
        if(SDL_Init(SDL_INIT_VIDEO)<0)
        {
            std::cerr<<"SDL could not initialize! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
        }
        window=SDL_CreateWindow("Battle City",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
        if (!window)
        {
            std::cerr<<"Window could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
        }
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            std::cerr<<"Renderer could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
            running=false;
        }

        generateWalls();
        spawnEnemies();

    }

    void update()
    {
        player.updateBullets();

        for(auto&enemy:enemies)
        {
            enemy.move(walls);
            enemy.updateBullets();
            if (rand()%100<2)
            {
                enemy.shoot();
            }
        }

        for(auto&enemy:enemies)
        {
            for(auto&bullet:enemy.bullets)
            {
                for(auto&wall:walls)
                {
                    if(wall.active&&SDL_HasIntersection(&bullet.rect,&wall.rect))
                    {
                        wall.active=false;
                        bullet.active=false;
                        break;
                    }
                }
            }
        }

        for(auto&bullet:player.bullets)
        {
            for(auto&wall:walls)
            {
                if(wall.active&&SDL_HasIntersection(&bullet.rect,&wall.rect))
                {
                    wall.active=false;
                    bullet.active=false;
                    break;
                }
            }
        }

        for(auto&bullet:player.bullets)
        {
            for(auto&enemy:enemies)
            {
                if(enemy.active&&SDL_HasIntersection(&bullet.rect,&enemy.rect))
                {
                    enemy.active=false;
                    bullet.active=false;
                }
            }
        }

        enemies.erase(remove_if(enemies.begin(),enemies.end(),
                                [](EnemyTank &e){return !e.active;}),enemies.end());
        if(enemies.empty())
        {
            running=false;
        }

        for(auto&enemy:enemies)
        {
            for(auto&bullet:enemy.bullets)
            {
                //Update
                if(SDL_HasIntersection(&bullet.rect,&player.rect))
                {
                    running=false;
                    return;
                }
            }
        }
    }

    void handleEvents()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT)
            {
                running=false;
            }
            else if(event.type==SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:player.move(0,-5,walls);break;
                    case SDLK_DOWN:player.move(0,5,walls);break;
                    case SDLK_LEFT:player.move(-5,0,walls);break;
                    case SDLK_RIGHT:player.move(5,0,walls);break;
                    case SDLK_SPACE:player.shoot();break;
                }
            }
        }
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer,128,128,128,255);//Set draw color to gray for boundaries
        SDL_RenderClear(renderer);//Clear the renderer with the set color

        SDL_SetRenderDrawColor(renderer,0,0,0,255);//Set draw color to black for tiles
        for(int i=1;i<MAP_HEIGHT-1;++i)
        {
            for(int j=1;j<MAP_WIDTH-1;++j)
            {
                SDL_Rect tile={j*TILE_SIZE,i*TILE_SIZE,TILE_SIZE,TILE_SIZE}; //Define tile rectangle
                SDL_RenderFillRect(renderer,&tile);//Fill the tile rectangle with the current draw color
            }
        }

        for(int i=0;i<int(walls.size());++i)
        {
            walls[i].render(renderer);
        }

        player.render(renderer);
        for(auto&enemy:enemies)
        {
            enemy.render(renderer);
        }
        SDL_RenderPresent(renderer);//Update the screen with the rendered content
    }

    void run()
    {
        while(running)
        {
            handleEvents();
            update();
            render();
            SDL_Delay(16);
        }
    }

    ~Game()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc,char *argv[])
{
    Game game;
    if(game.running)
    {
        game.run();
    }
    return 0;
}
