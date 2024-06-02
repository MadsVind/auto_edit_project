// test_credential_encryption.cpp
#define CATCH_CONFIG_MAIN¨

#include "test_credential_encryption.hpp"

TEST_CASE("writeEncryptedToFile and readDecryptedFromFile", "[CredentialEncryption]") {
    CredentialEncryption ce;
    std::string key = "testKey";
    std::string value = "testValue";
    std::string encryption_key = "testEncryptionKey";
    std::string filename = "testFile.json";

    // Clean up from any previous tests
    std::remove(filename.c_str());

    ce.writeEncryptedToFile(key, value, encryption_key, filename);

    std::ifstream file(filename);
    REQUIRE(file.is_open());

    nlohmann::json j;
    file >> j;
    file.close();

    REQUIRE(j[key] != value);

    std::string decrypted_value = ce.readDecryptedFromFile(key, encryption_key, filename);
    REQUIRE(decrypted_value == value);

    // Clean up after tests
    std::remove(filename.c_str());
}

TEST_CASE("writeEncryptedToFile and readDecryptedFromFile with empty key", "[CredentialEncryption]") {
    CredentialEncryption ce;
    std::string key = "";
    std::string value = "testValue";
    std::string encryption_key = "testEncryptionKey";
    std::string filename = "testFile.json";

    std::remove(filename.c_str());

    ce.writeEncryptedToFile(key, value, encryption_key, filename);

    std::ifstream file(filename);
    REQUIRE(file.is_open());

    nlohmann::json j;
    file >> j;
    file.close();

    REQUIRE(j[key] != value);

    std::string decrypted_value = ce.readDecryptedFromFile(key, encryption_key, filename);
    REQUIRE(decrypted_value == value);

    std::remove(filename.c_str());
}

TEST_CASE("writeEncryptedToFile and readDecryptedFromFile with empty value", "[CredentialEncryption]") {
    CredentialEncryption ce;
    std::string key = "testKey";
    std::string value = "";
    std::string encryption_key = "testEncryptionKey";
    std::string filename = "testFile.json";

    std::remove(filename.c_str());

    ce.writeEncryptedToFile(key, value, encryption_key, filename);

    std::ifstream file(filename);
    REQUIRE(file.is_open());

    nlohmann::json j;
    file >> j;
    file.close();

    REQUIRE(j[key] == value);

    std::string decrypted_value = ce.readDecryptedFromFile(key, encryption_key, filename);
    REQUIRE(decrypted_value == value);

    std::remove(filename.c_str());
}

TEST_CASE("writeEncryptedToFile and readDecryptedFromFile with non-existing key", "[CredentialEncryption]") {
    CredentialEncryption ce;
    std::string key = "testKey";
    std::string value = "testValue";
    std::string encryption_key = "testEncryptionKey";
    std::string filename = "testFile.json";

    std::remove(filename.c_str());

    ce.writeEncryptedToFile(key, value, encryption_key, filename);

    std::ifstream file(filename);
    REQUIRE(file.is_open());

    nlohmann::json j;
    file >> j;
    file.close();

    REQUIRE(j[key] != value);

    std::string non_existing_key = "nonExistingKey";
    std::string decrypted_value = ce.readDecryptedFromFile(non_existing_key, encryption_key, filename);
    REQUIRE(decrypted_value == "");

    std::remove(filename.c_str());
}
