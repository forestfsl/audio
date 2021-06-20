#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "playthread.h"


#define END(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    disconnect();
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();

    qDebug() << this << "析构了";
}


void MainWindow::on_playButton_clicked()
{

//    PlayThread *thread = new PlayThread(this);
//    thread->start();



    // 矩形框
    SDL_Rect srcRect = {0, 0, 512, 512};
    SDL_Rect dstRect = {200, 200, 100, 100};
    SDL_Rect rect;

    // 初始化子系统
    END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);

    // 加载BMP
    _surface = SDL_LoadBMP("/Users/songlin/audio/sdl_show_bmp/in.bmp");
    END(!_surface, SDL_LoadBMP);

    // 创建窗口
    _window = SDL_CreateWindow(
                 // 标题
                 "SDL显示BMP图片",
                 // x
                 SDL_WINDOWPOS_UNDEFINED,
                 // y
                 SDL_WINDOWPOS_UNDEFINED,
                 // w
                 _surface->w,
                 // h
                 _surface->h,
                 SDL_WINDOW_SHOWN
             );
    END(!_window, SDL_CreateWindow);

    // 创建渲染上下文
    _renderer = SDL_CreateRenderer(_window, -1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer) {
        _renderer = SDL_CreateRenderer(_window, -1, 0);
        END(!_renderer, SDL_CreateRenderer);
    }

    // 创建纹理
    _texture = SDL_CreateTextureFromSurface(_renderer, _surface);
    END(!_texture, SDL_CreateTextureFromSurface);

    // 画一个红色的矩形框
    END(SDL_SetRenderDrawColor(_renderer,
                               255, 0, 0, SDL_ALPHA_OPAQUE),
        SDL_SetRenderDrawColor);
    rect = {0, 0, 50, 50};
    END(SDL_RenderFillRect(_renderer, &rect),
        SDL_RenderFillRect);

    // 设置绘制颜色（画笔颜色）
    END(SDL_SetRenderDrawColor(_renderer,
                               255, 255, 0, SDL_ALPHA_OPAQUE),
        SDL_SetRenderDrawColor);

    // 用绘制颜色（画笔颜色）清除渲染目标
    END(SDL_RenderClear(_renderer),
        SDL_RenderClear);

    // 拷贝纹理数据到渲染目标（默认是window）
    END(SDL_RenderCopy(_renderer, _texture, &srcRect, &dstRect),
        SDL_RenderCopy);

    // 更新所有的渲染操作到屏幕上
    SDL_RenderPresent(_renderer);

    SDL_Delay(2000);

end:
    SDL_FreeSurface(_surface);
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}
