#include "playthread.h"
#include <SDL2/SDL.h>
#include <QDebug>
#include <QFile>

#define FILENAME "/Users/songlin/audio/qt_record/05_05_23_05_35.pcm"
//采样率
#define SAMPLE_RATE 44100
//采样格式
#define SAMPLE_FORMAT AUDIO_F32LSB
//采样大小
#define SAMPLE_SIZE SDL_AUDIO_BITSIZE(SAMPLE_FORMAT)
//声道数
#define CHANNELS 2
// 音频缓冲区的样本数量
#define SAMPLES 1024
//每个样本占用多少个字节
#define BYTES_PER_SAMPLE ((SAMPLE_SIZE * CHANNELS) >> 3)
//文件缓冲区的大小
#define BUFFER_SIZE (SAMPLES * BYTES_PER_SAMPLE)

typedef struct {
    int len = 0;
    int pullLen = 0;
    Uint8 *data = nullptr;
} AudioBuffer;

PlayThread::PlayThread(QObject *parent) : QThread(parent) {
    connect(this, &PlayThread::finished,
            this, &PlayThread::deleteLater);

}

PlayThread::~PlayThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();

    qDebug() << this << "析构了";
}

//等待音频设备回调(会回掉多次)
//stream:需要往stream中填充PCM数据
//len:希望填充的大小(samples * format * channels / 8)
void pull_audio_data(void *userdata,Uint8 *stream,int len){
    qDebug() <<"pull_audio_data" << len;
    //清空stream（静音处理）
    SDL_memset(stream,0,len);
    //去除AudioBuffer
    AudioBuffer *buffer = (AudioBuffer *)userdata;
    //文件数据还没准备好
    if (buffer->len <= 0) return;

    //取len，bufferLen的最小值(为了保证数据安全，防止指针越界)
    buffer->pullLen = (len > buffer->len) ? buffer->len : len;
    //填充数据
    SDL_MixAudio(stream,buffer->data,buffer->pullLen,SDL_MIX_MAXVOLUME);
    buffer->data += buffer->pullLen;
    buffer->len -= buffer->pullLen;
}

/*
 * SDL 播放音频有2种模式：
 * Push(推）：【程序】主动推胸数据给音频设备
 * Pull (拉）；【音频设备】主动向【程序】拉去数据
 */
 void PlayThread::run() {
     //初始化Audio子系统
     if (SDL_Init(SDL_INIT_AUDIO)){
         qDebug() << "SDL_Init error" << SDL_GetError();
         return;
     }
     //音频参数
     SDL_AudioSpec spec;
     //采样率
     spec.freq = SAMPLE_RATE;
     //采样格式(s161e)
     spec.format = SAMPLE_FORMAT;
     //声道数
     spec.channels = CHANNELS;
     //音频缓冲区的样本数量(这个值必须是2的幂)
     spec.samples = SAMPLES;
     //回调
     spec.callback = pull_audio_data;
     //传递给回调的参数
     AudioBuffer buffer;
     spec.userdata = &buffer;

     //打开设备
     if (SDL_OpenAudio(&spec,nullptr)){
         qDebug() << "SDL_OepnAudio error" << SDL_GetError();
         //清除所有的子系统
         SDL_Quit();
         return;
     }

     //打开文件
     QFile file(FILENAME);
     if(!file.open(QFile::ReadOnly)){
         qDebug() << "file open error" << FILENAME;
         //清除所有的子系统
         SDL_Quit();
         return;
     }
     //开始播放(0是取消暂停)
     SDL_PauseAudio(0);

     //存放从文件中读取的数据
     Uint8 data[BUFFER_SIZE];
     while(!isInterruptionRequested()){
         // 只要从文件中读取的音频数据，还没有填充完毕，就跳过
         if (buffer.len > 0) continue;
         buffer.len = file.read((char *) data, BUFFER_SIZE);
        // 文件数据已经读取完毕
         if (buffer.len <= 0) {
             // 剩余的样本数量
             int samples = buffer.pullLen / BYTES_PER_SAMPLE;
             int ms = samples * 1000 / SAMPLE_RATE;
             SDL_Delay(ms);
                    break;
         }
         // 读取到了文件数据
         buffer.data = data;
     }

     // 关闭文件
     file.close();

     // 关闭设备
     SDL_CloseAudio();

     // 清除所有的子系统
     SDL_Quit();
 }
