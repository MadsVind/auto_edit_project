
#define VIDEO_EDITOR_MAIN


#include "test_video_editor.hpp"

std::filesystem::path currentFile(__FILE__);
std::filesystem::path currentDir = currentFile.parent_path();
std::filesystem::path video_path_1 = currentDir / "test_videos" / "example1.mp4" ;
std::filesystem::path video_path_2 = currentDir / "test_videos" / "example2.mp4" ;
std::filesystem::path video_path_non = currentDir / "test_videos" / "nonexample.mp4" ;
std::filesystem::path output_path = currentDir / "test_videos" / "output.mp4";

// !!! Test appending the same video object upon itself And multiple appends
// !!! Test appending videos with different codecs
// !!! Test appending videos with different resolutions
// !!! Test appending videos with different frame rates
TEST_CASE("Video trims and appends videos correctly", "[Video]") {
    Video video1(video_path_1);
    Video video2(video_path_2);

    std::cout << "test 1 \n";
    video1.trimVideo(0000, 2000, video_path_1);

    std::cout << "test 2 \n";
    video1.appendVideos(video2, output_path.string());

    std::cout << "test 3 \n";

    std::ifstream outputFile(output_path);
    REQUIRE(outputFile.is_open());

    outputFile.close();

    std::filesystem::remove(output_path);
}

TEST_CASE("VideoEditor throws an error when trying to trim a video with invalid start or end second", "[VideoEditor]") {
    Video video(video_path_1);

    REQUIRE_THROWS(video.trimVideo(-1000, 2000, video_path_1));
    REQUIRE_THROWS(video.trimVideo(2000, 1000, video_path_1));
    REQUIRE_THROWS(video.trimVideo(0, 10000000, video_path_1));
}

TEST_CASE("VideoEditor throws an error when trying to append a video that does not exist", "[VideoEditor]") {
    Video video1(video_path_2);
    Video video2(video_path_non);

    REQUIRE_THROWS(video1.appendVideos(video2, output_path.string()));
}