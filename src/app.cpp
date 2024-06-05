#include "app.hpp"

void App::downloadClip(const std::string& url, const std::string& file_name) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    std::ofstream file(file_name, std::ios::binary);
    file.write(r.text.c_str(), r.text.size());
    file.close();
}

void App::menu() {
    clearConsole();
    std::cout << "1. Chose Clips\n";
    std::cout << "2. Upload Video\n";
    std::cout << "3. Settings\n";
    std::cout << "4. Exit\n";

    std::cout << ">> ";
    int choice = queryInt();

    switch (choice) {
    case 1:
        choseClips();
        break;
    case 2:
        buildVideo();
        break;
    case 3:
        settings.menu();
        break;
    case 4:
        deleteAllFilesInFolder("clips/");
        exit(0);
        break;
    }
    menu();
}

void App::buildVideo() {
    const std::string path = "clips/";
    const std::string output_file_name = "output.mp4";
    checkAndCreateDirectory(path);

    std::vector<VideoEditor> clips;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::filesystem::path filePath = entry.path();
            std::string directory = filePath.parent_path().string() + "/";
            std::string fileName = filePath.filename().string();
            clips.push_back(VideoEditor(directory, fileName));
        }
    }

    if (clips.empty()) {
        std::cout << "No clips found in the clips folder!" << "\n";
        return;
    }

    VideoEditor video = clips[0];
    clips.erase(clips.begin());
    video.appendVideos(clips, path + output_file_name);
}

void App::choseClips() {
    const std::string base_file_name = "clip.mp4";
    const std::string path = "clips/";
    checkAndCreateDirectory(path);

    std::vector<std::string> clip_urls = settings.getClipsUrls();

    if (clip_urls.empty()) {
        std::cout << "No clips found for the specified time span!" << "\n";
        return;
    }
    std::cout << "Downloading " << clip_urls.size() << " clips..." << "\n";

    std::vector<std::thread> threads;
    for (int i = 0; i < clip_urls.size(); i++) {
        std::string output_file_name = path + std::to_string(i + 1) + base_file_name;
        threads.push_back(std::thread(&App::downloadClip, this, clip_urls[i], output_file_name));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (int i = 0; i < clip_urls.size(); i++) {
        std::string output_file_name = path + std::to_string(i + 1) + base_file_name;
        char choice;
        do {
            clearConsole();
            std::cout << "Use clip " << i + 1 << " (y for keeping/n for deleting/x for stopping \n>> ";
            std::cin >> choice;
            std::cin.ignore();
        } while (choice != 'y' && choice != 'n' && choice != 'x'); 
        
        if (choice == 'x') {
            std::remove((output_file_name).c_str());
            break;
        }
        if (choice == 'n') std::remove((output_file_name).c_str());
        if (choice == 'y') {
            bool cropping = false;
            VideoEditor temp_video = VideoEditor(path + std::to_string(i + 1), base_file_name);
            int start_time = 0;
            int end_time = temp_video.getVideoLength();
            
            std::cout << "Crop the clip, start time in milliseconds\n>> ";
            int temp = queryInt();
            if (temp != -1) {
                start_time = temp;
                cropping = true;
            }
            std::cout << "Crop the clip, end time in milliseconds\n>> ";
            temp = queryInt();
            if (temp != -1) {
                end_time = temp;
                cropping = true;
            }
            if (cropping) {
                temp_video.trimVideo(start_time, end_time);
            }
        }
    }
}
