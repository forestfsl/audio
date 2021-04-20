#include "mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
       //设置窗口标题
       setWindowTitle("主窗口");
       //设置窗口大小,可以自由拖动
       resize(600,600);
       //设置窗口的固定大小，窗口不能通过拖拽边缘进行自由伸缩
//       setFixedSize(600,600);
       //设置窗口的位置，以父控件的左上角为坐标原点，没有父控件的情况，就以屏幕的左上角作为坐标原点
       move(200,200);

       // 创建按钮
       QPushButton *btn = new QPushButton;
       // 设置按钮的文字
       btn->setText("登录");
       // 设置父控件为当前窗口
       btn->setParent(this);
       // 设置按钮的位置和大小
       btn->move(50, 50);
       btn->resize(100, 50);

       // 创建第2个按钮
       new QPushButton("注册", this);
}

MainWindow::~MainWindow()
{
}

