#include <credential_encryption.hpp>

void CredentialEncryption::writeEncryptedToFile(const std::string& key, const std::string& value, const std::string& encryption_key, const std::string& filename) {
    nlohmann::json j;
    
    std::ifstream file(filename);
    if (file) {
        file >> j;
        file.close();
    }

    std::string encrypted_value;
    for (char c : value) {
        encrypted_value += static_cast<char>(c ^ encryption_key[0]);
    }

    j[key] = encrypted_value;

    std::ofstream outfile(filename);
    outfile << j;
    outfile.close();
}

std::string CredentialEncryption::readDecryptedFromFile(const std::string& key, const std::string& encryption_key, const std::string& filename) {
    std::ifstream file(filename);
    nlohmann::json j;
    file >> j;
    file.close();

    std::string encrypted_value = j[key];

    std::string client_value;
    for (char c : encrypted_value) {
        client_value += static_cast<char>(c ^ encryption_key[0]);
    }
  
    return client_value;
}