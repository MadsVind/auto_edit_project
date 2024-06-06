#include <youtube_api.hpp>


void YouTubeApi::initOAuthToken() {
    std::string redirect_uri = "urn:ietf:wg:oauth:2.0:oob";
    // Step 1: Get the device code
    std::string auth_url = "https://accounts.google.com/o/oauth2/v2/auth?"
                           "client_id=" + client_id +
                           "&response_type=code"
                           "&scope=https://www.googleapis.com/auth/youtube.upload"
                           "&redirect_uri=" + redirect_uri;


    // Step 2: Ask the user to go to the verification URL and enter the user code
    std::cout << "Go to the following URL in your browser:\n" << auth_url << std::endl;

    // Step 2: User grants access and is redirected back to the application with an authorization code
    std::string auth_code;
    std::cout << "Enter the authorization code: ";
    std::cin >> auth_code;

    // Step 3: Exchange the authorization code for an access token
    cpr::Response r = cpr::Post(cpr::Url{"https://oauth2.googleapis.com/token"},
                                cpr::Payload{{"client_id", this->client_id},
                                             {"client_secret", this->client_secret},
                                             {"code", auth_code},
                                             {"grant_type", "authorization_code"},
                                             {"redirect_uri", redirect_uri}});

    auto json = nlohmann::json::parse(r.text);
    if (json.contains("access_token") && !json["access_token"].is_null()) {
        this->access_token = json["access_token"];
    } else {
        this->access_token = "-1";
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

    std::cout << getAccessToken() << std::endl;

    cpr::Response r1 = cpr::Post(cpr::Url{"https://www.googleapis.com/upload/youtube/v3/videos"},
                                 cpr::Header{{"Authorization", "Bearer " + getAccessToken()},
                                             {"Content-Type", "application/json"}},
                                 cpr::Parameters{{"uploadType", "resumable"},
                                                 {"part", "snippet,status"},
                                                 {"key", getClientId()}},
                                 cpr::Body{body});
                                 
    if (r1.status_code != 200) {
        throw std::runtime_error("Failed to upload video to YouTube API. 1 " + r1.text);
    }
    std::string upload_url = r1.header["Location"];
    cpr::Response r2 = cpr::Put(cpr::Url{upload_url},
                                cpr::Header{{"Authorization", "Bearer " + getAccessToken()},
                                            {"Content-Type", "video/*"}},
                                cpr::Body{cpr::File{video_path}});
    if (r2.status_code != 200) {
        throw std::runtime_error("Failed to upload video to YouTube API. 2 " + r2.text);
    }
}

bool YouTubeApi::isCredentialsValid(const std::string& client_id, const std::string& client_secret) {
    std::string redirect_uri = "urn:ietf:wg:oauth:2.0:oob";
    // Step 1: Get the device code
    std::string auth_url = "https://accounts.google.com/o/oauth2/v2/auth?"
                           "client_id=" + client_id +
                           "&response_type=code"
                           "&scope=https://www.googleapis.com/auth/youtube.upload"
                           "&redirect_uri=" + redirect_uri;


    // Step 2: Ask the user to go to the verification URL and enter the user code
    std::cout << "Go to the following URL in your browser:\n" << auth_url << std::endl;

    // Step 2: User grants access and is redirected back to the application with an authorization code
    std::string auth_code;
    std::cout << "Enter the authorization code: ";
    std::cin >> auth_code;

    // Step 3: Exchange the authorization code for an access token
    cpr::Response r = cpr::Post(cpr::Url{"https://oauth2.googleapis.com/token"},
                                cpr::Payload{{"client_id", client_id},
                                             {"client_secret", client_secret},
                                             {"code", auth_code},
                                             {"grant_type", "authorization_code"},
                                             {"redirect_uri", redirect_uri}});
    
    return r.status_code == 200;
}