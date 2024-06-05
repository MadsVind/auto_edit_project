#include <credential_encryption.hpp>

std::string CredentialEncryption::bitOrEncrypt(const std::string& value, const std::string& encryption_key) {
    std::string encrypted_value;
    for (char c : value) {
        encrypted_value += static_cast<char>(c ^ encryption_key[0]);
    }
    return encrypted_value;
}

std::string CredentialEncryption::bitOrDecrypt(const std::string& value, const std::string& encryption_key) {
    std::string client_value;
    for (char c : value) {
        client_value += static_cast<char>(c ^ value[0]);
    }
    return client_value;
}