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

class App {
   public:
    App(const std::string& client_id, const std::string& client_secret) : twitch_con(client_id, client_secret) {};

    void setGameID(const std::string& game_id) {this->game_id = game_id;};
    std::string getGameID() {return game_id;};
    void setClipAmount(const int& clipAmount) {this->clipAmount = clipAmount;};
    int getClipAmount() {return clipAmount;};
    void setClipTimeSpan(const int& timeSpanDays) {this->timeSpanDays = timeSpanDays;};
    int getClipTimeSpan() {return timeSpanDays;};


    void queryClipAmount(); 
    void queryClipTimeSpan();
    void downloadClip(const std::string& url, const std::string& file_name);
    void setGame();
    void choseClips();
    void menu();
   private:
    const std::string encryption_key = "simple_key";
    const std::string credentials_file_name = "../credentials.txt";

    std::string game_id = "32399";
    int timeSpanDays = 7;
    int clipAmount = 10;
    TwitchApi twitch_con;
};

#endif
