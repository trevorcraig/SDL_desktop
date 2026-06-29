#include "video_player.h"
#include <stdio.h>


static bool SetupAudio(
    VideoPlayer* vp
)
{
    if (
        vp->audio_stream < 0
    )
    {
        printf(
            "No audio stream\n"
        );

        return false;
    }

    AVCodecParameters* p =
        vp->format
        ->streams[
            vp->audio_stream
        ]
        ->codecpar;

    const AVCodec* codec =
        avcodec_find_decoder(
            p->codec_id
        );

    if (!codec)
    {
        return false;
    }

    vp->audio_codec =
        avcodec_alloc_context3(
            codec
        );

    avcodec_parameters_to_context(
        vp->audio_codec,
        p
    );

    avcodec_open2(
        vp->audio_codec,
        codec,
        NULL
    );

    vp->audio_frame =
        av_frame_alloc();

    SDL_AudioSpec spec =
    {
        .format =
            SDL_AUDIO_F32,

        .channels =
            2,

        .freq =
            48000
    };

    vp->audio =
        SDL_OpenAudioDeviceStream(
            SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
            &spec,
            NULL,
            NULL
        );

    if (!vp->audio)
    {
        printf(
            "Audio create failed:\n%s\n",
            SDL_GetError()
        );

        return false;
    }

    if (
        vp->audio
    )
    {
        // SDL_ResumeAudioStream(vp->audio);
        SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(vp->audio));
        // SDL_ResumeAudioStreamDevice(
        //     vp->audio
        // );
    }

    vp->swr =
        swr_alloc();

    av_opt_set_chlayout(
        vp->swr,
        "in_chlayout",
        &vp->audio_codec->ch_layout,
        0
    );

    AVChannelLayout stereo =
        AV_CHANNEL_LAYOUT_STEREO;

    av_opt_set_chlayout(
        vp->swr,
        "out_chlayout",
        &stereo,
        0
    );

    av_opt_set_int(
        vp->swr,
        "in_sample_rate",
        vp->audio_codec->sample_rate,
        0
    );

    av_opt_set_int(
        vp->swr,
        "out_sample_rate",
        48000,
        0
    );

    av_opt_set_sample_fmt(
        vp->swr,
        "in_sample_fmt",
        vp->audio_codec->sample_fmt,
        0
    );

    av_opt_set_sample_fmt(
        vp->swr,
        "out_sample_fmt",
        AV_SAMPLE_FMT_FLT,
        0
    );

    swr_init(
        vp->swr
    );

    return true;
}

static bool PointInRect(
    float x,
    float y,
    SDL_FRect* r
)
{
    return (
        x >= r->x &&
        x <= r->x + r->w &&
        y >= r->y &&
        y <= r->y + r->h
    );
}

static void Render(
    Widget* w,
    SDL_Renderer* r
)
{
    VideoPlayer* vp =
        (VideoPlayer*)w;

    if (
        vp->playing
    )
    {
        VideoPlayer_Update(
            vp
        );
    }

    if (
        vp->texture
    )
    {
        SDL_RenderTexture(
            r,
            vp->texture,
            NULL,
            &w->rect
        );
    }
        SDL_FRect bar =
        {
            w->rect.x,
            w->rect.y
            +
            w->rect.h
            -
            20,

            w->rect.w,

            20
        };

        SDL_SetRenderDrawColor(
            r,
            60,
            60,
            60,
            255
        );

        SDL_RenderFillRect(
            r,
            &bar);
    float progress =
        0.0f;

    if (
        vp->duration_sec > 0
    )
    {
        progress =
            vp->current_sec
            /
            vp->duration_sec;

        if (
            progress > 1.0f
        )
        {
            progress =
                1.0f;
        }
    }

    SDL_FRect fill =
    {
        bar.x,
        bar.y,

        bar.w
        *
        progress,

        bar.h
    };

    SDL_SetRenderDrawColor(
        r,
        50,
        180,
        255,
        255
    );

    SDL_RenderFillRect(
        r,
        &fill
    );
}

static void Handle(
    Widget* w,
    SDL_Event* e
)
{
    VideoPlayer* vp =
        (VideoPlayer*)w;

    if (
        e->type
        !=
        SDL_EVENT_MOUSE_BUTTON_DOWN
    )
    {
        return;
    }

    //--------------------------------
    // Seek bar area
    //--------------------------------

    SDL_FRect bar =
    {
        w->rect.x,

        w->rect.y
        +
        w->rect.h
        -
        20,

        w->rect.w,

        20
    };

    float mx =
        e->button.x;

    float my =
        e->button.y;

    if (
        PointInRect(
            mx,
            my,
            &bar
        )
    )
    {
        double pct =
            (
                mx
                -
                bar.x
            )
            /
            bar.w;

        if (
            pct < 0
        )
        {
            pct =
                0;
        }

        if (
            pct > 1
        )
        {
            pct =
                1;
        }
        VideoPlayer_Seek(
            vp,
            pct
            *
            vp->duration_sec
        );
    }
}

