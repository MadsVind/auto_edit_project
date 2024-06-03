#ifndef TWITCH_API_HPP
#define TWITCH_API_HPP

#include <string>
#include <stdexcept>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <map>
#include <vector>
#include <iostream>

class TwitchApi {
   public:
    void setClientID(const std::string& client_id) { this->client_id = client_id; }
    void setClientSecret(const std::string& client_secret) { this->client_secret = client_secret; }

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
     * Retrieves the top games from the Twitch API.
     *
     * @param access_token The access token.
     * @param client_id The client ID.
     * @param count The number of top games to retrieve.
     * @return A map of game names and their id.
     * @throws std::runtime_error if the top games cannot be obtained.
     */
    std::map<std::string, std::string> getTopGames(const std::string& access_token, const std::string& client_id, const int& count);

    /**
     * Retrieves the download URLs of the top clips for a specific game within a certain time span from the Twitch API.
     *
     * @param access_token The access token.
     * @param client_id The client ID.
     * @param game_id The ID of the game.
     * @param hours The number of hours in the past to retrieve clips from.
     * @return A vector of clip download URLs.
     * @throws std::runtime_error if the clip URLs cannot be obtained.
     */
    std::vector<std::string> getTopClipsInTimeSpan(const std::string& access_token, const std::string& client_id, const std::string& game_id, const int& hours);

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