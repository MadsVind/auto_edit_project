
#define VIDEO_EDITOR_MAIN


#include "test_video_editor.hpp"

std::filesystem::path currentFile(__FILE__);
std::filesystem::path currentDir = currentFile.parent_path();
std::filesystem::path video_path_1 = currentDir / "test_videos" / "example1.mp4" ;
std::filesystem::path video_path_2 = currentDir / "test_videos" / "example2.mp4" ;
std::filesystem::path video_path_non = currentDir / "test_videos" / "nonexample.mp4" ;
std::filesystem::path output_path = currentDir / "test_videos" / "output.mp4";


TEST_CASE("VideoEditor trims and appends videos correctly", "[VideoEditor]") {
    VideoEditor video1(video_path_1);
    VideoEditor video2(video_path_2);

    std::cout << "test 1 \n";
    video1.trimVideo(0000, 2000);

    std::cout << "test 2 \n";
    video1.appendVideos(video2, output_path.string());

    std::cout << "test 3 \n";

    std::ifstream outputFile(output_path);
    REQUIRE(outputFile.is_open());

    outputFile.close();

    std::filesystem::remove(output_path);
}

TEST_CASE("VideoEditor throws an error when trying to trim a video with invalid start or end second", "[VideoEditor]") {
    VideoEditor video(video_path_1);

    REQUIRE_THROWS(video.trimVideo(-1000, 2000));
    REQUIRE_THROWS(video.trimVideo(2000, 1000));
    REQUIRE_THROWS(video.trimVideo(0, 10000000));
}

TEST_CASE("VideoEditor throws an error when trying to append a video that does not exist", "[VideoEditor]") {
    VideoEditor video1(video_path_2);
    VideoEditor video2(video_path_non);

    REQUIRE_THROWS(video1.appendVideos(video2, output_path.string()));
}