void VideoPlayer_Init(
    VideoPlayer* vp,
    SDL_Renderer* renderer,
    float x,
    float y,
    float w,
    float h,
    const char* file
)
{
    memset(
        vp,
        0,
        sizeof(
            VideoPlayer
        )
    );

    vp->base.rect =
        (SDL_FRect)
        {
            x,
            y,
            w,
            h
        };

    vp->base.Render =
        Render;

    vp->base.Handle =
        Handle;

    vp->playing =
        true;

    vp->speed =
        1.0f;

    if (
        file
    )
    {
        VideoPlayer_Load(
            vp,
            renderer,
            file
        );
    }
}

void VideoPlayer_Update(VideoPlayer* vp){
    //--------------------------------
    // No file loaded yet
    //--------------------------------

    if (
        !vp ||
        !vp->format ||
        !vp->frame ||
        !vp->packet ||
        !vp->texture
    )
    {
        return;
    }
    // Paused    
    if (!vp->playing){
        return;
    }
    // 1. If we have a frame ready, check if it's time to display it
    if (vp->frame->pts != AV_NOPTS_VALUE && !vp->first_frame && !vp->seeking) {
        // Calculate exactly how many milliseconds into the video this frame belongs
        // uint64_t frame_target_ms = (uint64_t)(vp->frame->pts * vp->time_base * 1000.0);
        uint64_t frame_target_ms =(uint64_t)(vp->frame->pts * vp->time_base * 1000.0/ vp->speed);
        uint64_t real_elapsed_ms = SDL_GetTicks() - vp->start_time_ms;
        // vp->current_sec = vp->frame->pts*vp->time_base;
        if (vp->frame->pts != AV_NOPTS_VALUE)
        {
            vp->current_sec = vp->frame->pts * vp->time_base;
        }

        // If the real-world clock hasn't caught up to the frame's timestamp yet, 
        // return early and keep displaying the current texture frame.
        if (real_elapsed_ms < frame_target_ms) {
            return; 
        }     
    }

    // 2. Time to read the next frame from the file
 
    if (av_read_frame(vp->format, vp->packet) >= 0)
    {
        if (vp->packet->stream_index == vp->stream)
        {
            avcodec_send_packet(vp->codec, vp->packet);

            if (avcodec_receive_frame(vp->codec, vp->frame) == 0)
            {
                // Synchronize our master clock when the very first frame hits the screen
                if (vp->first_frame) {
                    vp->start_time_ms = SDL_GetTicks();
                    vp->first_frame = false;
                }
                if (vp->seeking)
                {
                    vp->seeking = false;
                    vp->first_frame = false;
                    vp->start_time_ms = SDL_GetTicks();
                }                

                // Upload the validated frame to the GPU texture layout
                SDL_UpdateYUVTexture(
                    vp->texture,
                    NULL,
                    vp->frame->data[0], vp->frame->linesize[0],
                    vp->frame->data[1], vp->frame->linesize[1],
                    vp->frame->data[2], vp->frame->linesize[2]
                );
            }
        }
        else if (
            vp->packet->stream_index
            ==
            vp->audio_stream
        )
        {
            avcodec_send_packet(
                vp->audio_codec,
                vp->packet
            );

            while (
                avcodec_receive_frame(
                    vp->audio_codec,
                    vp->audio_frame
                ) == 0
            )
            {
                float samples[
                    8192
                ];

                uint8_t* out[] =
                {
                    (uint8_t*)samples
                };

                int frames =
                    swr_convert(
                        vp->swr,
                        out,
                        4096,
                        (
                            const uint8_t**)
                            vp->audio_frame->data,
                        vp->audio_frame->nb_samples
                    );

                if (
                    frames > 0
                )
                {
                    SDL_PutAudioStreamData(
                        vp->audio,
                        samples,
                        frames
                        *
                        sizeof(float)
                        *
                        2
                    );
                }
            }
        }
        av_packet_unref(vp->packet);
    }
    
}

void VideoPlayer_Destroy(
    VideoPlayer* vp
)
{
    if (vp->texture)
    {
        SDL_DestroyTexture(
            vp->texture
        );
    }
    if (vp->frame)
    {
        av_frame_free(
            &vp->frame
        );
    }
    if (vp->packet)
    {
        av_packet_free(&vp->packet);
    }
    if (vp->codec)
    {
        avcodec_free_context(&vp->codec);
    }
    if (vp->format)
    {
        avformat_close_input(&vp->format);
    }
    if (
        vp->audio
    )
    {
        SDL_DestroyAudioStream(
            vp->audio
        );
    }

    if (
        vp->audio_frame
    )
    {
        av_frame_free(
            &vp->audio_frame
        );
    }

    if (
        vp->audio_codec
    )
    {
        avcodec_free_context(
            &vp->audio_codec
        );
    }

    if (
        vp->swr
    )
    {
        swr_free(
            &vp->swr
        );
    }

}

