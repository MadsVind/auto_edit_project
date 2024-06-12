#ifndef YOUTUBE_API_HPP
#define YOUTUBE_API_HPP

#include <api.hpp>

class YouTubeApi : public Api {
   public:
    YouTubeApi() {} // imediate initialization
    YouTubeApi(const std::string& client_id, const std::string& client_secret) 
        : Api(client_id, client_secret, "https://www.googleapis.com/oauth2/v4/token") {}


    void uploadVideo(const std::string& video_path, 
                     const std::string& title, 
                     const std::string& description, 
                     const std::string& category_id, 
                     const std::vector<std::string>& tags);
};

#endif 