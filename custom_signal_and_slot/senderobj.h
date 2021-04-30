#ifndef SENDEROBJ_H
#define SENDEROBJ_H

#include <QObject>

class SenderObj : public QObject
{
    Q_OBJECT
public:
    explicit SenderObj(QObject *parent = nullptr);

signals:
    //自定义信号
    void exit();

};

#endif // SENDEROBJ_H
