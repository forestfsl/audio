#include "ffmpegs.h"
#include <QDebug>
#include <QFile>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

// 输入缓冲区的大小
#define IN_DATA_SIZE 4096

FFmpegs::FFmpegs() {

}

static int frameIdx = 0;

static int decode(AVCodecContext *ctx,
                  AVPacket *pkt,
                  AVFrame *frame,
                  QFile &outFile) {
    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(ctx, pkt);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_send_packet error" << errbuf;
        return ret;
    }

    while (true) {
        // 获取解码后的数据
        ret = avcodec_receive_frame(ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            ERROR_BUF(ret);
            qDebug() << "avcodec_receive_frame error" << errbuf;
            return ret;
        }

        qDebug() << "解码出第" << ++frameIdx << "帧";

        // 将解码后的数据写入文件
        // 写入Y平面
//        outFile.write((char *) frame->data[0],
//                      frame->linesize[0] * ctx->height);
//        // 写入U平面
//        outFile.write((char *) frame->data[1],
//                      frame->linesize[1] * ctx->height >> 1);
//        // 写入V平面
//        outFile.write((char *) frame->data[2],
//                      frame->linesize[2] * ctx->height >> 1);

             /*   outFile.write((char *) frame->data[0],
                              921600);
                // 写入U平面
                outFile.write((char *) frame->data[1],
                              230400);
                // 写入V平面
                outFile.write((char *) frame->data[2],
                              230400);
//       qDebug() << frame->linesize[0] * ctx->height << (frame->linesize[1] * ctx->height >> 1) << (frame->linesize[2] * ctx->height >> 1);
//        return 0;
//  av_image_alloc(frame->data,frame->linesize,1280,720,AV_PIX_FMT_YUV420P,1); 连续内存地址
//                return;


        /*
         * frame->data[0] 0x7ffea128c000
         * frame->data[1] 0x7ffea1d33000
         * frame->data[2] 0x7ffea1d6d000
         *
         * frame->data[1] - frame->data[0] = 11169792 = y平面的大小
         * frame->data[2] - frame->data[1] = 237568 = u平面的大小
         *
         * y平面的大小 1280*720*1 = 921600
         * u平面的大小 1280*720*0.25 = 230400
         * v平面的大小 1280*720*0.25 =  230400
         */
 qDebug() << frame->data[0] << frame->data[1] << frame->data[2];

//        // 将解码后的数据写入文件(1382400)
        int imgSize = av_image_get_buffer_size(ctx->pix_fmt, ctx->width, ctx->height, 1);
        qDebug() << "输出一幁的大小" << imgSize;
//        // outFile.write((char *) frame->data[0], frame->linesize[0]);
        outFile.write((char *) frame->data[0], imgSize);
    }
}

