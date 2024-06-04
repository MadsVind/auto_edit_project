#include "app.hpp"

const int HOURS_IN_DAY = 24;

void clearConsole() {
    try {
        system("clear");
    } catch (const std::exception& e) {
        system("cls");
    }
}

bool checkAndCreateDirectory(const std::string& dir) {
    struct stat info;

    if (stat(dir.c_str(), &info) != 0) {
        if (mkdir(dir.c_str(), 0777) == -1) {
            std::cerr << "Error :  " << strerror(errno) << std::endl;
            return false;
        }
    } else if (!(info.st_mode & S_IFDIR)) {
        std::cerr << dir << " is not a directory" << std::endl;
        return false;
    }

    return true;
}

int queryInt() {
    std::string input;
    std::getline(std::cin, input);
    std::cin.ignore();
    if (input.empty()) {
        return -1;
    } else {
        try {
            int integer = std::stoi(input);
            return integer;
        } catch (std::invalid_argument&) {
            std::cout << "Invalid input. Please enter an integer.\n>> ";
            return queryInt();
        }
    }
}

void deleteAllFilesInFolder(const std::string& folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::filesystem::remove(entry);
        }
    }
}

void App::menu() {
    clearConsole();
    std::cout << "1. Set Game\n";
    std::cout << "2. Set Clip Amount\n";
    std::cout << "3. Set Clip Time Span\n";
    std::cout << "4. Chose Clips\n";
    std::cout << "5. Exit\n";

    int choice;
    std::cout << ">> ";
    std::cin >> choice;

    switch (choice) {
    case 1:
        queryGame();
        break;
    case 2:
        queryClipAmount();
        break;
    case 3:
        queryClipTimeSpan();
        break;
    case 4:
        choseClips();
        break;
    case 5:
        deleteAllFilesInFolder("clips/");
        exit(0);
        break;
    }
    menu();
}

void App::downloadClip(const std::string& url, const std::string& file_name) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    std::ofstream file(file_name, std::ios::binary);
    file.write(r.text.c_str(), r.text.size());
    file.close();
}

void App::queryGame() {
    std::map<std::string, std::string> top_games = twitch_con.getTopGames(10);

    int i = 0;
    for (const auto& game : top_games) {
        std::cout << i << ": " << game.first << "\n";
        i++;
    }
    std::cout << "Enter the index of the game you want to download clips from\n>> ";
    int game_index;
    std::cin >> game_index;

    std::map<std::string, std::string>::iterator game_it = top_games.begin();
    std::advance(game_it, game_index);
    setGameID(game_it->second);
}

void App::queryClipAmount() {
    int clips;
    std::cout << "Enter the number of clips you want to download.\n>> ";
    std::cin >> clips;
    setClipAmount(clips);
}

void App::queryClipTimeSpan() {
    int days;
    std::cout << "Enter the number of days you want to download clips from.\n>> ";
    std::cin >> days;
    setClipTimeSpan(days);
}

void App::choseClips() {
    const std::string base_file_name = "clip.mp4";
    const std::string path = "clips/";
    checkAndCreateDirectory(path);

    std::vector<std::string> clip_urls = twitch_con.getTopClipsInTimeSpan(game_id, getClipAmount() * HOURS_IN_DAY, getClipAmount());

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

