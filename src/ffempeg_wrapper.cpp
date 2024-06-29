#include "ffempeg_wrapper.hpp"

int initInputContext(const std::string& filename, AVFormatContext** pFormatCtx) {
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

int initOutputContext(const std::string& filename, AVFormatContext** outFmtCtx) {
    if (avformat_alloc_output_context2(outFmtCtx, nullptr, nullptr, filename.c_str()) < 0) {
        std::cerr << "Could not create output context\n";
        return -1;
    }
    return 0;
}

int initOutputFile(const std::string& filename, AVFormatContext* outFmtCtx) {
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

int copyStreamParameters(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx, int* video_stream_index, int* audio_stream_index) {
    for (int i = 0; i < inFmtCtx->nb_streams; i++) {
        AVStream* out_stream;
        AVStream* in_stream = inFmtCtx->streams[i];
        AVCodecParameters* in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            *video_stream_index = i;
        } else if (in_codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            *audio_stream_index = i;
        } else {
            continue;
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

int copyStreamParameters(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx) {
    return copyStreamParameters(inFmtCtx, outFmtCtx, (new int), (new int));
}

void addStreamPacketsToOutput(std::vector<AVPacket> packets, bool is_trim_start, bool is_trim_end, 
                             int64_t start_time, int64_t trim_end_millisec, 
                             AVFormatContext* out_format_context, AVStream* out_stream, 
                             int64_t* last_pts, int64_t *temp_pts) {
                                
    for (auto pkt : packets) {
        int64_t current_time = (av_q2d(out_stream->time_base) * pkt.pts) * 1000;

        if (is_trim_start && current_time < start_time) {                                                                                       
           *last_pts = -pkt.pts;
            av_packet_unref(&pkt);
            continue;
        } 

        pkt.pts = pkt.pts + *last_pts;
        pkt.dts = pkt.dts + *last_pts;

        current_time = (av_q2d(out_stream->time_base) * pkt.pts) * 1000;
    
        *temp_pts = pkt.pts; 
        if (is_trim_end && (current_time > trim_end_millisec)) {
            av_packet_unref(&pkt);
            break;
        } 

        // Write packet
        if (av_interleaved_write_frame(out_format_context, &pkt) < 0) {
            std::cerr << "Error muxing packet\n";
            break;
        }
        av_packet_unref(&pkt);
    }
}

// !!! Does not yet trim start !!! needs to be simplyfied
int addPacketsToOutput(AVFormatContext* in_format_context, AVFormatContext* out_format_context, 
                       int64_t* last_video_pts, int64_t* last_audio_pts,
                       int64_t start_time_ms, int64_t end_time_ms) {

    AVPacket pkt;
    std::vector<AVPacket> video_packets;
    std::vector<AVPacket> audio_packets;

    int64_t temp_video_pts = 0;
    int64_t temp_audio_pts = 0;

    bool is_trim_start = start_time_ms > 0;
    bool is_trim_end = end_time_ms > 0;

    AVStream *out_video_stream = nullptr; 
    AVStream *out_audio_stream = nullptr; 

    while (av_read_frame(in_format_context, &pkt) >= 0) {
        AVStream *in_stream = in_format_context->streams[pkt.stream_index];
        AVStream *out_stream = out_format_context->streams[pkt.stream_index];
        const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(out_stream->codecpar->codec_id);

        if (codec_desc->type == AVMEDIA_TYPE_VIDEO && out_video_stream == nullptr) out_video_stream = out_stream;
        if (codec_desc->type == AVMEDIA_TYPE_AUDIO && out_audio_stream == nullptr) out_audio_stream = out_stream;
        
        pkt.pts = av_rescale_q_rnd(pkt.pts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF);
        pkt.dts = av_rescale_q_rnd(pkt.dts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;

        if      (codec_desc->type == AVMEDIA_TYPE_VIDEO) video_packets.push_back(pkt);
        else if (codec_desc->type == AVMEDIA_TYPE_AUDIO) audio_packets.push_back(pkt);
        else av_packet_unref(&pkt);
    }

    int64_t start_time_keyframe = -1;

    for (AVPacket pkt : video_packets) {
        int64_t current_time = (av_q2d(out_video_stream->time_base) * pkt.pts) * 1000;
        if (pkt.flags == 1) start_time_keyframe = current_time;
        if (current_time > start_time_ms) break;  
    }

    addStreamPacketsToOutput(video_packets, 
                             is_trim_start, is_trim_end, 
                             start_time_keyframe, end_time_ms, 
                             out_format_context, out_video_stream, 
                             last_video_pts, &temp_video_pts);
                             
    addStreamPacketsToOutput(audio_packets, 
                             is_trim_start, is_trim_end, 
                             start_time_keyframe, end_time_ms, 
                             out_format_context, out_audio_stream, 
                             last_audio_pts, &temp_audio_pts);

    *last_video_pts = temp_video_pts;
    *last_audio_pts = temp_audio_pts;
    std::cout << "last_video_pts: " << *last_video_pts << std::endl;
    std::cout << "last_audio_pts: " << *last_audio_pts << std::endl;
    return 0;
}
