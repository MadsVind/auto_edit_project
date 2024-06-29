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

// !!! Does not yet trim start !!! needs to be simplyfied
int addPacketsToOutput(AVFormatContext* in_format_context, AVFormatContext* out_format_context, 
                       int64_t* last_video_pts, int64_t* last_audio_pts,
                       int64_t trim_start_millisec, int64_t trim_end_millisec) {

    AVPacket pkt;
    uint64_t temp_video_pts = 0;
    uint64_t temp_audio_pts = 0;

    bool is_video_over_trim = false;
    bool is_audio_over_trim = false;

    bool is_trim_start = trim_start_millisec > 0;
    bool is_trim_end = trim_end_millisec > 0;

    bool first_keyframe_found = false;

    while (av_read_frame(in_format_context, &pkt) >= 0) {
        AVStream *in_stream = in_format_context->streams[pkt.stream_index];
        AVStream *out_stream = out_format_context->streams[pkt.stream_index];
        const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(out_stream->codecpar->codec_id);

        bool is_video = codec_desc->type == AVMEDIA_TYPE_VIDEO;
        bool is_audio = codec_desc->type == AVMEDIA_TYPE_AUDIO;

        pkt.pts = av_rescale_q_rnd(pkt.pts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF);
        pkt.dts = av_rescale_q_rnd(pkt.dts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF);

        uint64_t current_duration = (av_q2d(out_stream->time_base) * pkt.pts) * 1000; 


        /**
         * problem is that first frame is not a keyframe, there is only 2 keyframes in example video
         * find out how to encode
         * 
         * ez solution is to save all pts durations after first video keyframe and then add to output, 
         * but will have to be added to vector since the stream is not garenteed to be video first.
         */
        if (is_trim_start && (current_duration < trim_start_millisec) || first_keyframe_found && !(pkt.flags == 1)) {                                                                                       
            if (is_audio) *last_audio_pts = -pkt.pts;
            if (is_video) *last_video_pts = -pkt.pts;
            av_packet_unref(&pkt);
            continue;
        } 

        
        int offset = (is_video) ? *last_video_pts : *last_audio_pts;
        pkt.pts = pkt.pts + offset;
        pkt.dts = pkt.dts + offset;

        if      (is_video && !is_video_over_trim) temp_video_pts = pkt.pts; 
        else if (is_audio && !is_audio_over_trim) temp_audio_pts = pkt.pts;


        current_duration = (av_q2d(out_stream->time_base) * pkt.pts) * 1000;
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        if (is_trim_end && (current_duration > trim_end_millisec)) {
            av_packet_unref(&pkt);
            if (is_audio_over_trim && is_video_over_trim) break; 
            if (is_video) is_video_over_trim = true;
            if (is_audio) is_audio_over_trim = true;
            continue;
        } 

        // Write packet
        if (av_interleaved_write_frame(out_format_context, &pkt) < 0) {
            std::cerr << "Error muxing packet\n";
            break;
        }
        av_packet_unref(&pkt);
    }
    *last_video_pts = temp_video_pts;
    *last_audio_pts = temp_audio_pts;
    return 0;
}
