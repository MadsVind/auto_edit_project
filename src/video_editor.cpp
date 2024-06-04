#include "video_editor.hpp"


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void VideoEditor::trimVideo(int startSecond, int endSecond) const {
    // Get the duration of the video
    std::string command = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 " + filePath + fileName;
    std::string durationStr = exec(command.c_str());
    double duration = std::stod(durationStr);

    // Check if the start and end seconds are within the duration of the video
    if (startSecond < 0 || endSecond > duration || startSecond >= endSecond) {
        throw std::invalid_argument("Invalid start or end second");
    }

    std::string tempFileName = filePath + "temp_" + fileName;
    command = "ffmpeg -i " + filePath + fileName + " -ss " + std::to_string(startSecond) + " -to " + std::to_string(endSecond) + " -c copy " + tempFileName + " -y";
    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to trim video");
    }

    // Replace the original file with the new one
    command = "mv " + tempFileName + " " + filePath + fileName;
    result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to replace the original video file");
    }
}

void VideoEditor::appendVideos(const std::vector<VideoEditor>& videos, std::string outputPath) const {
    bool isSameFile = false;
    if (videos.empty()) return;
    if (outputPath == filePath + fileName) {
        outputPath = filePath + "temp_" + fileName;
        isSameFile = true;
    }
    
    std::ofstream listFile("list.txt");
    listFile << "file '" << filePath + fileName << "'\n";
    for (const auto& video : videos) {
        std::string videoPath = video.getFilePath() + video.getFileName();
        if (!std::filesystem::exists(videoPath)) {
            throw std::runtime_error("Video file does not exist: " + videoPath);
        }
        listFile << "file '" << videoPath << "'\n";
    }
    listFile.close();

    std::string command = "ffmpeg -f concat -safe 0 -i list.txt -c copy " + outputPath + " -y";
    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to append videos");
    }
    remove("list.txt");

    if (!isSameFile) return;
    command = "mv " + outputPath + " " + filePath + fileName;
    result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to replace the original video file");
    }
}

void VideoEditor::appendVideos(const VideoEditor& video, std::string outputPath) const {
    std::vector<VideoEditor> videos = {video};
    appendVideos(videos, outputPath); // Call the method that takes a vector
}