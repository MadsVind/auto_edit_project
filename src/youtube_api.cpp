#include <youtube_api.hpp>

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