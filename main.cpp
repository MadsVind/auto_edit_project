#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <credential_encryption.hpp>
#include <twitch_api.hpp>

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

    CredentialEncryption ce;
    const std::string client_id     = ce.readDecryptedFromFile("client_id", encryption_key , filename);
    const std::string client_secret = ce.readDecryptedFromFile("client_secret", encryption_key, filename);

    TwitchApi ta;
    std::string access_token = ta.getOAuthToken(client_id, client_secret);
    std::string clip_download_url = ta.getClipDownloadUrl(access_token, client_id, clip_id);
    
    // Step 3: Download the Clip
    downloadClip(clip_download_url, output_file_name );

    std::cout << "Clip downloaded successfully!" << std::endl;
    return 0;
}