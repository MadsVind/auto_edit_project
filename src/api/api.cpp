#include "api.hpp"

void Api::initOAuthToken() {
    cpr::Response r = cpr::Post(cpr::Url{token_url},
                                cpr::Payload{{"client_id", client_id},
                                             {"client_secret", client_secret},
                                             {"grant_type", "client_credentials"}});

    auto json = nlohmann::json::parse(r.text);
    if(json.contains("access_token") && !json["access_token"].is_null()) {
        access_token = json["access_token"];
    } else {
        access_token = "-1";
    }
}

bool Api::isCredentialsValid(const std::string& client_id, const std::string& client_secret) {
    cpr::Response r = cpr::Post(cpr::Url{token_url},
                                cpr::Payload{{"client_id", client_id},
                                             {"client_secret", client_secret},
                                             {"grant_type", "client_credentials"}});

    auto json = nlohmann::json::parse(r.text);
    std::cout << r.text << std::endl;
    return json.contains("access_token") && !json["access_token"].is_null();
}

void Api::setCredentials(const std::string& client_id, const std::string& client_secret) {
    this->client_id = client_id;
    this->client_secret = client_secret;
    initOAuthToken();
}
