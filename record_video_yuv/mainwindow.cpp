#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_videoButton_clicked()
{
    if(!_videoThread){
        _videoThread = new VideoThread(this);
        _videoThread->start();
        connect(_videoThread,&VideoThread::finished,
                [this](){
            _videoThread = nullptr;
            ui->videoButton->setText("开始录视频");
        });
        ui->videoButton->setText("结束录制视频");
    }else{
        _videoThread->requestInterruption();
        _videoThread = nullptr;
        //设置按钮文字
        ui->videoButton->setText("开始录制视频");
    }
}
