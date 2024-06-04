#ifndef APP_HPP
#define APP_HPP

#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <twitch_api.hpp>
#include <vector>
#include <map>
#include <thread>
#include <sys/stat.h>
#include <video_editor.hpp>

/**
 * @brief The App class represents an application that interacts with the Twitch API.
 */
class App {
   public:
    /**
     * @brief Constructs an App object with the specified client ID and client secret.
     * @param client_id The client ID used for authentication with the Twitch API.
     * @param client_secret The client secret used for authentication with the Twitch API.
     */
    App(const std::string& client_id, const std::string& client_secret) : twitch_con(client_id, client_secret) {};

    /**
     * @brief Displays the menu for the application.
     */
    void menu();

   private:
    const std::string encryption_key = "simple_key";
    const std::string credentials_file_name = "../credentials.txt";

    std::string game_id = "32399";
    int timeSpanDays = 7;
    int clipAmount = 10;
    TwitchApi twitch_con;

    /**
     * @brief Sets the game ID to the specified value.
     * @param game_id The new game ID.
     */
    void setGameID(const std::string& game_id) {this->game_id = game_id;};

    /**
     * @brief Returns the current game ID.
     * @return The current game ID.
     */
    std::string getGameID() {return game_id;};

    /**
     * @brief Sets the clip amount to the specified value.
     * @param clipAmount The new clip amount.
     */
    void setClipAmount(const int& clipAmount) {this->clipAmount = clipAmount;};

    /**
     * @brief Returns the current clip amount.
     * @return The current clip amount.
     */
    int getClipAmount() {return clipAmount;};

    /**
     * @brief Sets the clip time span to the specified value.
     * @param timeSpanDays The new clip time span in days.
     */
    void setClipTimeSpan(const int& timeSpanDays) {this->timeSpanDays = timeSpanDays;};

    /**
     * @brief Returns the current clip time span.
     * @return The current clip time span in days.
     */
    int getClipTimeSpan() {return timeSpanDays;};

    /**
     * @brief Downloads a clip from the specified URL and saves it with the specified file name.
     * @param url The URL of the clip to download.
     * @param file_name The file name to save the downloaded clip.
     */
    void downloadClip(const std::string& url, const std::string& file_name);

    /**
     * @brief Queries the Twitch API for game information.
     */
    void queryGame();

    /**
     * @brief Allows the user to choose clips from the queried game.
     */
    void choseClips();

    /**
     * @brief Queries the user for the desired clip amount.
     */
    void queryClipAmount(); 

    /**
     * @brief Queries the user for the desired clip time span.
     */
    void queryClipTimeSpan();
};

#endif
