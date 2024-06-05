#include "io_util.hpp"

void clearConsole() {
    try {
        system("clear");
    } catch (const std::exception& e) {
        system("cls");
    }
}

bool checkAndCreateDirectory(const std::string& dir) {
    struct stat info;

    if (stat(dir.c_str(), &info) != 0) {
        if (mkdir(dir.c_str(), 0777) == -1) {
            std::cerr << "Error :  " << strerror(errno) << std::endl;
            return false;
        }
    } else if (!(info.st_mode & S_IFDIR)) {
        std::cerr << dir << " is not a directory" << std::endl;
        return false;
    }

    return true;
}

int queryInt() {
    std::string input;
    std::getline(std::cin, input);
    std::cin.ignore();
    if (input.empty()) {
        return -1;
    } else {
        try {
            int integer = std::stoi(input);
            return integer;
        } catch (std::invalid_argument&) {
            std::cout << "Invalid input. Please enter an integer.\n>> ";
            return queryInt();
        }
    }
}

void deleteAllFilesInFolder(const std::string& folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::filesystem::remove(entry);
        }
    }
}
