#include "settings.hpp"

void Settings::initSettings() {
    std::string encrypted_id = readFromSettingsFile("twitch_client_id");
    std::string encrypted_secret = readFromSettingsFile("twitch_client_secret");

    twitch_con = TwitchApi(ce.bitOrDecrypt(encrypted_id, encryption_key), 
                           ce.bitOrDecrypt(encrypted_secret, encryption_key));

    encrypted_id = readFromSettingsFile("youtube_client_id");
    encrypted_secret = readFromSettingsFile("youtube_client_secret");
    youtube_con = YouTubeApi(ce.bitOrDecrypt(encrypted_id, encryption_key), 
                           ce.bitOrDecrypt(encrypted_secret, encryption_key));

    game_id = readFromSettingsFile("game_id");
    if (game_id == "") game_id = "32399";
    std::string clip_amount_str = readFromSettingsFile("clip_amount");
    clip_amount = (clip_amount_str == "") ? 10 : std::stoi(clip_amount_str);

    std::string time_span_hours_str = readFromSettingsFile("time_span_hours");
    time_span_hours = (time_span_hours_str == "") ? 7 * 24 : std::stoi(time_span_hours_str);
}

void Settings::menu() {
    clearConsole();
    std::cout << "1. Set Game\n";
    std::cout << "2. Set Clip Amount\n";
    std::cout << "3. Set Clip Time Span\n";
    std::cout << "4. Set Twitch Credentials\n";
    std::cout << "5. Set Youtube Credentials\n";
    std::cout << "6. Back To Menu\n";

    std::cout << ">> ";
    int choice = queryInt();

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
        queryCredentials("twitch", &twitch_con);
        break;
    case 5:
        queryCredentials("youtube", &youtube_con);
        break;
    case 6:
        return;
        break;
    }
    menu();
}

void Settings::queryGame() {
    std::map<std::string, std::string> top_games = twitch_con.getTopGames(10);

    int i = 0;
    for (const auto& game : top_games) {
        std::cout << i << ": " << game.first << "\n";
        i++;
    }
    std::cout << "Enter the index of the game you want to download clips from\n>> ";
    int game_index = queryInt();

    std::map<std::string, std::string>::iterator game_it = top_games.begin();
    std::advance(game_it, game_index);
    game_id = game_it->second;
    writeToSettingsFile("game_id", game_id);
}

void Settings::queryClipAmount() {
    int temp_clip_amount;
    std::cout << "Enter the number of clips you want to download.\n>> ";
    clip_amount = queryInt();
    writeToSettingsFile("clip_amount", std::to_string(clip_amount));
}

void Settings::queryClipTimeSpan() {
    int days;
    std::cout << "Enter the number of days you want to download clips from.\n>> ";
    days = queryInt();
    time_span_hours = days * HOURS_IN_DAY;
    writeToSettingsFile("time_span_hours", std::to_string(time_span_hours));
}

void Settings::queryCredentials(const std::string& service, Api* api) { // is not generic, should be changed to not for only twitch
    std::string client_id;
    std::string client_secret;
    bool connection_successful = false;

    do {
        std::cout << "Enter your " << service << " client ID (x for cancel).\n>> ";
        std::cin >> client_id;
        if (client_id == "x") return;

        std::cout << "Enter your " << service << " client secret (x for cancel).\n>> ";
        std::cin >> client_secret;
        if (client_secret == "x") return;

        connection_successful = api->isCredentialsValid(client_id, client_secret);

    } while (!connection_successful);
    api->setCredentials(client_id, client_secret);

    client_id = ce.bitOrEncrypt(client_id, encryption_key);
    client_secret = ce.bitOrEncrypt(client_secret, encryption_key);

    writeToSettingsFile(service + "_client_id", client_id);
    writeToSettingsFile(service + "_client_secret", client_secret);
}

void Settings::writeToSettingsFile(const std::string& key, const std::string& value) {
    nlohmann::json j;
    
    std::ifstream read_file(settings_file_name);
    if (read_file.is_open()) {
        read_file >> j;
        read_file.close();
    } else {
        std::cerr << "Unable to open file for reading: " << settings_file_name << std::endl;
    }

    j[key] = value;

    std::ofstream write_file(settings_file_name);
    if (write_file.is_open()) {
        write_file << j;
        write_file.close();
    } else {
        std::cerr << "Unable to open file for writing: " << settings_file_name << std::endl;
    }
}

std::string Settings::readFromSettingsFile(const std::string& key) {
    nlohmann::json j;
    std::string value;
    std::ifstream file(settings_file_name);
    if (file) {
        file >> j;
        file.close();
    }

    try {
        value = j[key];
    } catch(const std::exception& e) {
        return "";
    }

    file.close();
    return value;
}