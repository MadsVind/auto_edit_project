#ifndef TWITCH_API_HPP
#define TWITCH_API_HPP

#include <string>
#include <stdexcept>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

class TwitchApi {
public:
    /**
     * Retrieves the OAuth token from the Twitch API.
     *
     * @param client_id The client ID.
     * @param client_secret The client secret.
     * @return The OAuth token.
     * @throws std::runtime_error if the access token cannot be obtained.
     */
    std::string getOAuthToken(const std::string& client_id, const std::string& client_secret);
    /**
     * Retrieves the download URL for a Twitch clip.
     *
     * @param access_token The access token.
     * @param client_id The client ID.
     * @param clip_id The clip ID.
     * @return The download URL.
     * @throws std::runtime_error if the clip download URL cannot be obtained.
     */
    std::string getClipDownloadUrl(const std::string& access_token, const std::string& client_id, const std::string& clip_id);
};

#endif 