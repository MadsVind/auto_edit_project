#ifndef CREDENTIAL_ENCRYPTION_HPP
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
     * @brief Encrypts the value using the specified encryption key and returns the encrypted value.
     * 
     * @param value The value to be encrypted.
     * @param encryption_key The encryption key used to encrypt the key-value pair.
     * @return The encrypted value.
     */
    std::string bitOrEncrypt(const std::string& value, const std::string& encryption_key);

    /**
     * @brief Decrypts the value using the specified encryption key.
     * 
     * @param key The value to be decrypted.
     * @param encryption_key The encryption key used to decrypt.
     * @return The decrypted value.
     */
    std::string bitOrDecrypt(const std::string& value, const std::string& encryption_key);
};

#endif 