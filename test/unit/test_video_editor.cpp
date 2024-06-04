
#define VIDEO_EDITOR_MAIN

#include "test_video_editor.hpp"

std::filesystem::path currentFile(__FILE__);
std::filesystem::path currentDir = currentFile.parent_path();
std::filesystem::path videoPath = currentDir / "test_videos/" ;
std::filesystem::path outputPath = currentDir / "test_videos" / "output.mp4";


TEST_CASE("VideoEditor trims and appends videos correctly", "[VideoEditor]") {
    VideoEditor video1(videoPath.string(), "example1.mp4");
    VideoEditor video2(videoPath.string(), "example2.mp4");

    video1.trimVideo(0000, 2000);

    video1.appendVideos(video2, outputPath.string());

    std::ifstream outputFile(outputPath);
    REQUIRE(outputFile.is_open());

    outputFile.close();

    std::filesystem::remove(outputPath);
}

TEST_CASE("VideoEditor throws an error when trying to trim a video with invalid start or end second", "[VideoEditor]") {
    VideoEditor video(videoPath.string(), "example1.mp4");

    REQUIRE_THROWS(video.trimVideo(-1000, 2000));
    REQUIRE_THROWS(video.trimVideo(2000, 1000));
    REQUIRE_THROWS(video.trimVideo(0, 10000000));
}

TEST_CASE("VideoEditor throws an error when trying to append a video that does not exist", "[VideoEditor]") {
    VideoEditor video1(videoPath.string(), "example1.mp4");
    VideoEditor video2(videoPath.string(), "nonexistent.mp4");

    REQUIRE_THROWS(video1.appendVideos(video2, outputPath.string()));
}