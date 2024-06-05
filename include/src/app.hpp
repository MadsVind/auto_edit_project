#ifndef APP_HPP
#define APP_HPP

#include <video_editor.hpp>
#include <thread>
#include <settings.hpp>

/**
 * @brief The App class represents an application that interacts with the Twitch API.
 */
class App {
   private:
    Settings settings;

    /**
     * @brief Downloads a clip from the specified URL and saves it with the specified file name.
     * @param url The URL of the clip to download.
     * @param file_name The file name to save the downloaded clip.
     */
    void downloadClip(const std::string& url, const std::string& file_name);

    /**
     * @brief Allows the user to choose clips from the queried game.
     */
    void choseClips();

    /**
     * @brief Builds a video.
     * 
     * This function is responsible for building a video.
     * It performs the necessary operations to create a video.
     * 
     * @return void
     */
    void buildVideo();

   public:

    /**
     * @brief Displays the menu for the application.
     */
    void menu();
};

#endif
