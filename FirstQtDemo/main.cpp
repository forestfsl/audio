#include "mainwindow.h"

#include <QApplication>
// 为了使用qDebug函数
#include <QDebug>



extern "C" {
#include <libavcodec/avcodec.h>
}

int main(int argc, char *argv[])
{
    // 打印版本信息
       qDebug() << av_version_info();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
