#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<ctime>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<SDL.h>

#include"Constants.h"
#include"Menu.h"
#include"Game.h"
using namespace std;

const vector<vector<int>>Map=
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
    {0,0,1,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0},
    {0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int main(int argc, char *argv[])
{
    srand(time(nullptr));//Random theo thời gian thực, đảm bảo luôn ngẫu nhiên

    //Kiểm tra khởi tạo thư viện IMG
    int imgFlags=IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags)&imgFlags))
    {
        std::cerr<<"SDL_image could not initialize! SDL_image Error: "<<IMG_GetError()<<std::endl;
    }

    while(true)
    {
        //Chạy menu game
        Menu menu;
        if(menu.running)
        {
            menu.run();
        }

        //Chạy game
        if(menu.startGame)
        {
            Game game(menu.playerNum==2);
            if(game.running)
            {
                game.run();
            }

            //Xử lí lựa chọn ở màn hình game over
            if(game.returnToMenu)
            {
                continue;//Trở về menu
            }
            else
            {
                break;//Thoát chương trình
            }
        }
        else
        {
            break;//Thoát khi menu dừng
        }
    }

    //Đóng các thư viện SDL
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    return 0;
}