void VideoPlayer_Play(
    VideoPlayer* vp
)
{
    if (
        vp->playing
    )
    {
        return;
    }

    vp->playing =
        true;

    //--------------------------------
    // Resume without jumping
    //--------------------------------

    vp->start_time_ms +=
        SDL_GetTicks()
        -
        vp->paused_time_ms;
}

void VideoPlayer_Stop(
    VideoPlayer* vp
)
{
    if (
        !vp->playing
    )
    {
        return;
    }

    vp->playing =
        false;

    vp->paused_time_ms =
        SDL_GetTicks();
}

void VideoPlayer_SetSpeed(
    VideoPlayer* vp,
    float speed
)
{
    if (
        speed <= 0.0f
    )
    {
        return;
    }

    //--------------------------------
    // Keep current position
    //--------------------------------

    uint64_t now =
        SDL_GetTicks();

    uint64_t elapsed =
        now
        -
        vp->start_time_ms;

    vp->start_time_ms =now-(uint64_t)(elapsed*(vp->speed/speed));
    vp->speed =speed;
}

bool VideoPlayer_Load(
    VideoPlayer* vp,
    SDL_Renderer* renderer,
    const char* file
)
{
    if (
        vp->texture
    )
    {
        VideoPlayer_Destroy(
            vp
        );
    }

    if (
        avformat_open_input(
            &vp->format,
            file,
            NULL,
            NULL
        ) < 0
    )
    {
        return false;
    }

    AVDictionary* opts =NULL;

    av_dict_set(
        &opts,
        "analyzeduration",
        "10000000",
        0
    );

    av_dict_set(
        &opts,
        "probesize",
        "10000000",
        0
    );

    avformat_find_stream_info(
        vp->format,
        &opts
    );

    av_dict_free(
        &opts
    );
    vp->audio_stream =
    av_find_best_stream(
        vp->format,
        AVMEDIA_TYPE_AUDIO,
        -1,
        -1,
        NULL,
        0
    );
    SetupAudio(vp);

    vp->stream =
        av_find_best_stream(
            vp->format,
            AVMEDIA_TYPE_VIDEO,
            -1,
            -1,
            NULL,
            0
        );

    if (
        vp->stream < 0
    )
    {
        return false;
    }

    AVCodecParameters* p =
        vp->format
        ->streams[
            vp->stream
        ]
        ->codecpar;

    const AVCodec* codec =
        avcodec_find_decoder(
            p->codec_id
        );

    vp->codec =
        avcodec_alloc_context3(
            codec
        );

    avcodec_parameters_to_context(
        vp->codec,
        p
    );

    avcodec_open2(
        vp->codec,
        codec,
        NULL
    );

    vp->frame =
        av_frame_alloc();

    vp->packet =
        av_packet_alloc();

    vp->texture =
        SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_IYUV,
            SDL_TEXTUREACCESS_STREAMING,
            vp->codec->width,
            vp->codec->height
        );

    vp->time_base =
        av_q2d(
            vp
            ->format
            ->streams[
                vp->stream
            ]
            ->time_base
        );

    vp->duration_sec =(double)vp->format->duration / AV_TIME_BASE;
    vp->first_frame =
        true;

    vp->start_time_ms =
        SDL_GetTicks();

    vp->playing =
        true;
    printf(
    "Opened: %s\n",
    file
    );

    printf(
        "Streams: %d\n",
        vp->format->nb_streams
    );

    printf(
        "Video: %dx%d\n",
        vp->codec->width,
        vp->codec->height
    );

    return true;
}

void VideoPlayer_Seek(VideoPlayer* vp, double sec)
{
    if (!vp || !vp->format) return;

    vp->seeking = true;
    vp->recovering = true;

    int64_t ts = (int64_t)(sec / vp->time_base);

    av_seek_frame(
        vp->format,
        vp->stream,
        ts,
        AVSEEK_FLAG_BACKWARD
    );

    // 🔥 flush VIDEO decoder
    avcodec_flush_buffers(vp->codec);

    // 🔥 flush AUDIO decoder 
    if (vp->audio_codec)
        avcodec_flush_buffers(vp->audio_codec);
    // Audio rebuild    
    swr_free(&vp->swr);

    vp->swr = swr_alloc();

    av_opt_set_chlayout(vp->swr, "in_chlayout", &vp->audio_codec->ch_layout, 0);

    AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
    av_opt_set_chlayout(vp->swr, "out_chlayout", &stereo, 0);

    av_opt_set_int(vp->swr, "in_sample_rate", vp->audio_codec->sample_rate, 0);
    av_opt_set_int(vp->swr, "out_sample_rate", 48000, 0);

    av_opt_set_sample_fmt(vp->swr, "in_sample_fmt", vp->audio_codec->sample_fmt, 0);
    av_opt_set_sample_fmt(vp->swr, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

    swr_init(vp->swr);

    vp->current_sec = sec;
    vp->first_frame = true;

    vp->start_time_ms = SDL_GetTicks();

    vp->seeking = false;
}