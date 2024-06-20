#include <iostream>
#include <memory>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/timestamp.h>
#include <libavutil/mathematics.h>
}

int init_input_context(const std::string& filename, AVFormatContext** pFormatCtx) {
    // Open video file
    if (avformat_open_input(pFormatCtx, filename.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return -1;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(*pFormatCtx, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        return -1;
    }

    return 0;
}

int init_output_context(const std::string& filename, AVFormatContext** outFmtCtx) {
    if (avformat_alloc_output_context2(outFmtCtx, nullptr, nullptr, filename.c_str()) < 0) {
        std::cerr << "Could not create output context\n";
        return -1;
    }
    return 0;
}

int init_output_file(const std::string& filename, AVFormatContext* outFmtCtx) {
    if (!(outFmtCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&outFmtCtx->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
            std::cerr << "Could not open output file " << filename << std::endl;
            return -1;
        }
    }

    if (avformat_write_header(outFmtCtx, nullptr) < 0) {
        std::cerr << "Error occurred when opening output file\n";
        return -1;
    }

    return 0;
}

int copy_stream_parameters(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx, int& video_stream_index) {
    for (int i = 0; i < inFmtCtx->nb_streams; i++) {
        AVStream* out_stream;
        AVStream* in_stream = inFmtCtx->streams[i];
        AVCodecParameters* in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO && in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
            continue;
        }
        else if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }

        out_stream = avformat_new_stream(outFmtCtx, nullptr);
        if (!out_stream) {
            std::cerr << "Failed allocating output stream\n";
            return -1;
        }

        if (avcodec_parameters_copy(out_stream->codecpar, in_codecpar) < 0) {
            std::cerr << "Failed to copy codec parameters\n";
            return -1;
        }
        out_stream->codecpar->codec_tag = 0;
    }
    return 0;
}

int add_packets_to_output(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx, 
                          uint64_t* last_pts = nullptr, uint64_t* last_dts = nullptr, 
                          int64_t pts_offset = 0, int64_t dts_offset = 0, 
                          int64_t trim_start_millisec = 0, int64_t trim_end_millisec = 0) {

    AVPacket pkt;
    bool is_trimmed = (trim_start_millisec > 0 || trim_end_millisec > 0);
    while (av_read_frame(inFmtCtx, &pkt) >= 0) {
        // Copy packet
        AVStream *in_stream, *out_stream;
        in_stream = inFmtCtx->streams[pkt.stream_index];
        out_stream = outFmtCtx->streams[pkt.stream_index];

        if (is_trimmed && (av_q2d(in_stream->time_base) * pkt.pts) * 1000  > trim_end_millisec) {
            *last_dts = pkt.dts;
            *last_pts = pkt.pts;
            av_packet_unref(&pkt);
            break;
        }

        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF) + pts_offset;
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF) + dts_offset;
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;

        // Write packet
        if (av_interleaved_write_frame(outFmtCtx, &pkt) < 0) {
            std::cerr << "Error muxing packet\n";
            break;
        }
        av_packet_unref(&pkt);
    }
    return 0;
}

int main() {
    // Your existing code to open the input file and find stream info...
    
    std::string inputFile = "/home/vind/auto_edit_project/test/unit/test_videos/example1.mp4";
    std::string inputFile2 = "/home/vind/auto_edit_project/test/unit/test_videos/example2.mp4";
    std::string outputFile = "/home/vind/auto_edit_project/test/unit/test_videos/output.mp4";

    const char * videoPath = inputFile.c_str();
    const char * videoPath2 = inputFile2.c_str();
    const char * outPath = outputFile.c_str();

    AVFormatContext* pFormatCtx = nullptr;
    AVFormatContext* pFormatCtx2 = nullptr;
    AVFormatContext* outFmtCtx = nullptr;

    init_input_context(videoPath, &pFormatCtx);
    init_input_context(videoPath2, &pFormatCtx2);
    init_output_context(outPath, &outFmtCtx);
 

    if (pFormatCtx->duration != AV_NOPTS_VALUE) {
        int64_t duration_in_millisecs = pFormatCtx->duration / 1000;
        std::cout << "Duration: " << duration_in_millisecs << " milliseconds" << std::endl;
    } else {
        std::cout << "Duration: Unknown" << std::endl;
    }

    // Calculate half duration
    int64_t half_duration = (pFormatCtx->duration / 2) / 1000;
    int video_stream_index = -1;
    int video_stream_index2 = -1;

    copy_stream_parameters(pFormatCtx, outFmtCtx, video_stream_index);
    copy_stream_parameters(pFormatCtx2, outFmtCtx, video_stream_index2);
    init_output_file(outPath, outFmtCtx);

    uint64_t last_pts = 0;
    uint64_t last_dts = 0;

    add_packets_to_output(pFormatCtx, outFmtCtx, &last_pts, &last_dts, 0, 0, 0, half_duration);

    int64_t pts_offset = av_rescale_q(last_pts + 1, pFormatCtx2->streams[video_stream_index2]->time_base, outFmtCtx->streams[video_stream_index]->time_base);
    int64_t dts_offset = av_rescale_q(last_dts + 1, pFormatCtx2->streams[video_stream_index2]->time_base, outFmtCtx->streams[video_stream_index]->time_base);

    add_packets_to_output(pFormatCtx2, outFmtCtx, &last_pts, &last_dts, pts_offset, dts_offset);


    av_write_trailer(outFmtCtx);

    // Cleanup
    if (!(outFmtCtx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&outFmtCtx->pb);
    avformat_free_context(outFmtCtx);

    avformat_close_input(&pFormatCtx);
    return 0;
}



//#include "main.hpp"
//
//int main() {
//    App app;
//    app.menu();
//    return 0;
//}
