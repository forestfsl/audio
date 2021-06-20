#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>

class AudioThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(QObject *parent = nullptr);
    ~AudioThread();
private:
    void run();

signals:

};

#endif // AUDIOTHREAD_H
