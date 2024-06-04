#ifndef VIDEO_EDITOR_HPP
#define VIDEO_EDITOR_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <memory>
#include <filesystem>
#include <iomanip>

/**
 * @class VideoEditor
 * @brief Represents a video editor for editing video files.
 */
/**
 * @class VideoEditor
 * @brief Represents a video editor that can trim and append videos.
 */
class VideoEditor {
private:
    std::string filePath; /**< The path to the video file. */
    std::string fileName; /**< The name of the video file. */

public:
    /**
     * @brief Constructs a VideoEditor object with the specified file name.
     * @param fileName The name of the video file.
     */
    VideoEditor(const std::string& filePath, const std::string& fileName) : filePath(filePath), fileName(fileName) {}

    std::string getFileName() const {return fileName;}
    std::string getFilePath() const {return filePath;}


    /**
     * @brief Get the length of the video.
     * @return The length of the video in milliseconds.
     */
    double getVideoLength() const;

    /**
     * @brief Trims the video from the specified start second to the specified end second.
     * @param startSecond The start second of the trimmed video.
     * @param endSecond The end second of the trimmed video.
     */
    void trimVideo(double startMillisecond, double endMillisecond) const;

    /**
     * @brief Appends multiple videos to the current video.
     * @param videos The vector of VideoEditor objects to be appended.
     * @param output_path The output path of the appended video.
     */
    void appendVideos(const std::vector<VideoEditor>& videos, std::string outputPath) const;

    /**
     * @brief Appends a single video to the current video.
     * @param video The VideoEditor object to be appended.
     * @param output_path The output path of the appended video.
     */
    void appendVideos(const VideoEditor& video, std::string outputPath) const;
};

#endif // VIDEO_EDITOR_HPP