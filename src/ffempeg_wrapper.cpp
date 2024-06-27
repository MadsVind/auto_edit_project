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

// !!! Does not yet trim start !!!
int addPacketsToOutput(AVFormatContext* inFmtCtx, AVFormatContext* outFmtCtx, 
                       uint64_t* last_video_pts, uint64_t* last_audio_pts,
                       int64_t trim_start_millisec, int64_t trim_end_millisec) {

    AVPacket pkt;
    uint64_t temp_video_pts = 0;
    uint64_t temp_audio_pts = 0;

    bool is_video_over_trim = false;
    bool is_audio_over_trim = false;

    while (av_read_frame(inFmtCtx, &pkt) >= 0) {
        // Copy packet
        AVStream *in_stream, *out_stream;
        in_stream = inFmtCtx->streams[pkt.stream_index];
        out_stream = outFmtCtx->streams[pkt.stream_index];
        const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(out_stream->codecpar->codec_id);

        uint64_t current_duration = (av_q2d(out_stream->time_base) * pkt.pts) * 1000;
        if ((current_duration < trim_start_millisec && trim_start_millisec > 0)) {
            av_packet_unref(&pkt);
            if (codec_desc->type == AVMEDIA_TYPE_VIDEO) *last_video_pts = pkt.pts;
            if (codec_desc->type == AVMEDIA_TYPE_AUDIO) *last_audio_pts = pkt.pts;
            std::cout << "pts: " << pkt.pts  << "  last video pts: " << *last_video_pts << "  last audio pts: " << *last_audio_pts << std::endl;
            continue;
        }

        if (last_video_pts != nullptr && codec_desc->type == AVMEDIA_TYPE_VIDEO) {
            pkt.pts = av_rescale_q_rnd(pkt.pts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF) + *last_video_pts;
            pkt.dts = av_rescale_q_rnd(pkt.dts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF) + *last_video_pts;
            if (!is_video_over_trim) temp_video_pts = pkt.pts; // Update after rescaling and offset adjustment
        }
        else if (last_audio_pts != nullptr && codec_desc->type == AVMEDIA_TYPE_AUDIO) {
            pkt.pts = av_rescale_q_rnd(pkt.pts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF) + *last_audio_pts;
            pkt.dts = av_rescale_q_rnd(pkt.dts + 1, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF) + *last_audio_pts;
            if (!is_audio_over_trim) temp_audio_pts = pkt.pts; // Update after rescaling and offset adjustment
        }
        current_duration = (av_q2d(out_stream->time_base) * pkt.pts) * 1000;
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        std::cout << "current_duration: " << current_duration << " trim_end " << trim_end_millisec << " trim_start " << trim_start_millisec << std::endl;
        if ((current_duration > trim_end_millisec && trim_end_millisec > 0)) {
            av_packet_unref(&pkt);
            if (is_audio_over_trim && is_video_over_trim) break; 
            if (codec_desc->type == AVMEDIA_TYPE_VIDEO) is_video_over_trim = true;
            if (codec_desc->type == AVMEDIA_TYPE_AUDIO) is_audio_over_trim = true;
            continue;
        } 

        std::cout << "pts: " << pkt.pts  << "  Adding packet to stream index: " << pkt.stream_index;
        if (codec_desc) std::cout << ", Codec: " << codec_desc->name;
        std::cout << std::endl;
        // Write packet
        if (av_interleaved_write_frame(outFmtCtx, &pkt) < 0) {
            std::cerr << "Error muxing packet\n";
            break;
        }
        av_packet_unref(&pkt);
    }
    *last_video_pts = temp_video_pts;
    *last_audio_pts = temp_audio_pts;
    std::cout << "Finished adding packets to output: temp video:audio  " << temp_video_pts << "  :  " << temp_audio_pts << "\n" ;
    return 0;
}

