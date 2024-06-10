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

    const std::string BASE_FILE_NAME = "clip.mp4";
    const std::string PATH = "clips/";
    const std::string RESULT_PATH = "output/";
    const std::string RESULT_FILE_NAME = "output.mp4";

    /**
     * @brief Downloads a clip from the specified URL and saves it with the specified file name.
     * @param url The URL of the clip to download.
     * @param file_name The file name to save the downloaded clip.
     */
    void downloadClip(const std::string& url, const std::string& file_name);

    /**
     * Downloads clips from the specified URLs.
     *
     * @param clip_urls A vector of strings containing the URLs of the clips to download.
     */
    void downloadClips(const std::vector<std::string>& clip_urls);

    /**
     * @brief Allows the user to choose clips from the queried game.
     */
    void choseClips();

    /**
     * Edits a video located at the specified path.
     *
     * @param video_path The path to the video file.
     */
    void editVideo(const std::string& video_path);

    /**
     * @brief Builds a video.
     * 
     * This function is responsible for building a video.
     * It performs the necessary operations to create a video.
     * 
     * @return void
     */
    void buildVideo();

    /**
     * @brief Uploads a video.
     * 
     * This function is responsible for uploading a video to a specified destination.
     * It handles the necessary steps to upload the video file, such as establishing a connection,
     * authenticating the user, and transferring the file.
     * 
     * @note This function assumes that the video file is already available and accessible.
     */
    void uploadVideo();

   public:

    /**
     * @brief Displays the menu for the application.
     */
    void menu();
};

#endif
