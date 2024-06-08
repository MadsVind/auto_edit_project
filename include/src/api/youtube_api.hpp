#ifndef YOUTUBE_API_HPP
#define YOUTUBE_API_HPP

#include <api.hpp>

class YouTubeApi : public Api {
   public:
    YouTubeApi() : Api("https://www.googleapis.com/oauth2/v4/token") {} // imediate initialization
    YouTubeApi(const std::string& access_token) : Api(access_token, "https://www.googleapis.com/oauth2/v4/token") {}
    YouTubeApi(const std::string& client_id, const std::string& client_secret) 
        : Api(client_id, client_secret, "https://www.googleapis.com/oauth2/v4/token") {}


    /**
     * Uploads a video to YouTube.
     *
     * @param video_path The path to the video file.
     * @param title The title of the video.
     * @param description The description of the video.
     * @param category_id The ID of the category to which the video belongs.
     * @param tags The tags associated with the video.
     */
    void uploadVideo(const std::string& video_path, 
                     const std::string& title, 
                     const std::string& description, 
                     const std::string& category_id, 
                     const std::vector<std::string>& tags);

    /**
     * @brief Initializes the OAuth token for the YouTube API.
     * 
     * This function is responsible for initializing the OAuth token required for making authenticated requests to the YouTube API.
     * It should be called before making any API requests.
     * 
     * @note This function must be implemented by the derived class.
     */
    void initOAuthToken() override;

    /**
     * @brief Checks if the token is valid.
     * 
     * This function checks whether the token is valid or not. It returns true if the token is valid,
     * and false otherwise.
     * 
     * @return true if the token is valid, false otherwise.
     */
    bool isTokenValid() override;
};

#endif 