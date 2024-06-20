#include "app.hpp"

void App::downloadClip(const std::string& url, const std::string& file_name) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    std::ofstream file(file_name, std::ios::binary);
    file.write(r.text.c_str(), r.text.size());
    file.close();
}

void App::downloadClips(const std::vector<std::string>& clip_urls) {
    std::cout << "Downloading " << clip_urls.size() << " clips..." << "\n";

    std::vector<std::thread> threads;
    for (int i = 0; i < clip_urls.size(); i++) {
        std::string output_file_name = PATH + std::to_string(i + 1) + BASE_FILE_NAME;
        threads.push_back(std::thread(&App::downloadClip, this, clip_urls[i], output_file_name));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void endProgram() {
    std::cout << "Deleting Clips..." << "\n";
    deleteAllFilesInFolder("clips/");
    std::cout << "Exiting program..." << "\n";
    exit(0);
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
        uploadVideo();
        return;
    case 3:
        settings.menu();
        break;
    case 4:
        endProgram();
        break;
    }
    menu();
}

void App::buildVideo() {
    checkAndCreateDirectory(PATH);
    checkAndCreateDirectory(PATH + RESULT_PATH);

    std::vector<VideoEditor> clips;
    for (const auto& entry : std::filesystem::directory_iterator(PATH)) {
        if (entry.is_regular_file()) {
            clips.push_back(VideoEditor(entry.path().string()));
        }
    }

    if (clips.empty()) {
        std::cout << "No clips found in the clips folder!" << "\n";
        return;
    }

    VideoEditor video = clips[0];
    clips.erase(clips.begin());
    video.appendVideos(clips, PATH + RESULT_PATH + RESULT_FILE_NAME);
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void App::uploadVideo() {
    std::string title = settings.readFromSettingsFile("video_title");
    std::string category_id = settings.readFromSettingsFile("video_category");
    std::string description = settings.readFromSettingsFile("video_description");
    std::string temp_tags = settings.readFromSettingsFile("video_tags");
    std::vector<std::string> tags;

    if (temp_tags.length() > 2) {
        temp_tags = temp_tags.erase(0, 1);
        temp_tags = temp_tags.erase(temp_tags.length() - 2, 1);
        tags = splitString(temp_tags, ',');
    } 

    std::cout << "Insert start of title\n>> ";
    std::string start_title;
    std::getline(std::cin, start_title);

    title = start_title + " " + title;

    std::cout << "Uploading video..." << "\n";
    settings.getYouTubeApi().uploadVideo(PATH + RESULT_PATH + RESULT_FILE_NAME, title, description, category_id, tags);
    std::cout << "Video uploaded successfully!" << "\n";
    endProgram();
}

void App::choseClips() {
    checkAndCreateDirectory(PATH);
    std::vector<std::string> clip_urls = settings.getClipsUrls();

    if (clip_urls.empty()) {
        std::cout << "No clips found for the specified time span!" << "\n";
        return;
    }
    downloadClips(clip_urls);

    for (int i = 0; i < clip_urls.size(); i++) {
        std::string output_file_name = PATH + std::to_string(i + 1) + BASE_FILE_NAME;
        char choice;
        do {
            clearConsole();
            std::cout << "Clip " << i + 1 << " \n1. for keeping\n2. for editing and keeping\n3. for deleting\n4. Accept remaining\n>> ";
            choice = queryInt();
        } while (choice != 1 && choice != 2 && choice != 3 && choice != 4); 
        
        if (choice == 1) continue;
        else if (choice == 2) editVideo(std::to_string(i + 1) + BASE_FILE_NAME);
        else if (choice == 3) std::remove((output_file_name).c_str());
        else if (choice == 4) break;
    }
}

void App::editVideo(const std::string& file_name) {
    bool cropping = false;
    VideoEditor temp_video = VideoEditor(PATH + file_name);
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
    if (cropping) temp_video.trimVideo(start_time, end_time);
}