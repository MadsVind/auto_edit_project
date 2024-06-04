#include <youtube_api.hpp>

void YouTubeApi::initOAuthToken() {
    cpr::Response r = cpr::Post(cpr::Url{"https://www.googleapis.com/oauth2/v4/token"},
                                cpr::Parameters{{"client_id", client_id},
                                                {"client_secret", client_secret},
                                                {"grant_type", "client_credentials"}});
    if (r.status_code != 200) {
        throw std::runtime_error("Failed to obtain access token from YouTube API.");
    }
    nlohmann::json j = nlohmann::json::parse(r.text);
    access_token = j["access_token"];
}

void YouTubeApi::uploadVideo(const std::string& video_path, 
                             const std::string& title, 
                             const std::string& description, 
                             const std::string& category_id, 
                             const std::vector<std::string>& tags) {
    cpr::Response r1 = cpr::Post(cpr::Url{"https://www.googleapis.com/upload/youtube/v3/videos"},
                                 cpr::Header{{"Authorization", "Bearer " + access_token},
                                             {"Content-Type", "video/*"}},
                                 cpr::Parameters{{"uploadType", "resumable"},
                                                 {"part", "snippet,status"},
                                                 {"key", client_id}},
                                 cpr::Body{"snippet", {{"categoryId", category_id},
                                                       {"title", title},
                                                         {"description", description},
                                                         {"tags", tags}},
                                            "status", {{"privacyStatus", "public"}}});
    if (r1.status_code != 200) {
        throw std::runtime_error("Failed to upload video to YouTube API.");
    }
    std::string upload_url = r1.header["Location"];
    cpr::Response r2 = cpr::Put(cpr::Url{upload_url},
                                cpr::Header{{"Authorization", "Bearer " + access_token},
                                            {"Content-Type", "video/*"}},
                                cpr::Body{cpr::File{video_path}});
    if (r2.status_code != 200) {
        throw std::runtime_error("Failed to upload video to YouTube API.");
    }
}