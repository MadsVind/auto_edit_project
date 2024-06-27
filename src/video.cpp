#include "video.hpp"

std::string getPathToFile(std::string file_path)  {
    return file_path.substr(0, file_path.find_last_of("/"));
}

std::string getFileFormat(std::string file_path)  {
    size_t pos = file_path.find_last_of(".");
    return file_path.substr(pos, file_path.length() - pos);
}

int64_t Video::getDuration() const {
    AVFormatContext* video_format_ctx = nullptr;
    initInputContext(file_path, &video_format_ctx);
    if (video_format_ctx->duration != AV_NOPTS_VALUE) {
        return video_format_ctx->duration / 1000;
    } else {
        throw std::runtime_error("Duration: Unknown");
    }
    avformat_close_input(&video_format_ctx);
}

// !!! Does not yet trim start !!!
void Video::trimVideo(const double start_millisecond, const double end_millisecond, const std::string& output_path) {
    AVFormatContext* out_format_ctx = nullptr;
    AVFormatContext* video_format_ctx = nullptr;

    std::string temp_output_path = (output_path == file_path) ? getPathToFile(output_path) + "/temp" + getFileFormat(output_path) 
                                                              : output_path; 

    
    initInputContext(file_path, &video_format_ctx);
    initOutputContext(temp_output_path, &out_format_ctx);
    copyStreamParameters(video_format_ctx, out_format_ctx);
    initOutputFile(temp_output_path, out_format_ctx);
    
    uint64_t last_video_pts = 0;
    uint64_t last_audio_pts = 0;
    addPacketsToOutput(video_format_ctx, out_format_ctx, &last_video_pts, &last_audio_pts, start_millisecond, end_millisecond);
    av_write_trailer(out_format_ctx);
    // Cleanup
    if (!(out_format_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&out_format_ctx->pb);
    avformat_free_context(out_format_ctx);
    avformat_close_input(&video_format_ctx);
}

//!!! Does not replace the original video file if == output_path
void Video::appendVideos(const std::vector<Video>& videos, const std::string& output_path) {
    AVFormatContext* out_format_ctx = nullptr;
    AVFormatContext* temp_format_ctx = nullptr;
    std::vector<AVFormatContext*> format_ctx_list;

    initInputContext(file_path, &temp_format_ctx);
    format_ctx_list.push_back(temp_format_ctx);
    for (const Video& video : videos) {
        temp_format_ctx = nullptr;
        initInputContext(video.file_path, &temp_format_ctx);
        format_ctx_list.push_back(temp_format_ctx);
    }

    std::string temp_output_path = (output_path == file_path) ? getPathToFile(output_path) + "/temp" + getFileFormat(output_path) 
                                                              : output_path; ;
    
    int video_stream_index = -1;
    int audio_stream_index = -1;
    initOutputContext(temp_output_path, &out_format_ctx);
    copyStreamParameters(format_ctx_list[0], out_format_ctx, &video_stream_index, &audio_stream_index);
    initOutputFile(temp_output_path, out_format_ctx);
    
    uint64_t last_video_pts = 0;
    uint64_t last_audio_pts = 0;
    addPacketsToOutput(format_ctx_list[0], out_format_ctx, 
                       &last_video_pts, &last_audio_pts); 


    for (int i = 1; i < format_ctx_list.size(); i++) {
        addPacketsToOutput(format_ctx_list[i], out_format_ctx, 
                           &last_video_pts, &last_audio_pts);
    }

    av_write_trailer(out_format_ctx);

    // Cleanup
    if (!(out_format_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&out_format_ctx->pb);
    avformat_free_context(out_format_ctx);

    for (AVFormatContext* video_format_ctx : format_ctx_list) {
        avformat_close_input(&video_format_ctx);
    }
}

void Video::appendVideos(const Video& video, const std::string& output_path) {
    std::vector<Video> videos;
    videos.push_back(video);
    appendVideos(videos, output_path); // Call the method that takes a vector
}

