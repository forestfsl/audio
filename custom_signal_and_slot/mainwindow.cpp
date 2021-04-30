#include "mainwindow.h"
#include "counter.h"
#include "senderobj.h"
#include "receiverobj.h"

//https://doc.qt.io/qt-5/signalsandslots.html
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //创建对象
    SenderObj *sender = new SenderObj;
    receiverObj *receiver = new receiverObj;
    //连接
    connect(sender,&SenderObj::exit,receiver,&receiverObj::handleExit);

    //发出信号，最终会调用receiverobj::handleExit 函数
    emit sender->exit();
    //销毁对象
    delete sender;
    delete  receiver;
}


MainWindow::~MainWindow()
{
}

void testCounter(){
    Counter a, b;
       QObject::connect(&a, &Counter::valueChanged,
                        &b, &Counter::setValue);

       /*Calling a.setValue(12) makes a emit a valueChanged(12) signal,
               which b will receive in its setValue() slot, i.e. b.setValue(12) is called.
               Then b emits the same valueChanged() signal, but since no slot has been
               connected to  b's valueChanged() signal, the signal is ignored.*/
       a.setValue(12);     // a.value() == 12, b.value() == 12
       b.setValue(48);     // a.value() == 12, b.value() == 48
}
