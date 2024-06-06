#include <credential_encryption.hpp>

std::string CredentialEncryption::bitOrEncrypt(const std::string& value, const std::string& encryption_key) {
    if (value.empty() || encryption_key.empty()) {
        return value;
    }
    std::string encrypted_value = "";
    for (size_t i = 0; i < value.size(); ++i) {
        encrypted_value += static_cast<char>(value[i] ^ encryption_key[i % encryption_key.size()]);
    }
    return encrypted_value;
}

std::string CredentialEncryption::bitOrDecrypt(const std::string& value, const std::string& encryption_key) {
    if (value.empty() || encryption_key.empty()) {
        return value;
    }
    std::string decrypted_value = "";
    for (size_t i = 0; i < value.size(); ++i) {
        decrypted_value += static_cast<char>(value[i] ^ encryption_key[i % encryption_key.size()]);
    }
    return decrypted_value;
}