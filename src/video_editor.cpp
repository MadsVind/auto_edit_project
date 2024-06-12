#include "video_editor.hpp"

double VideoEditor::getVideoLength() const {
    AVFormatContext *fmt_ctx = NULL;
    int ret;
    if ((ret = avformat_open_input(&fmt_ctx, (filePath + fileName).c_str(), NULL, NULL)) < 0) {
        throw std::runtime_error("Could not open source file");
    }
    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        throw std::runtime_error("Could not find stream information");
    }
    double duration = fmt_ctx->duration / AV_TIME_BASE;
    avformat_close_input(&fmt_ctx);
    return duration * 1000; // Convert to milliseconds
}

void VideoEditor::trimVideo(double startMillisecond, double endMillisecond) const {
    double duration = getVideoLength(); 

    if (startMillisecond < 0 || endMillisecond > duration || startMillisecond >= endMillisecond) {
        throw std::invalid_argument("Invalid start or end millisecond");
    }

    double startSecond = startMillisecond / 1000.0;
    double endSecond = endMillisecond / 1000.0;

    char filter_descr[512];
    snprintf(filter_descr, sizeof(filter_descr), "trim=start=%f:end=%f,asetpts=PTS-STARTPTS", startSecond, endSecond);

    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket packet;
    const char *in_filename, *out_filename;
    int ret;
    int *stream_mapping = NULL;
    int stream_mapping_size = 0;

    in_filename  = (filePath + fileName).c_str();
    out_filename = (filePath + "temp_" + fileName).c_str();

    avformat_open_input(&ifmt_ctx, in_filename, 0, 0);
    avformat_find_stream_info(ifmt_ctx, 0);
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = (int *)av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping));

    while (true) {
        AVStream *in_stream, *out_stream;

        ret = av_read_frame(ifmt_ctx, &packet);
        if (ret < 0)
            break;

        in_stream  = ifmt_ctx->streams[packet.stream_index];
        out_stream = ofmt_ctx->streams[stream_mapping[packet.stream_index]];

        packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        packet.duration = av_rescale_q(packet.duration, in_stream->time_base, out_stream->time_base);
        packet.pos = -1;

        av_interleaved_write_frame(ofmt_ctx, &packet);
        av_packet_unref(&packet);
    }

    av_write_trailer(ofmt_ctx);

    avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    rename(out_filename, in_filename);
}

void VideoEditor::appendVideos(const std::vector<VideoEditor>& videos, std::string outputPath) const {
    bool isSameFile = false;
    if (videos.empty()) return;
    if (outputPath == filePath + fileName) {
        outputPath = filePath + "temp_" + fileName;
        isSameFile = true;
    }

    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket packet;
    const char *out_filename;
    int ret;
    int *stream_mapping = NULL;
    int stream_mapping_size = 0;

    out_filename = outputPath.c_str();

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);

    for (const auto& video : videos) {
        std::string videoPath = video.getFilePath() + video.getFileName();
        if (!std::filesystem::exists(videoPath)) {
            throw std::runtime_error("Video file does not exist: " + videoPath);
        }

        avformat_open_input(&ifmt_ctx, videoPath.c_str(), 0, 0);
        avformat_find_stream_info(ifmt_ctx, 0);

        stream_mapping_size = ifmt_ctx->nb_streams;
        stream_mapping = (int *)av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping));

        while (true) {
            AVStream *in_stream, *out_stream;

            ret = av_read_frame(ifmt_ctx, &packet);
            if (ret < 0) break;

            in_stream  = ifmt_ctx->streams[packet.stream_index];
            out_stream = ofmt_ctx->streams[stream_mapping[packet.stream_index]];

            packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            packet.duration = av_rescale_q(packet.duration, in_stream->time_base, out_stream->time_base);
            packet.pos = -1;

            av_interleaved_write_frame(ofmt_ctx, &packet);
            av_packet_unref(&packet);
        }

        avformat_close_input(&ifmt_ctx);
        av_freep(&stream_mapping);
    }

    av_write_trailer(ofmt_ctx);

    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    if (!isSameFile) return;
    rename(outputPath.c_str(), (filePath + fileName).c_str());
}

void VideoEditor::appendVideos(const VideoEditor& video, std::string outputPath) const {
    std::vector<VideoEditor> videos = {video};
    appendVideos(videos, outputPath); // Call the method that takes a vector
}