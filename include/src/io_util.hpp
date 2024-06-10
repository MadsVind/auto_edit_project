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
 * @brief Checks if a file exists and creates it if it doesn't.
 *
 * This function checks if the specified file exists. If the file does not exist, it creates
 * a new file with the specified filename.
 *
 * @param filename The name of the file to check and create.
 * @return True if the file exists or was successfully created, false otherwise.
 */
bool checkAndCreateFile(const std::string& filename);

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