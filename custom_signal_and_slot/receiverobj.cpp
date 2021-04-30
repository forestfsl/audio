#include "receiverobj.h"
#include <QDebug>

receiverObj::receiverObj(QObject *parent) : QObject(parent)
{

}


//实现槽函数，编写处理信号的代码
void receiverObj::handleExit(){
    qDebug() << "receiverobj::handleExit()";
}
