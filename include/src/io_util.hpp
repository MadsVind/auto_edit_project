#ifndef IO_UTIL_HPP
#define IO_UTIL_HPP

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <filesystem>

/**
 * @brief Clears the console screen.
*/
void clearConsole();

/**
 * @brief Checks if a directory exists and creates it if it does not.
 * @param dir The directory to check and create.
 * @return true if the directory exists or was successfully created, false otherwise.
*/
bool checkAndCreateDirectory(const std::string& dir);

/**
 * @brief Queries the user for an integer input.
 * @return The integer input.
*/
int queryInt();

/**
 * @brief Deletes all files in a folder.
 * @param folderPath The path to the folder.
*/
void deleteAllFilesInFolder(const std::string& folderPath);

#endif // IO_UTIL_H