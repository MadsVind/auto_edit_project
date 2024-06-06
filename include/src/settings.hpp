#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <credential_encryption.hpp>
#include <nlohmann/json.hpp>
#include <twitch_api.hpp>
#include <youtube_api.hpp>
#include <vector>
#include <map>
#include <io_util.hpp>

class Settings {
   public:
    Settings() : twitch_con(), youtube_con() { initSettings(); };
    void menu();
    std::vector<std::string> getClipsUrls() {return twitch_con.getTopClipsInTimeSpan(game_id, time_span_hours, clip_amount);};

    TwitchApi getTwitchApi() {return twitch_con;};
    YouTubeApi getYouTubeApi() {return youtube_con;};

   private:
    const std::string encryption_key = "simple_key";
    const std::string settings_file_name = "../settings.txt";
    const int HOURS_IN_DAY = 24;

    std::string game_id;
    int time_span_hours;
    int clip_amount;
    TwitchApi twitch_con;
    YouTubeApi youtube_con;
    CredentialEncryption ce;

    /**
     * @brief Initializes the settings.
     * 
     * This function is responsible for initializing the settings of the application.
     * It performs any necessary setup or initialization tasks related to the settings.
     * 
     * @note This function should be called before using any settings-related functionality.
     */
    void initSettings(); 

    /**
     * @brief Queries the Twitch API for game information.
     */
    void queryGame();

    /**
     * @brief Queries the user for the desired clip amount.
     */
    void queryClipAmount(); 

    /**
     * @brief Queries the user for the desired clip time span.
     */
    void queryClipTimeSpan();

    
    /**
     * Queries the credentials for a given service and sets them in the provided API object.
     *
     * @param service The name of the service for which to query the credentials.
     * @param api     A pointer to the API object where the credentials will be set.
     */
    void queryCredentials(const std::string& service, Api* api);

   public:
    /**
     * Writes the given key-value pair to the settings file.
     *
     * @param key The key to write.
     * @param value The value to write.
     */
    void writeToSettingsFile(const std::string& key, const std::string& value);

    /**
     * Reads the value associated with the specified key from the settings file.
     *
     * @return The value associated with the key, or an empty string if the key is not found.
     * 
     * @param key The key to look up in the settings file.
     */
    std::string readFromSettingsFile(const std::string& key);
};

#endif
