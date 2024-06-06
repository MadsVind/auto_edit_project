// test_credential_encryption.cpp
#define CATCH_CONFIG_MAIN

#include "test_credential_encryption.hpp"
CredentialEncryption ce;

TEST_CASE("bitOrEncrypt and bitOrDecrypt", "[CredentialEncryption]") {
    std::string value = "testValue";
    std::string encryption_key = "testEncryptionKey";

    std::string encrypted_value = ce.bitOrEncrypt(value, encryption_key);
    REQUIRE(encrypted_value != value);

    std::string decrypted_value = ce.bitOrDecrypt(encrypted_value, encryption_key);
    REQUIRE(decrypted_value == value);
}

TEST_CASE("bitOrEncrypt and bitOrDecrypt with empty value", "[CredentialEncryption]") {
    std::string value = "";
    std::string encryption_key = "testEncryptionKey";

    std::string encrypted_value = ce.bitOrEncrypt(value, encryption_key);
    REQUIRE(encrypted_value == value);

    std::string decrypted_value = ce.bitOrDecrypt(encrypted_value, encryption_key);
    REQUIRE(decrypted_value == value);
}

TEST_CASE("bitOrEncrypt and bitOrDecrypt with empty encryption key", "[CredentialEncryption]") {
    std::string value = "testValue";
    std::string encryption_key = "";

    std::string encrypted_value = ce.bitOrEncrypt(value, encryption_key);
    REQUIRE(encrypted_value == value);

    std::string decrypted_value = ce.bitOrDecrypt(encrypted_value, encryption_key);
    REQUIRE(decrypted_value == value);
}