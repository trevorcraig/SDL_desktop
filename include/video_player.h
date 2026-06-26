#pragma once

#include <SDL3/SDL.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include "widget.h"

typedef struct
{
    Widget base;

    AVFormatContext* format;

    AVCodecContext* codec;

    SwsContext* sws;

    AVFrame* frame;

    AVFrame* rgb;

    AVPacket* packet;

    SDL_Texture* texture;

    int stream;
    
    uint64_t start_time_ms;  // When the video started playing
    double time_base;        // Internal clock factor of the video stream
    bool first_frame;        // Track if we need to initialize our clock
    uint64_t paused_time_ms;
    
    bool playing;
    float speed;
    double duration_sec;

    double current_sec;

    bool seeking;
    int audio_stream;

    AVCodecContext* audio_codec;

    SwrContext* swr;

    SDL_AudioStream* audio;
}
VideoPlayer;

void VideoPlayer_Init(
    VideoPlayer* vp,
    SDL_Renderer* renderer,
    float x,
    float y,
    float w,
    float h,
    const char* file
);
void VideoPlayer_Play(
    VideoPlayer* vp
);
void VideoPlayer_Update(
    VideoPlayer* vp
);

void VideoPlayer_Destroy(
    VideoPlayer* vp
);

void VideoPlayer_Stop(
    VideoPlayer* vp
);

void VideoPlayer_SetSpeed(
    VideoPlayer* vp,
    float speed
);

bool VideoPlayer_Load(
    VideoPlayer* vp,
    SDL_Renderer* renderer,
    const char* file
);
void VideoPlayer_Seek(VideoPlayer* vp, double sec);