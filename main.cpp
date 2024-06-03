#include "main.hpp"

// Function to download the clip
void downloadClip(const std::string& url, const std::string& file_name) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    std::ofstream file(file_name, std::ios::binary);
    file.write(r.text.c_str(), r.text.size());
    file.close();
}

int main() {
    const std::string clip_id = "ProtectiveDeadVultureAliens-r5smm8hR4m60iJmX";
    const std::string encryption_key = "simple_key";
    const std::string filename = "../credentials.txt";
    const std::string output_file_name = "twitch_clip.mp4";
    const int hours = 24*7;

    CredentialEncryption ce;
    const std::string client_id     = ce.readDecryptedFromFile("client_id", encryption_key , filename);
    const std::string client_secret = ce.readDecryptedFromFile("client_secret", encryption_key, filename);

    TwitchApi ta;
    std::string access_token = ta.getOAuthToken(client_id, client_secret);
    std::map<std::string, std::string> top_games = ta.getTopGames(access_token, client_id, 10);

    int i = 0;
    for (const auto& game : top_games) {
        std::cout << i << ": " << game.first << "\n";
        i++;
    }
    std::cout << "Enter the index of the game you want to download clips from: ";
    int game_index;
    std::cin >> game_index;

    std::map<std::string, std::string>::iterator game_it = top_games.begin();
    std::advance(game_it, game_index);

    std::vector<std::string> clip_urls = ta.getTopClipsInTimeSpan(access_token, client_id,  game_it->second, hours);

    downloadClip(clip_urls[0], output_file_name );

    std::cout << "Clip downloaded successfully!" << "\n";
    return 0;
}
