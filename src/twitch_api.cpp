#include <twitch_api.hpp>

std::string TwitchApi::getOAuthToken(const std::string& client_id, const std::string& client_secret) {
    cpr::Response r = cpr::Post(cpr::Url{"https://id.twitch.tv/oauth2/token"},
                                cpr::Payload{{"client_id", client_id},
                                             {"client_secret", client_secret},
                                             {"grant_type", "client_credentials"}});

    auto json = nlohmann::json::parse(r.text);
    if(json.contains("access_token") && !json["access_token"].is_null()) {
        return json["access_token"];
    } else {
        throw std::runtime_error("Failed to get access token");
    }
}

std::string TwitchApi::getClipDownloadUrl(const std::string& access_token, const std::string& client_id, const std::string& clip_id) {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.twitch.tv/helix/clips"},
                               cpr::Header{{"Client-ID", client_id},
                                           {"Authorization", "Bearer " + access_token}},
                               cpr::Parameters{{"id", clip_id}});

    auto json = nlohmann::json::parse(r.text);
    if(json.contains("data") && !json["data"].is_null() && !json["data"][0]["thumbnail_url"].is_null()) {
        std::string thumbnail_url = json["data"][0]["thumbnail_url"];
        std::string download_url = thumbnail_url.substr(0, thumbnail_url.find("-preview")) + ".mp4";
        return download_url;
    } else {
        throw std::runtime_error("Failed to get clip download URL");
    }
}