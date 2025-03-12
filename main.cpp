#include<iostream>
#include<vector>
#include<algorithm>
#include<cstdlib>
#include<SDL.h>

using namespace std;

const int SCREEN_WIDTH=800;
const int SCREEN_HEIGHT=600;
const int TILE_SIZE=40;
const int MAP_WIDTH=SCREEN_WIDTH/TILE_SIZE;
const int MAP_HEIGHT=SCREEN_HEIGHT/TILE_SIZE;
const int BULLET_SIZE=10;
const int BULLET_SPEED=5;

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
        rect= {x,y,TILE_SIZE,TILE_SIZE};
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
    int dirX,dirY;
    bool active;
    SDL_Rect rect;

    Bullet(int startX,int startY,int dX,int dY)
    {
        x=startX;
        y=startY;
        dirX=dX;
        dirY=dY;
        active=true;
        rect={x,y,BULLET_SIZE,BULLET_SIZE};
    }

    void update()
    {
        x+=dirX*BULLET_SPEED;
        y+=dirY*BULLET_SPEED;
        rect.x=x;
        rect.y=y;

        if(x<0||x>SCREEN_WIDTH||y<0||y>SCREEN_HEIGHT)
        {
            active=false;
        }
    }

    void render(SDL_Renderer *renderer)
    {
        if(active)
        {
            SDL_SetRenderDrawColor(renderer,255,0,0,255);
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
        rect= {x,y,TILE_SIZE,TILE_SIZE};
        dirX=0;
        dirY=-1;//Default direction up
    }

    void move(int dx,int dy,const vector<Wall>&walls)
    {
        int newX=x+dx;
        int newY=y+dy;
        this->dirX=dx;
        this->dirY=dy;

        SDL_Rect newRect= {newX,newY,TILE_SIZE,TILE_SIZE};
        for(int i=0; i<int(walls.size()); i++)
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
        int bulletX=x+TILE_SIZE/2-BULLET_SIZE/2;
        int bulletY=y+TILE_SIZE/2-BULLET_SIZE/2;
        bullets.push_back(Bullet(bulletX,bulletY,dirX,dirY));
    }

    void updateBullets(vector<Wall>&walls)
    {
        for (auto&bullet:bullets)
        {
            if(bullet.active)
            {
                bullet.update();
                for(auto&wall:walls)
                {
                    if(wall.active&&SDL_HasIntersection(&bullet.rect,&wall.rect))
                    {
                        bullet.active=false;
                        wall.active=false;//Đạn bắn vào tường sẽ phá hủy tường
                    }
                }
            }
        }
        bullets.erase(remove_if(bullets.begin(),bullets.end(),[](Bullet &b)
        {
            return !b.active;
        }), bullets.end());
    }

    void render(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer,255,255,0,255);
        SDL_RenderFillRect(renderer,&rect);
        for (auto &bullet : bullets)
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
    SDL_Rect rect;
    vector<Bullet>bullets;
    int shootTimer;
    int shootDelay;
    bool active;

    EnemyTank(int startX,int startY)
    {
        x=startX;
        y=startY;
        rect={x,y,TILE_SIZE,TILE_SIZE};

        // Chọn hướng di chuyển ban đầu ngẫu nhiên
        int direction=rand()%4;
        if (direction==0)
        {
            dirX=0;    // Lên
            dirY=-1;
        }
        else if(direction==1)
        {
            dirX=1;    // Phải
            dirY=0;
        }
        else if(direction==2)
        {
            dirX=0;    // Xuống
            dirY=1;
        }
        else
        {
            dirX=-1;    // Trái
            dirY=0;
        }

        shootTimer=0;
        shootDelay=120; // 120 frames ~ 2 giây (với 60 FPS)
        active=true;
    }

    void update(const vector<Wall>& walls, PlayerTank& player, vector<EnemyTank>& enemies)
    {
        if (!active) return;

        // Di chuyển
        int speed=2; // Tốc độ di chuyển của tank địch
        int newX=x+dirX*speed;
        int newY=y+dirY*speed;

        SDL_Rect newRect={newX,newY,TILE_SIZE,TILE_SIZE};
        bool collision=false;

        // Kiểm tra va chạm với tường
        for(const auto&wall:walls)
        {
            if (wall.active&&SDL_HasIntersection(&newRect, &wall.rect))
            {
                collision=true;
                break;
            }
        }

        // Kiểm tra va chạm với người chơi
        if(SDL_HasIntersection(&newRect,&player.rect))
        {
            collision=true;
        }

        // Kiểm tra va chạm với các xe tăng địch khác
        for(const auto& enemy:enemies)
        {
            if (&enemy!=this&&enemy.active&&SDL_HasIntersection(&newRect,&enemy.rect))
            {
                collision=true;
                break;
            }
        }

        // Kiểm tra va chạm với biên màn hình
        if(newX<TILE_SIZE||newX>SCREEN_WIDTH-TILE_SIZE*2||newY<TILE_SIZE||newY>SCREEN_HEIGHT-TILE_SIZE*2)
        {
            collision=true;
        }

        if(collision)
        {
            // Đổi hướng khi gặp vật cản
            int direction=rand()%4;
            if(direction==0)
            {
                dirX=0;    // Lên
                dirY=-1;
            }
            else if(direction==1)
            {
                dirX=1;    // Phải
                dirY=0;
            }
            else if(direction==2)
            {
                dirX=0;    // Xuống
                dirY=1;
            }
            else
            {
                dirX=-1;    // Trái
                dirY=0;
            }
        }
        else
        {
            // Di chuyển nếu không có va chạm
            x=newX;
            y=newY;
            rect.x=x;
            rect.y=y;
        }

        // Bắn đạn mỗi 2 giây
        shootTimer++;
        if (shootTimer>=shootDelay)
        {
            shoot();
            shootTimer=0;
        }

        // Cập nhật đạn
        updateBullets(walls,player);
    }

    void shoot()
    {
        int bulletX=x+TILE_SIZE/2-BULLET_SIZE/2;
        int bulletY=y+TILE_SIZE/2-BULLET_SIZE/2;
        bullets.push_back(Bullet(bulletX, bulletY, dirX, dirY));
    }

    void updateBullets(const vector<Wall>& walls,PlayerTank& player)
    {
        vector<Wall>wallscopy;
        for (auto& bullet:bullets)
        {
            if (bullet.active)
            {
                bullet.update();

                // Kiểm tra va chạm với tường
                for (auto& wall:wallscopy)
                {
                    if (wall.active&&SDL_HasIntersection(&bullet.rect, &wall.rect))
                    {
                        bullet.active=false;
                        wall.active=false;
                        break;
                    }
                }

                // Kiểm tra va chạm với người chơi
                if (bullet.active&&SDL_HasIntersection(&bullet.rect,&player.rect))
                {
                    bullet.active=false;
                    // Có thể thêm logic khi người chơi bị trúng đạn
                    // ví dụ: player.lives--;
                }
            }
        }

        // Xóa đạn không còn hoạt động
        bullets.erase(remove_if(bullets.begin(),bullets.end(),[](Bullet& b)
        {
            return !b.active;
        }), bullets.end());
    }

    void render(SDL_Renderer* renderer)
    {
        if(!active)return;

        // Vẽ xe tăng địch màu xanh dương
        SDL_SetRenderDrawColor(renderer,0,0,255,255);
        SDL_RenderFillRect(renderer,&rect);

        // Vẽ đạn
        for(auto& bullet:bullets)
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
    PlayerTank player;
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
        // Tạo 3 xe tăng địch ở vị trí khác nhau
        enemies.push_back(EnemyTank(2*TILE_SIZE,2*TILE_SIZE));
        enemies.push_back(EnemyTank((MAP_WIDTH-3)*TILE_SIZE,2*TILE_SIZE));
        enemies.push_back(EnemyTank((MAP_WIDTH/2)*TILE_SIZE,2*TILE_SIZE));
    }

    Game():player(((MAP_WIDTH-1)/2)*TILE_SIZE,(MAP_HEIGHT-2)*TILE_SIZE)
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
                case SDLK_UP:
                    player.move(0,-5,walls);
                    break;
                case SDLK_DOWN:
                    player.move(0,5,walls);
                    break;
                case SDLK_LEFT:
                    player.move(-5,0,walls);
                    break;
                case SDLK_RIGHT:
                    player.move(5,0,walls);
                    break;
                case SDLK_SPACE:
                    player.shoot();
                    break;
                }
            }
        }
    }

    void update()
    {
        player.updateBullets(walls);

        // Cập nhật các xe tăng địch
        for (auto& enemy:enemies)
        {
            enemy.update(walls,player,enemies);
        }

        // Kiểm tra va chạm đạn người chơi với xe tăng địch
        for (auto&bullet:player.bullets)
        {
            if (bullet.active)
            {
                for (auto&enemy:enemies)
                {
                    if(enemy.active&&SDL_HasIntersection(&bullet.rect,&enemy.rect))
                    {
                        bullet.active=false;
                        enemy.active=false;
                        break;
                    }
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
                SDL_Rect tile={j*TILE_SIZE,i*TILE_SIZE,TILE_SIZE,TILE_SIZE};//Define tile rectangle
                SDL_RenderFillRect(renderer,&tile);//Fill the tile rectangle with the current draw color
            }
        }

        for(int i=0;i<int(walls.size());++i)
        {
            walls[i].render(renderer);
        }

        player.render(renderer);

        // Vẽ các xe tăng địch
        for(auto& enemy:enemies)
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
            player.updateBullets(walls);
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
