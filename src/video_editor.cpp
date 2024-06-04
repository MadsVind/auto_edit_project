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

std::string formatTime(double milliseconds) {
    int hours = static_cast<int>(milliseconds) / 3600000;
    milliseconds -= hours * 3600000;
    int minutes = static_cast<int>(milliseconds) / 60000;
    milliseconds -= minutes * 60000;
    double seconds = milliseconds / 1000;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::fixed << std::setprecision(3) << seconds;

    return oss.str();
}

double VideoEditor::getVideoLength() const {
    std::string command = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 " + filePath + fileName;
    std::string durationStr = exec(command.c_str());
    double duration = std::stod(durationStr); // Duration in seconds
    return duration * 1000; // Convert to milliseconds
}

void VideoEditor::trimVideo(double startMillisecond, double endMillisecond) const {
    double duration = getVideoLength(); 

    if (startMillisecond < 0 || endMillisecond > duration || startMillisecond >= endMillisecond) {
        throw std::invalid_argument("Invalid start or end millisecond");
    }
    std::string startStr = formatTime(startMillisecond);
    std::string endStr = formatTime(endMillisecond);

    std::string tempFileName = filePath + "temp_" + fileName;
    std::string command = "ffmpeg -i " + filePath + fileName + " -ss " + startStr + " -to " + endStr + " -c copy " + tempFileName + " -y";
    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to trim video");
    }
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