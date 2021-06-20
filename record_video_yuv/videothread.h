#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>

class VideoThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoThread(QObject *parent = nullptr);
    ~VideoThread();

signals:

};

#endif // VIDEOTHREAD_H
