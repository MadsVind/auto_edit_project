#include <youtube_api.hpp>


void YouTubeApi::initOAuthToken() {
    // Get the device code
    cpr::Response r = cpr::Post(cpr::Url{"https://oauth2.googleapis.com/device/code"},
                                cpr::Payload{{"client_id", getClientId()},
                                             {"scope", "https://www.googleapis.com/auth/youtube.upload"}});

    auto json = nlohmann::json::parse(r.text);
    if (!json.contains("device_code") || json["device_code"].is_null()) {
        throw std::runtime_error("Failed to get device code from Google OAuth 2.0 server.");
    }

    std::string device_code = json["device_code"];
    std::string user_code = json["user_code"];
    std::string verification_url = json["verification_url"];

    // Ask the user to go to the verification URL and enter the user code
    std::cout << "Please go to " << verification_url << " and enter the code " << user_code << std::endl;

    // Poll the server to get the access token
    while (true) {
        r = cpr::Post(cpr::Url{"https://oauth2.googleapis.com/token"},
                      cpr::Payload{{"client_id", getClientId()},
                                   {"client_secret", getClientSecret()},
                                   {"device_code", device_code},
                                   {"grant_type", "urn:ietf:params:oauth:grant-type:device_code"}});

        json = nlohmann::json::parse(r.text);
        if (json.contains("access_token") && !json["access_token"].is_null()) {
            this->access_token = json["access_token"];
            break;
        }

        // Wait before polling again
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void YouTubeApi::uploadVideo(const std::string& video_path, 
                             const std::string& title, 
                             const std::string& description, 
                             const std::string& category_id, 
                             const std::vector<std::string>& tags) {
    nlohmann::json body_json;
    body_json["snippet"]["categoryId"] = category_id;
    body_json["snippet"]["title"] = title;
    body_json["snippet"]["description"] = description;
    body_json["snippet"]["tags"] = tags;
    body_json["status"]["privacyStatus"] = "public";
    std::string body = body_json.dump();

    cpr::Response r1 = cpr::Post(cpr::Url{"https://www.googleapis.com/upload/youtube/v3/videos"},
                                 cpr::Header{{"Authorization", "Bearer " + getAccessToken()},
                                             {"Content-Type", "application/json"}},
                                 cpr::Parameters{{"uploadType", "resumable"},
                                                 {"part", "snippet,status"},
                                                 {"key", getClientId()}},
                                 cpr::Body{body});
                                 
    if (r1.status_code != 200) {
        throw std::runtime_error("Failed to upload video to YouTube API.");
    }
    std::string upload_url = r1.header["Location"];
    cpr::Response r2 = cpr::Put(cpr::Url{upload_url},
                                cpr::Header{{"Authorization", "Bearer " + getAccessToken()},
                                            {"Content-Type", "video/*"}},
                                cpr::Body{cpr::File{video_path}});
    if (r2.status_code != 200) {
        throw std::runtime_error("Failed to upload video to YouTube API.");
    }
}

bool YouTubeApi::isCredentialsValid(const std::string& client_id, const std::string& client_secret) {
    // Step 1: Get the device code
    cpr::Response r = cpr::Post(cpr::Url{"https://oauth2.googleapis.com/device/code"},
                                cpr::Payload{{"client_id", client_id},
                                             {"scope", "https://www.googleapis.com/auth/youtube.upload"}});

    auto json = nlohmann::json::parse(r.text);
    if (!json.contains("device_code") || json["device_code"].is_null()) {
        std::cerr << "Failed to get device code from Google OAuth 2.0 server.\n" << r.text << std::endl;
        return false;
    }

    std::string device_code = json["device_code"];
    std::string user_code = json["user_code"];
    std::string verification_url = json["verification_url"];

    // Step 2: Ask the user to go to the verification URL and enter the user code
    std::cout << "Please go to " << verification_url << " and enter the code " << user_code << std::endl;

    // Step 3: Poll the server to get the access token
    while (true) {
        r = cpr::Post(cpr::Url{"https://oauth2.googleapis.com/token"},
                      cpr::Payload{{"client_id", client_id},
                                   {"client_secret", client_secret},
                                   {"device_code", device_code},
                                   {"grant_type", "urn:ietf:params:oauth:grant-type:device_code"}});

        json = nlohmann::json::parse(r.text);
        if (json.contains("access_token") && !json["access_token"].is_null()) {
            access_token = json["access_token"];
            return true;
        }

        // Wait before polling again
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}