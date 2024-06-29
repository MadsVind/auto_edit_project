#ifndef FFEMPEG_WRAPPER_HPP
#define FFEMPEG_WRAPPER_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/timestamp.h>
#include <libavutil/mathematics.h>
}


int initInputContext(const std::string& filename, AVFormatContext** pFormatCtx);
int initOutputContext(const std::string& filename, AVFormatContext** outFmtCtx);
int initOutputFile(const std::string& filename, AVFormatContext* outFmtCtx);
int copyStreamParameters(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx, int* video_stream_index, int* audio_stream_index);
int copyStreamParameters(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx);
int addPacketsToOutput(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx, 
                       int64_t* last_pts = nullptr, int64_t* last_dts = nullptr,
                       int64_t trim_start_millisec = 0, int64_t trim_end_millisec = 0);

#endif 