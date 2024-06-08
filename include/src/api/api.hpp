#ifndef API_HPP
#define API_HPP

#include <string>
#include <stdexcept>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <map>
#include <vector>
#include <iostream>

class Api {
   protected:
    std::string access_token;
    std::string client_id;
    std::string client_secret;
    std::string token_url;

    /**
     * Retrieves the OAuth token from the API.
     * Set access_token to "-1" if the token cannot be obtained.
     */
    virtual void initOAuthToken();

    /**
     * @brief Get the client ID.
     * 
     * This function returns the client ID.
     * 
     * @return The client ID as a string.
     */
    std::string getClientId() { return client_id; }

    /**
     * @brief Get the client secret.
     * 
     * This function returns the client secret used for authentication.
     * 
     * @return The client secret as a string.
     */
    std::string getClientSecret() { return client_secret; }

   public:
    Api() {} 
    Api(const std::string& token_url) : token_url(token_url) {} 
    Api(const std::string& access_token, const std::string& token_url) : access_token(access_token), token_url(token_url) {}
    Api(const std::string& client_id, const std::string& client_secret, const std::string& token_url)
        : client_id(client_id), client_secret(client_secret), token_url(token_url) {
        initOAuthToken();
    }

    /**
     * @brief Checks if the token is valid.
     * 
     * @return true if the token is valid, false otherwise.
     */
    virtual bool isTokenValid() { return access_token != "-1"; } 
    
    /**
     * @brief Sets the credentials for the API.
     * 
     * This function sets the client ID and client secret for the API authentication.
     * 
     * @param client_id The client ID to be set.
     * @param client_secret The client secret to be set.
     */
    void setCredentials(const std::string& client_id, const std::string& client_secret);

     /**
     * @brief Retrieves the access token.
     *
     * This function returns the access token used for authentication.
     *
     * @return The access token as a string.
     */
    std::string getAccessToken() { return access_token; }

    /**
     * @brief Sets the access token for the API.
     * 
     * This function sets the access token that will be used for authentication when making API requests.
     * 
     * @param access_token The access token to set.
     */
    void setAccessToken(const std::string& access_token) { this->access_token = access_token; }


    void setClientId(const std::string& client_id) { this->client_id = client_id; }
};

#endif 