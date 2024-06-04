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
   private:
    std::string access_token;
    std::string client_id;
    std::string client_secret;
   public:
    TwitchApi(const std::string& client_id, const std::string& client_secret) : client_id(client_id), client_secret(client_secret) {
        initOAuthToken();
    }
    /**
     * Retrieves the OAuth token from the Twitch API.
     * @throws std::runtime_error if the access token cannot be obtained.
     */
    void initOAuthToken();

    /**
     * Retrieves the top games from the Twitch API.
     *
     * @param count The number of top games to retrieve.
     * @return A map of game names and their id.
     * @throws std::runtime_error if the top games cannot be obtained.
     */
    std::map<std::string, std::string> getTopGames(const int& count);

    /**
     * Retrieves the download URLs of the top clips for a specific game within a certain time span from the Twitch API.
     *
     * @param game_id The ID of the game.
     * @param hours The number of hours in the past to retrieve clips from.
     * @return A vector of clip download URLs.
     * @throws std::runtime_error if the clip URLs cannot be obtained.
     */
    std::vector<std::string> getTopClipsInTimeSpan(const std::string& game_id, const int& hours, const int& clip_amount);
};

#endif 