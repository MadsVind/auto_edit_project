#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <ffempeg_wrapper.hpp>

/**
 * @brief A class that represents a video file.
 */
class Video {
private:
    std::string file_path; /**< The name of the video file. */

public:
    /**
     * @brief Constructs a Video object with the specified file name.
     * @param fileName The name of the video file.
     */
    Video(const std::string& file_path) : file_path(file_path) {}

    std::string getFilePath() const {return file_path;}

    int64_t getDuration() const;

    /**
     * @brief Trims the video from the specified start second to the specified end second.
     * @param startSecond The start second of the trimmed video.
     * @param endSecond The end second of the trimmed video.
     */
    void trimVideo(const double start_millisecond, const double end_millisecond, const std::string& output_path);

    /**
     * @brief Appends multiple videos to the current video.
     * ! Does not work for incompatible video formats. (File type, codec, scale, etc.) 
     * @param videos The vector of VideoEditor objects to be appended.
     * @param output_path The output path of the appended video.
     */
    void appendVideos(const std::vector<Video>& videos, const std::string& output_path);

    /**
     * @brief Appends a single video to the current video.
     * !Does not work for incompatible video formats. (File type, codec, scale, etc.)
     * @param video The VideoEditor object to be appended.
     * @param output_path The output path of the appended video.
     */
    void appendVideos(const Video& video, const std::string& output_path);
  
};

#endif // VIDEO_HPP