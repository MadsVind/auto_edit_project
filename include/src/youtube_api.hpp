#ifndef YOUTUBE_API_HPP
#define YOUTUBE_API_HPP

#include <string>
#include <stdexcept>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <map>
#include <vector>
#include <iostream>

class YouTubeApi {
   private:
    std::string access_token;
    std::string client_id;
    std::string client_secret;
   public:
    YouTubeApi(const std::string& client_id, const std::string& client_secret) : client_id(client_id), client_secret(client_secret) {
        initOAuthToken();
    }

    void initOAuthToken();

    void uploadVideo(const std::string& video_path, 
                     const std::string& title, 
                     const std::string& description, 
                     const std::string& category_id, 
                     const std::vector<std::string>& tags);
};

#endif 