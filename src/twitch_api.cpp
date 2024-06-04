#include <twitch_api.hpp>

void TwitchApi::initOAuthToken() {
    cpr::Response r = cpr::Post(cpr::Url{"https://id.twitch.tv/oauth2/token"},
                                cpr::Payload{{"client_id", client_id},
                                             {"client_secret", client_secret},
                                             {"grant_type", "client_credentials"}});

    auto json = nlohmann::json::parse(r.text);
    if(json.contains("access_token") && !json["access_token"].is_null()) {
        access_token = json["access_token"];
    } else {
        throw std::runtime_error("Failed to get access token");
    }
}

std::map<std::string, std::string> TwitchApi::getTopGames(const int& count) {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.twitch.tv/helix/games/top"},
                               cpr::Header{{"Client-ID", client_id},
                                           {"Authorization", "Bearer " + access_token}},
                               cpr::Parameters{{"first", std::to_string(count)}});

    auto json = nlohmann::json::parse(r.text);
    std::map<std::string, std::string> games;
    if(json.contains("data") && !json["data"].is_null()) {
        for(const auto& game : json["data"]) {
            if(!game["name"].is_null()) {
                games[game["name"]] = game["id"];
            }
        }
    } else {
        throw std::runtime_error("Failed to get top games");
    }

    return games;
}

std::vector<std::string> TwitchApi::getTopClipsInTimeSpan(const std::string& game_id, const int& hours, const int& clip_amount) {
    auto now = std::chrono::system_clock::now();
    auto week_ago = now - std::chrono::hours(hours);
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto week_ago_time_t = std::chrono::system_clock::to_time_t(week_ago);

    std::stringstream now_ss;
    now_ss << std::put_time(std::gmtime(&now_time_t), "%FT%TZ"); // Format to RFC3339
    std::stringstream week_ago_ss;
    week_ago_ss << std::put_time(std::gmtime(&week_ago_time_t), "%FT%TZ"); // Format to RFC3339

    cpr::Response r = cpr::Get(cpr::Url{"https://api.twitch.tv/helix/clips"},
                               cpr::Header{{"Client-ID", client_id},
                                           {"Authorization", "Bearer " + access_token}},
                               cpr::Parameters{{"game_id", game_id},
                                               {"started_at", week_ago_ss.str()},
                                               {"ended_at", now_ss.str()},
                                               {"first", std::to_string(clip_amount)}});

    auto json = nlohmann::json::parse(r.text);

    std::vector<std::string> clip_download_urls;

    if(json.contains("data") && !json["data"].is_null()) {
        for(const auto& clip : json["data"]) {
            if(!clip["thumbnail_url"].is_null()) {
                std::string thumbnail_url = clip["thumbnail_url"];
                std::string download_url = thumbnail_url.substr(0, thumbnail_url.find("-preview")) + ".mp4";
                clip_download_urls.push_back(download_url);
            } else {
                throw std::runtime_error("Failed to get clip download URL: 'thumbnail_url' field is missing");
            }
        }
    } else {
        std::cerr << "Response text: " << r.text << std::endl;
        throw std::runtime_error("Failed to get clip download URLs: 'data' field is missing or null");
    }

    return clip_download_urls;
}