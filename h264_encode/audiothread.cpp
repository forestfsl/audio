#include "audiothread.h"
#include <QDebug>
#include "ffmpegs.h"

AudioThread::AudioThread(QObject *parent) : QThread(parent)
{
    //当监听到线程结束时(finished),就调用deleteLater回收内存
    connect(this,&AudioThread::finished,
            this,&AudioThread::deleteLater);
}

AudioThread::~AudioThread(){
    //断开所有的连接
    disconnect();
    //内存回收之前，正常结束线程
    requestInterruption();
    //安全退出
    quit();
    wait();
    qDebug() << this << "析构 （内存被回收）";
}

void AudioThread::run(){
    VideoEncodeSpec in;
    in.filename = "/Users/songlin/audio/h264_encode/352x288-yuv420p.yuv";
    in.width = 352;
    in.height = 288;
    in.fps = 25;//这里不是30而是25，是因为终端输出的是25
    in.pixFmt = AV_PIX_FMT_YUV420P;
    FFmpegs::h264Encode(in,"/Users/songlin/audio/h264_encode/out.h264");

// ffmpeg -s 352x288 -pix_fmt yuv420p -i 352x288-yuv420p.yuv -framerate 30 -c:v libx264 outCommand.h264 命令行
//    Stream #0:0: Video: h264 (libx264), yuv420p, 352x288, q=-1--1, 25 fps, 25 tbn, 25 tbc
}
