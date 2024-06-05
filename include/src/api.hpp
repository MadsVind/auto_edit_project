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
    void initOAuthToken();

    /**
     * @brief Retrieves the access token.
     *
     * This function returns the access token used for authentication.
     *
     * @return The access token as a string.
     */
    std::string getAccessToken() { return access_token; }

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
    Api(const std::string& client_id, const std::string& client_secret, const std::string& token_url)
        : client_id(client_id), client_secret(client_secret), token_url(token_url) {
        initOAuthToken();
    }

    /**
     * @brief Checks if the token is valid.
     * 
     * @return true if the token is valid, false otherwise.
     */
    bool isTokenValid() { return access_token != "-1"; } 

    /**
     * @brief Checks if the provided credentials are valid.
     *
     * This function verifies the validity of the provided client ID and client secret.
     *
     * @param client_id The client ID to be validated.
     * @param client_secret The client secret to be validated.
     * @return true if the credentials are valid, false otherwise.
     */
    bool isCredentialsValid(const std::string& client_id, const std::string& client_secret);

    /**
     * @brief Sets the credentials for the API.
     * 
     * This function sets the client ID and client secret for the API authentication.
     * 
     * @param client_id The client ID to be set.
     * @param client_secret The client secret to be set.
     */
    void setCredentials(const std::string& client_id, const std::string& client_secret);
};

#endif 