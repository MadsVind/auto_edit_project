#ifndef  CREDENTIAL_ENCRYPTION_HPP
#define CREDENTIAL_ENCRYPTION_HPP

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

/**
 * @brief The CredentialEncryption class provides methods for encrypting and decrypting credentials.
 */
class CredentialEncryption {
public:
    /**
     * @brief Writes the encrypted key-value pair to a file using the specified encryption key.
     * 
     * @param key The key to be encrypted and written to the file.
     * @param value The value to be encrypted and written to the file.
     * @param encryption_key The encryption key used to encrypt the key-value pair.
     * @param filename The name of the file to write the encrypted data to.
     */
    void writeEncryptedToFile(const std::string& key, const std::string& value, const std::string& encryption_key, const std::string& filename);

    /**
     * @brief Reads and decrypts the value associated with the specified key from a file using the specified encryption key.
     * 
     * @param key The key to be decrypted and used to retrieve the value from the file.
     * @param encryption_key The encryption key used to decrypt the value associated with the key.
     * @param filename The name of the file to read the encrypted data from.
     * @return The decrypted value associated with the specified key.
     */
    std::string readDecryptedFromFile(const std::string& key, const std::string& encryption_key, const std::string& filename);
};

#endif 