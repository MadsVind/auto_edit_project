#include "video_editor.hpp"

double VideoEditor::getVideoLength() const {
    
}

void VideoEditor::trimVideo(double startMillisecond, double endMillisecond) const {
    
}

void VideoEditor::appendVideos(const std::vector<VideoEditor>& videos, std::string outputPath) const {

}

void VideoEditor::appendVideos(const VideoEditor& video, std::string outputPath) const {
    std::vector<VideoEditor> videos = {video};
    appendVideos(videos, outputPath); // Call the method that takes a vector
}