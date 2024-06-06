#ifndef TWITCH_API_HPP
#define TWITCH_API_HPP

#include <api.hpp>

class TwitchApi : public Api {
   public:
    TwitchApi() {} // imediate initialization
    TwitchApi(const std::string& client_id, const std::string& client_secret)
        : Api(client_id, client_secret, "https://id.twitch.tv/oauth2/token") {}

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