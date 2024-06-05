// test_credential_encryption.cpp
#define CATCH_CONFIG_MAIN

#include "test_credential_encryption.hpp"

TEST_CASE("bitOrEncrypt and bitOrDecrypt", "[CredentialEncryption]") {
    CredentialEncryption ce;
    std::string value = "testValue";
    std::string encryption_key = "testEncryptionKey";

    std::string encrypted_value = ce.bitOrEncrypt(value, encryption_key);
    REQUIRE(encrypted_value != value);

    std::string decrypted_value = ce.bitOrDecrypt(encrypted_value, encryption_key);
    REQUIRE(decrypted_value == value);
}

TEST_CASE("bitOrEncrypt and bitOrDecrypt with empty value", "[CredentialEncryption]") {
    CredentialEncryption ce;
    std::string value = "";
    std::string encryption_key = "testEncryptionKey";

    std::string encrypted_value = ce.bitOrEncrypt(value, encryption_key);
    REQUIRE(encrypted_value == value);

    std::string decrypted_value = ce.bitOrDecrypt(encrypted_value, encryption_key);
    REQUIRE(decrypted_value == value);
}

TEST_CASE("bitOrEncrypt and bitOrDecrypt with empty encryption key", "[CredentialEncryption]") {
    CredentialEncryption ce;
    std::string value = "testValue";
    std::string encryption_key = "";

    std::string encrypted_value = ce.bitOrEncrypt(value, encryption_key);
    REQUIRE(encrypted_value == value);

    std::string decrypted_value = ce.bitOrDecrypt(encrypted_value, encryption_key);
    REQUIRE(decrypted_value == value);
}