void FFmpegs::h264Decode(const char *inFilename,
                         VideoDecodeSpec &out) {
    // 返回结果
    int ret = 0;

    // 用来存放读取的输入文件数据（h264）
    // 加上AV_INPUT_BUFFER_PADDING_SIZE是为了防止某些优化过的reader一次性读取过多导致越界
    char inDataArray[IN_DATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    char *inData = inDataArray;

    // 每次从输入文件中读取的长度（h264）
    // 输入缓冲区中，剩下的等待进行解码的有效数据长度
    int inLen;
    // 是否已经读取到了输入文件的尾部
    int inEnd = 0;

    // 文件
    QFile inFile(inFilename);
    QFile outFile(out.filename);

    // 解码器
    AVCodec *codec = nullptr;
    // 上下文
    AVCodecContext *ctx = nullptr;
    // 解析器上下文
    AVCodecParserContext *parserCtx = nullptr;

    // 存放解码前的数据(h264)
    AVPacket *pkt = nullptr;
    // 存放解码后的数据(yuv)
    AVFrame *frame = nullptr;

    // 获取解码器
    //    codec = avcodec_find_decoder_by_name("h264");
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "decoder not found";
        return;
    }

    // 初始化解析器上下文
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        qDebug() << "av_parser_init error";
        return;
    }

    // 创建上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug() << "avcodec_alloc_context3 error";
        goto end;
    }

    // 创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug() << "av_packet_alloc error";
        goto end;
    }

    // 创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }

    // 打开解码器
    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_open2 error" << errbuf;
        goto end;
    }

    // 打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error:" << inFilename;
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error:" << out.filename;
        goto end;
    }

    // 读取文件数据
    do {
        inLen = inFile.read(inDataArray, IN_DATA_SIZE);
        // 设置是否到了文件尾部
        inEnd = !inLen;

        // 让inData指向数组的首元素
        inData = inDataArray;

        // 只要输入缓冲区中还有等待进行解码的数据
        while (inLen > 0 || inEnd) {
            // 到了文件尾部（虽然没有读取任何数据，但也要调用av_parser_parse2，修复bug）

            // 经过解析器解析
            ret = av_parser_parse2(parserCtx, ctx,
                                   &pkt->data, &pkt->size,
                                   (uint8_t *) inData, inLen,
                                   AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

            if (ret < 0) {
                ERROR_BUF(ret);
                qDebug() << "av_parser_parse2 error" << errbuf;
                goto end;
            }

            // 跳过已经解析过的数据
            inData += ret;
            // 减去已经解析过的数据大小
            inLen -= ret;

            qDebug() << inEnd << pkt->size << ret;

            // 解码
            if (pkt->size > 0 && decode(ctx, pkt, frame, outFile) < 0) {
                goto end;
            }

            // 如果到了文件尾部
            if (inEnd) break;
        }
    } while (!inEnd);
//        while ((inLen = inFile.read(inDataArray, IN_DATA_SIZE)) > 0){
//            // 让inData指向数组的首元素
//            inData = inDataArray;
//            while (inLen > 0) {
//                // 经过解析器解析
//                ret = av_parser_parse2(parserCtx, ctx,
//                                       &pkt->data, &pkt->size,
//                                       (uint8_t *) inData, inLen,
//                                       AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

//                if (ret < 0) {
//                    ERROR_BUF(ret);
//                    qDebug() << "av_parser_parse2 error" << errbuf;
//                    goto end;
//                }

//                // 跳过已经解析过的数据
//                inData += ret;
//                // 减去已经解析过的数据大小
//                inLen -= ret;

//                // 解码
//                if (pkt->size > 0 && decode(ctx, pkt, frame, outFile) < 0) {
//                    goto end;
//                }
//            }
//        }

    // 刷新缓冲区
    //    pkt->data = nullptr;
    //    pkt->size = 0;
    //    decode(ctx, pkt, frame, outFile);
    decode(ctx, nullptr, frame, outFile);

    // 赋值输出参数
    out.fps = ctx->framerate.num;
    out.width = ctx->width;
    out.height = ctx->height;
    out.pixFmt = ctx->pix_fmt;
    // 用framerate.num获取帧率，并不是time_base.den

end:
    inFile.close();
    outFile.close();
    av_packet_free(&pkt);
    av_frame_free(&frame);
    av_parser_close(parserCtx);
    avcodec_free_context(&ctx);

// bug fix
// https://patchwork.ffmpeg.org/project/ffmpeg/patch/tencent_609A2E9F73AB634ED670392DD89A63400008@qq.com/

//
//    while ((inLen = inFile.read(inDataArray, IN_DATA_SIZE)) > 0){
//        while (inLen > 0) {
    // 让inData指向数组的首元素
//    inData = inDataArray;
//            // 经过解析器解析
//            ret = av_parser_parse2(parserCtx, ctx,
//                                   &pkt->data, &pkt->size,
//                                   (uint8_t *) inData, inLen,
//                                   AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

//            if (ret < 0) {
//                ERROR_BUF(ret);
//                qDebug() << "av_parser_parse2 error" << errbuf;
//                goto end;
//            }

//            // 跳过已经解析过的数据
//            inData += ret;
//            // 减去已经解析过的数据大小
//            inLen -= ret;

//            // 解码
//            if (pkt->size > 0 && decode(ctx, pkt, frame, outFile) < 0) {
//                goto end;
//            }
//        }
//    }
}
