//#include "main.hpp"
//
//int main() {
//    App app;
//    app.menu();
//    return 0;
//}

#include <iostream>
#include "video.hpp"

int main() {
    Video video1("/home/vind/auto_edit_project/test/unit/test_videos/example.mp4");
    Video video2("/home/vind/auto_edit_project/test/unit/test_videos/example2.mp4");
    Video video3("/home/vind/auto_edit_project/test/unit/test_videos/example1.mp4");

    std::vector<Video> videos;
    videos.push_back(video2);
    videos.push_back(video3);

    video1.trimVideo(1000, 000, "/home/vind/auto_edit_project/test/unit/test_videos/temp_example1.mp4");
    //video1.appendVideos(videos, "/home/vind/auto_edit_project/test/unit/test_videos/temp_example2.mp4");
    return 0;
}

