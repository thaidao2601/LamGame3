#include"Menu.h"
#include<iostream>

Menu::Menu()
{
    running=true;
    startGame=false;
    playerNum=0;

    //Kiểm tra khởi tạo SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        std::cerr<<"SDL could not initialize! SDL_Error: "<<SDL_GetError()<<std::endl;
        running=false;
        return;
    }

    if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0)
    {
        std::cerr<<"SDL_mixer could not initialize! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
        running=false;
        return;
    }

    //Kiểm tra khởi tạo TTF
    if(TTF_Init()<0)
    {
        std::cerr<<"TTF could not initialize! TTF_Error: "<<TTF_GetError()<<std::endl;
        running=false;
        return;
    }

    //Tạo window và kiểm tra quá trình tạo window
    window=SDL_CreateWindow("Battle City",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if(!window)
    {
        std::cerr<<"Window could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
        running=false;
        return;
    }

    //Tạo render và kiểm tra qua trình tạo render
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        std::cerr<<"Renderer could not be created! SDL_Error: "<<SDL_GetError()<<std::endl;
        running=false;
        return;
    }

    menuMusic=Mix_LoadMUS("media/menumusic.mp3");
    if(!menuMusic)
    {
        std::cerr<<"Failed to load menu music! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
    }

    //Tải ảnh nền menu và kiểm tra quá trình tải
    backgroundTexture=IMG_LoadTexture(renderer,"media/menu_background.png");
    if(!backgroundTexture)
    {
        std::cerr<<"Failed to load menu background! IMG_Error: "<<IMG_GetError()<<std::endl;
    }

    //Thiết lập vị trị các nút trong menu
    int buttonWidth=300;
    int buttonHeight=60;
    int buttonGap=30;
    int startY=(SCREEN_HEIGHT/2)-buttonHeight-(buttonGap/2)-buttonHeight/2;
    onePlayerButton={(SCREEN_WIDTH-buttonWidth)/2,startY,buttonWidth,buttonHeight};
    twoPlayerButton={(SCREEN_WIDTH-buttonWidth)/2,startY+buttonHeight+buttonGap,buttonWidth,buttonHeight};
    exitButton={(SCREEN_WIDTH-buttonWidth)/2,startY+2*(buttonHeight+buttonGap),buttonWidth,buttonHeight};
}

void Menu::renderText(const char *text,SDL_Rect rect,SDL_Color color)
{
    //Tải phông và kiểm tra quá trình tải
    TTF_Font *font=TTF_OpenFont("media/consola.ttf",24);
    if(!font)
    {
        std::cerr<<"Failed to load font! TTF_Error: "<<TTF_GetError()<<std::endl;
        return;
    }

    //Tải bề mặt để chứa chữ, trên nền trong suốt
    SDL_Surface *textSurface=TTF_RenderUTF8_Solid(font,text,color);
    if(!textSurface)
    {
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture *textTexture=SDL_CreateTextureFromSurface(renderer,textSurface);

    //Center text in button
    SDL_Rect textRect={rect.x+(rect.w-textSurface->w)/2,rect.y+(rect.h-textSurface->h)/2,textSurface->w,textSurface->h};

    SDL_RenderCopy(renderer,textTexture,NULL,&textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void Menu::render()
{
    if(backgroundTexture)
    {
        SDL_RenderCopy(renderer,backgroundTexture,NULL,NULL);
    }
    else
    {
        //Fall back to black background if image couldn't be loaded
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
    }
    //Draw title
    SDL_Color titleColor={255,255,0,255};//Yellow
    TTF_Font *titleFont=TTF_OpenFont("media/consola.ttf",48);
    if(!titleFont)
    {
        //Fallback if font file not found
        titleFont=TTF_OpenFont("media/consola.ttf",48);
    }

    if(titleFont)
    {
        SDL_Surface *titleSurface=TTF_RenderUTF8_Solid(titleFont,"Battle City",titleColor);
        if(titleSurface)
        {
            SDL_Texture *titleTexture=SDL_CreateTextureFromSurface(renderer,titleSurface);
            SDL_Rect titleRect={(SCREEN_WIDTH-titleSurface->w)/2,100,titleSurface->w,titleSurface->h};
            SDL_RenderCopy(renderer,titleTexture,NULL,&titleRect);
            SDL_FreeSurface(titleSurface);
            SDL_DestroyTexture(titleTexture);
        }
        TTF_CloseFont(titleFont);
    }

    //Draw "One Player" button
    SDL_SetRenderDrawColor(renderer,100,100,255,255);//Blue button
    SDL_RenderFillRect(renderer,&onePlayerButton);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);//White border
    SDL_RenderDrawRect(renderer,&onePlayerButton);

    //Draw "Two Player" button
    SDL_SetRenderDrawColor(renderer,100,255,100,255);//Green button
    SDL_RenderFillRect(renderer,&twoPlayerButton);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);//White border
    SDL_RenderDrawRect(renderer,&twoPlayerButton);

    //Draw "Exit" button
    SDL_SetRenderDrawColor(renderer,255,100,100,255);//Red button
    SDL_RenderFillRect(renderer,&exitButton);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);//White border
    SDL_RenderDrawRect(renderer,&exitButton);

    //Draw button text
    SDL_Color textColor={255,255,255,255};//White text
    renderText("Một người chơi",onePlayerButton,textColor);
    renderText("Hai người chơi",twoPlayerButton,textColor);
    renderText("Thoát",exitButton,textColor);

    SDL_RenderPresent(renderer);
}

void Menu::handleEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type==SDL_QUIT)
        {
            running=false;
        }
        else if(event.type==SDL_MOUSEBUTTONDOWN)
        {
            int mouseX,mouseY;
            SDL_GetMouseState(&mouseX,&mouseY);

            //Check if One Player button was clicked
            if(mouseX>=onePlayerButton.x&&mouseX<=(onePlayerButton.x+onePlayerButton.w)&&
               mouseY>=onePlayerButton.y&&mouseY<=(onePlayerButton.y+onePlayerButton.h))
            {
                startGame=true;
                running=false;
                playerNum=1;
            }
            else if(mouseX>=twoPlayerButton.x&&mouseX<=(twoPlayerButton.x+twoPlayerButton.w)&&
                    mouseY>=twoPlayerButton.y&&mouseY<=(twoPlayerButton.y+twoPlayerButton.h))
            {
                startGame=true;
                running=false;
                playerNum=2;
            }
            //Check if Exit button was clicked
            else if(mouseX>=exitButton.x&&mouseX<=(exitButton.x+exitButton.w)&&
                    mouseY>=exitButton.y&&mouseY<=(exitButton.y+exitButton.h))
            {
                running=false;
            }
        }
    }
}

void Menu::run()
{
    if(menuMusic)
    {
        if(Mix_PlayMusic(menuMusic,0)==-1)
        {
            std::cerr<<"Failed to play menu music! SDL_mixer Error: "<<Mix_GetError()<<std::endl;
        }
    }
    while(running)
    {
        handleEvents();
        render();
        SDL_Delay(16);
    }
}

Menu::~Menu()
{
    if(backgroundTexture)
    {
        SDL_DestroyTexture(backgroundTexture);
    }
    if(menuMusic)
    {
        Mix_FreeMusic(menuMusic);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
